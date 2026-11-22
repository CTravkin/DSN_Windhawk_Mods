struct AvailabilitySnapshot {
    std::array<bool, std::size(catalog)> unavailable{};
    std::array<bool, std::size(catalog)> credentialsRequired{};
};

constexpr PCWSTR uacPolicyKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\System";
constexpr PCWSTR settingsPolicyKey = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Policies\\Explorer";

constexpr bool ConfirmedMissingError(LSTATUS status) {
    return status == ERROR_FILE_NOT_FOUND || status == ERROR_PATH_NOT_FOUND;
}

bool MissingFile(std::wstring const& path) {
    if (path.empty()) return false;
    const auto attributes = GetFileAttributesW(path.c_str());
    return attributes == INVALID_FILE_ATTRIBUTES ? ConfirmedMissingError(GetLastError()) :
                                                   (attributes & FILE_ATTRIBUTE_DIRECTORY) != 0;
}

bool MissingControlRegistration(std::wstring_view resource) {
    const auto [kind, guid] = Split(resource, L':');
    if (kind != L"shell") return false;
    const auto key = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ControlPanel\\NameSpace\\" +
                     std::wstring(guid);
    HKEY handle{};
    const auto status = RegOpenKeyExW(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_QUERY_VALUE, &handle);
    if (status == ERROR_SUCCESS) RegCloseKey(handle);
    return ConfirmedMissingError(status);
}

std::optional<DWORD> ReadMachineDword(PCWSTR key, PCWSTR name) {
    DWORD value{};
    DWORD bytes = sizeof(value);
    if (RegGetValueW(HKEY_LOCAL_MACHINE, key, name, RRF_RT_REG_DWORD, nullptr, &value, &bytes) != ERROR_SUCCESS)
        return {};
    return value;
}

enum class SettingsPolicyMode { None, Hide, ShowOnly, Unknown };

struct SettingsPolicy {
    SettingsPolicyMode mode;
    std::vector<std::wstring> pages;
};

SettingsPolicy ParseSettingsPolicy(std::wstring value) {
    for (auto& ch : value) {
        if (ch >= L'A' && ch <= L'Z') ch += L'a' - L'A';
    }
    auto [prefix, list] = Split(value, L':');
    SettingsPolicy policy{SettingsPolicyMode::Unknown, {}};
    if (prefix == L"hide") policy.mode = SettingsPolicyMode::Hide;
    else if (prefix == L"showonly") policy.mode = SettingsPolicyMode::ShowOnly;
    else return policy;

    do {
        const auto [page, rest] = Split(list, L';');
        if (page.empty() || !std::all_of(page.begin(), page.end(), [](wchar_t ch) {
                return (ch >= L'a' && ch <= L'z') || (ch >= L'0' && ch <= L'9') || ch == L'-' || ch == L'_';
            })) return {SettingsPolicyMode::Unknown, {}};
        policy.pages.emplace_back(page);
        if (page.size() == list.size()) return policy;
        list = rest;
    } while (true);
}

SettingsPolicy ReadSettingsPolicy(HKEY hive, PCWSTR key, PCWSTR name) {
    DWORD bytes{};
    auto status = RegGetValueW(hive, key, name, RRF_RT_REG_SZ, nullptr, nullptr, &bytes);
    if (ConfirmedMissingError(status)) return {SettingsPolicyMode::None, {}};
    if (status != ERROR_SUCCESS || bytes < sizeof(wchar_t) || bytes > 32768 || bytes % sizeof(wchar_t))
        return {SettingsPolicyMode::Unknown, {}};

    std::wstring value(bytes / sizeof(wchar_t), L'\0');
    status = RegGetValueW(hive, key, name, RRF_RT_REG_SZ, nullptr, value.data(), &bytes);
    if (status != ERROR_SUCCESS || bytes < sizeof(wchar_t) || bytes % sizeof(wchar_t))
        return {SettingsPolicyMode::Unknown, {}};
    value.resize(bytes / sizeof(wchar_t) - 1);
    if (value.empty()) return {SettingsPolicyMode::None, {}};
    return ParseSettingsPolicy(std::move(value));
}

std::optional<bool> SettingsPolicyHides(SettingsPolicy const& policy, std::wstring_view page,
                                        bool categoryRoot) {
    if (policy.mode == SettingsPolicyMode::None || policy.mode == SettingsPolicyMode::Unknown)
        return {};
    const bool listed = std::find(policy.pages.begin(), policy.pages.end(), page) != policy.pages.end();
    if (policy.mode == SettingsPolicyMode::Hide) return listed;
    // A category can still be present through pages outside the mod's catalog.
    return !categoryRoot && !listed;
}

bool HiddenBySettingsPolicy(std::wstring_view page, bool categoryRoot,
                            std::array<SettingsPolicy, 2> const& policies) {
    bool hidden = false;
    for (auto const& policy : policies) {
        if (policy.mode == SettingsPolicyMode::None) continue;
        // When both scopes are configured, hide only if both confirm it.
        if (!SettingsPolicyHides(policy, page, categoryRoot).value_or(false)) return false;
        hidden = true;
    }
    return hidden;
}

constexpr bool SavedPasskeysAbsent(DWORD build, std::optional<DWORD> revision) {
    // The public Windows 11 rollout began with KB5030310 (22621.2361).
    return build < 22621 || (build == 22621 && revision && *revision < 2361);
}

bool SavedPasskeysAbsent() {
    using RtlGetVersionFn = LONG(WINAPI*)(OSVERSIONINFOW*);
    const auto module = GetModuleHandleW(L"ntdll.dll");
    if (!module) return false;
    const auto getVersion = reinterpret_cast<RtlGetVersionFn>(GetProcAddress(module, "RtlGetVersion"));
    OSVERSIONINFOW version{};
    version.dwOSVersionInfoSize = sizeof(version);
    if (!getVersion || getVersion(&version) != 0 || !version.dwBuildNumber) return false;
    std::optional<DWORD> revision;
    if (version.dwBuildNumber == 22621) {
        revision = ReadMachineDword(L"SOFTWARE\\Microsoft\\Windows NT\\CurrentVersion", L"UBR");
    }
    return SavedPasskeysAbsent(version.dwBuildNumber, revision);
}

std::optional<bool> TokenHasEnabledAdministrators(HANDLE token) {
    BYTE sid[SECURITY_MAX_SID_SIZE]{};
    DWORD sidSize = sizeof(sid);
    if (!CreateWellKnownSid(WinBuiltinAdministratorsSid, nullptr, sid, &sidSize)) return {};

    DWORD bytes{};
    if (GetTokenInformation(token, TokenGroups, nullptr, 0, &bytes) ||
        GetLastError() != ERROR_INSUFFICIENT_BUFFER || bytes < sizeof(TOKEN_GROUPS)) return {};
    std::vector<BYTE> buffer(bytes);
    if (!GetTokenInformation(token, TokenGroups, buffer.data(), bytes, &bytes)) return {};
    const auto groups = reinterpret_cast<TOKEN_GROUPS const*>(buffer.data());
    for (DWORD i = 0; i < groups->GroupCount; ++i) {
        auto const& group = groups->Groups[i];
        if ((group.Attributes & SE_GROUP_ENABLED) && EqualSid(group.Sid, sid)) return true;
    }
    return false;
}

constexpr bool CredentialsNeeded(TOKEN_ELEVATION_TYPE type, std::optional<bool> administrator,
                                 std::optional<DWORD> adminPrompt) {
    if (type == TokenElevationTypeFull || !administrator) return false;
    if (!*administrator) return type == TokenElevationTypeDefault || type == TokenElevationTypeLimited;
    return type == TokenElevationTypeLimited && adminPrompt && (*adminPrompt == 1 || *adminPrompt == 3);
}

constexpr bool LegacyAdminApprovalMode(LSTATUS status, DWORD mode) {
    return ConfirmedMissingError(status) || (status == ERROR_SUCCESS && mode == 1);
}

bool CredentialsNeeded() {
    HANDLE rawToken{};
    if (!OpenProcessToken(GetCurrentProcess(), TOKEN_QUERY, &rawToken)) return false;
    const std::unique_ptr<void, decltype(&CloseHandle)> token(rawToken, CloseHandle);
    TOKEN_ELEVATION_TYPE type{};
    DWORD bytes{};
    if (!GetTokenInformation(token.get(), TokenElevationType, &type, sizeof(type), &bytes)) return false;

    // Administrator protection uses a separate on-demand token and prompt policy.
    // The legacy token and policy checks cannot classify that mode reliably.
    DWORD mode{};
    bytes = sizeof(mode);
    const auto status = RegGetValueW(HKEY_LOCAL_MACHINE, uacPolicyKey, L"TypeOfAdminApprovalMode",
                                     RRF_RT_REG_DWORD, nullptr, &mode, &bytes);
    if (!LegacyAdminApprovalMode(status, mode)) return false;

    if (type == TokenElevationTypeDefault) {
        // With UAC off, administrators can get an unlinked token with full access.
        return CredentialsNeeded(type, TokenHasEnabledAdministrators(token.get()), {});
    }
    if (type != TokenElevationTypeLimited) return false;

    TOKEN_LINKED_TOKEN linked{};
    if (!GetTokenInformation(token.get(), TokenLinkedToken, &linked, sizeof(linked), &bytes)) return false;
    const std::unique_ptr<void, decltype(&CloseHandle)> fullToken(linked.LinkedToken, CloseHandle);
    return CredentialsNeeded(type, TokenHasEnabledAdministrators(fullToken.get()),
                             ReadMachineDword(uacPolicyKey, L"ConsentPromptBehaviorAdmin"));
}

bool RequiresAdministrator(std::wstring_view kind, std::wstring_view target) {
    // These Windows executables use requireAdministrator. Ordinary MMC routes
    // and tools that can also open with standard rights are kept visible.
    if (kind == L"exe") {
        const auto name = Split(target, L';').first;
        return name == L"OptionalFeatures.exe" || name == L"RecoveryDrive.exe" || name == L"MdSched.exe";
    }
    return kind == L"native" && target == L"10961";
}

AvailabilitySnapshot BuildAvailability() {
    AvailabilitySnapshot result;
    const bool home = IsHomeEdition();
    const bool wifi = HasWirelessInterface();
    const bool startsOnThisPc = ExplorerStartsOnThisPc();
    const bool noPasskeys = SavedPasskeysAbsent();
    const bool credentials = CredentialsNeeded();
    const bool missingControlHost = MissingFile(SystemPath(L"control.exe"));
    constexpr PCWSTR policyName = L"SettingsPageVisibility";
    const std::array policies{ReadSettingsPolicy(HKEY_LOCAL_MACHINE, settingsPolicyKey, policyName),
                              ReadSettingsPolicy(HKEY_CURRENT_USER, settingsPolicyKey, policyName)};

    for (size_t i = 0; i < std::size(catalog) && !unloading; ++i) {
        auto const& item = catalog[i];
        const auto [kind, target] = Split(item.action, L':');
        result.credentialsRequired[i] = credentials && RequiresAdministrator(kind, target);
        if (kind == L"exe") {
            const auto [program, args] = Split(target, L';');
            result.unavailable[i] = MissingFile(ProgramPath(program)) ||
                (program.ends_with(L".cpl") && missingControlHost) ||
                (args.ends_with(L".msc") && MissingFile(SystemPath(args))) ||
                (home && (args == L"gpedit.msc" || args == L"secpol.msc" || args == L"lusrmgr.msc"));
        } else if (kind == L"control") {
            result.unavailable[i] = missingControlHost || MissingControlRegistration(item.resource) ||
                                     (home && target == L"Microsoft.BitLockerDriveEncryption");
        } else if (kind == L"folder") {
            result.unavailable[i] = startsOnThisPc && target == L"0AC0837C-BBF8-452A-850D-79D08E667CA7";
        } else if (kind == L"uri" || kind == L"category") {
            const auto page = Split(target, L';').first;
            const bool categoryRoot = kind == L"uri" && item.parent < 0 && page != L"windowsupdate";
            result.unavailable[i] = (page == L"network-wifi" && !wifi) || (page == L"savedpasskeys" && noPasskeys) ||
                                     (!home && HiddenBySettingsPolicy(page, categoryRoot, policies));
        }
    }
    return result;
}

constexpr ULONGLONG availabilityRefreshInterval = 20000;
SRWLOCK availabilityLock = SRWLOCK_INIT;
AvailabilitySnapshot availability;
std::atomic<bool> availabilityRefreshing{};
std::atomic<ULONGLONG> availabilityLastRefresh{};

AvailabilitySnapshot ReadAvailability() {
    AcquireSRWLockShared(&availabilityLock);
    const auto result = availability;
    ReleaseSRWLockShared(&availabilityLock);
    return result;
}

void CALLBACK RefreshAvailabilityWorker(PTP_CALLBACK_INSTANCE callback, PVOID module) noexcept {
    FreeLibraryWhenCallbackReturns(callback, static_cast<HMODULE>(module));
    try {
        if (!unloading) {
            const auto next = BuildAvailability();
            if (!unloading) {
                AcquireSRWLockExclusive(&availabilityLock);
                availability = next;
                ReleaseSRWLockExclusive(&availabilityLock);
            }
        }
    } catch (...) {
        // Keep the most recent complete snapshot when a refresh cannot finish.
    }
    availabilityLastRefresh = GetTickCount64();
    availabilityRefreshing = false;
}

void RefreshAvailability(bool force = false) {
    const auto isFresh = [] {
        const auto refreshedAt = availabilityLastRefresh.load();
        return refreshedAt && GetTickCount64() - refreshedAt < availabilityRefreshInterval;
    };
    if (unloading || (!force && isFresh()) || availabilityRefreshing.exchange(true)) return;
    if (unloading || (!force && isFresh())) {
        availabilityRefreshing = false;
        return;
    }

    HMODULE module{};
    if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                          reinterpret_cast<PCWSTR>(&RefreshAvailabilityWorker), &module)) {
        if (TrySubmitThreadpoolCallback(RefreshAvailabilityWorker, module, nullptr)) return;
        FreeLibrary(module);
    }
    availabilityLastRefresh = GetTickCount64();
    availabilityRefreshing = false;
}

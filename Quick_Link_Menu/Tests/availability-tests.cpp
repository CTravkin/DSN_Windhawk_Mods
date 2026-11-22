#include <windows.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <memory>
#include <optional>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace {

struct CatalogItem {
    PCWSTR key;
    int group;
    int parent;
    bool enabledByDefault;
    wchar_t glyph;
    PCWSTR action;
    PCWSTR resource;
    std::array<PCWSTR, 6> names;
};

constexpr CatalogItem catalog[] = {
    {L"test.missing", 0, -1, true, 0, L"exe:quick-link-menu-test-462294e5.exe", L"", {}},
    {L"test.exists", 0, -1, true, 0, L"exe:cmd.exe", L"", {}},
};
std::atomic<bool> unloading;

std::pair<std::wstring_view, std::wstring_view> Split(std::wstring_view value, wchar_t divider) {
    const auto pos = value.find(divider);
    if (pos == value.npos) return {value, {}};
    return {value.substr(0, pos), value.substr(pos + 1)};
}

std::wstring SystemPath(std::wstring_view name) {
    wchar_t path[MAX_PATH]{};
    const auto length = GetSystemDirectoryW(path, std::size(path));
    assert(length && length < std::size(path));
    return std::wstring(path, length) + L'\\' + std::wstring(name);
}

std::wstring ProgramPath(std::wstring_view name) { return SystemPath(name); }
bool HasWirelessInterface() { return true; }
bool IsHomeEdition() { return false; }
bool ExplorerStartsOnThisPc() { return false; }

#include "../Mod/Source/Availability.h"

void TestCredentials() {
    assert(LegacyAdminApprovalMode(ERROR_FILE_NOT_FOUND, 0));
    assert(LegacyAdminApprovalMode(ERROR_SUCCESS, 1));
    assert(!LegacyAdminApprovalMode(ERROR_SUCCESS, 2));
    assert(!LegacyAdminApprovalMode(ERROR_ACCESS_DENIED, 0));
    assert(CredentialsNeeded(TokenElevationTypeDefault, false, {}));
    assert(!CredentialsNeeded(TokenElevationTypeDefault, true, {}));
    assert(!CredentialsNeeded(TokenElevationTypeDefault, {}, {}));
    assert(!CredentialsNeeded(TokenElevationTypeFull, true, 1));
    assert(CredentialsNeeded(TokenElevationTypeLimited, false, {}));
    assert(!CredentialsNeeded(TokenElevationTypeLimited, true, {}));
    constexpr std::array adminPromptNeedsCredentials{false, true, false, true, false, false};
    for (DWORD prompt = 0; prompt < adminPromptNeedsCredentials.size(); ++prompt) {
        assert(CredentialsNeeded(TokenElevationTypeLimited, true, prompt) == adminPromptNeedsCredentials[prompt]);
    }

    assert(RequiresAdministrator(L"native", L"10961"));
    assert(RequiresAdministrator(L"exe", L"OptionalFeatures.exe"));
    assert(RequiresAdministrator(L"exe", L"RecoveryDrive.exe"));
    assert(RequiresAdministrator(L"exe", L"MdSched.exe"));
    assert(!RequiresAdministrator(L"native", L"10960"));
    assert(!RequiresAdministrator(L"exe", L"mmc.exe;services.msc"));
    assert(!RequiresAdministrator(L"exe", L"regedit.exe"));
}

void TestSettingsPolicy() {
    constexpr SettingsPolicy none{SettingsPolicyMode::None, {}};
    const auto hide = ParseSettingsPolicy(L"HiDe:About;installed-apps");
    const auto showOnly = ParseSettingsPolicy(L"SHOWONLY:Bluetooth;About");
    assert(HiddenBySettingsPolicy(L"about", false, {none, hide}));
    assert(HiddenBySettingsPolicy(L"installed-apps", false, {hide, none}));
    assert(!HiddenBySettingsPolicy(L"game", false, {hide, none}));
    assert(!HiddenBySettingsPolicy(L"about", false, {hide, showOnly}));
    assert(HiddenBySettingsPolicy(L"installed-apps", false, {hide, showOnly}));
    assert(!HiddenBySettingsPolicy(L"system", true, {showOnly, none}));
    assert(HiddenBySettingsPolicy(L"installed-apps", false, {showOnly, none}));
    assert(!HiddenBySettingsPolicy(L"about", false, {hide, ParseSettingsPolicy(L"showonly:")}));
    assert(!HiddenBySettingsPolicy(L"other", false, {none, ParseSettingsPolicy(L"showonly:about;")}));
    assert(!HiddenBySettingsPolicy(L"other", false, {none, ParseSettingsPolicy(L"showonly:ms-settings:about")}));
}

void TestPasskeysThreshold() {
    assert(SavedPasskeysAbsent(22000, {}));
    assert(SavedPasskeysAbsent(22621, 2360));
    assert(!SavedPasskeysAbsent(22621, 2361));
    assert(!SavedPasskeysAbsent(22621, {}));
    assert(!SavedPasskeysAbsent(26100, 0));
}

void TestSnapshot() {
    auto snapshot = ReadAvailability();
    assert(!snapshot.unavailable[0] && !snapshot.unavailable[1]);
    assert(!snapshot.credentialsRequired[0] && !snapshot.credentialsRequired[1]);

    RefreshAvailability();
    const auto deadline = GetTickCount64() + 5000;
    while (GetTickCount64() < deadline && !(snapshot = ReadAvailability()).unavailable[0]) Sleep(10);
    assert(snapshot.unavailable[0] && !snapshot.unavailable[1]);
    assert(!snapshot.credentialsRequired[0] && !snapshot.credentialsRequired[1]);
}

}  // namespace

int main() {
    TestCredentials();
    TestSettingsPolicy();
    TestPasskeysThreshold();
    TestSnapshot();
}

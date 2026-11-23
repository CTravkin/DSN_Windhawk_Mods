#include <winsock2.h>
#include <windows.h>
#include <ipifcons.h>
#include <netioapi.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <cassert>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <string_view>
#include <thread>
#include <utility>
#include <vector>

namespace {

struct CatalogItem {
    PCWSTR key;
    int group;
    int parent;
    wchar_t glyph;
    PCWSTR action;
    PCWSTR resource;
    std::array<PCWSTR, 6> names;
};

constexpr CatalogItem catalog[] = {
    {L"test.missing", 0, -1, 0, L"exe:quick-link-menu-test-462294e5.exe", L"", {}},
    {L"test.exists", 0, -1, 0, L"exe:cmd.exe", L"", {}},
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
HRESULT ReadDeviceKeyboardResourceStatus() { return S_OK; }
bool IsHomeEdition() { return false; }
bool ExplorerStartsOnThisPc() { return false; }

#include "../Mod/Source/ModuleWorkQueue.h"
#include "../Mod/Source/PowerActions.h"

ModuleWorkQueue moduleWorkQueue;
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

void TestDeviceSettings() {
    assert(PreferLegacyDeveloperTitle(22631, {}));
    assert(PreferLegacyDeveloperTitle(26100, 4349));
    assert(!PreferLegacyDeveloperTitle(26100, 6725));
    assert(!PreferLegacyDeveloperTitle(26300, 10075));
    assert(!PreferLegacyDeveloperTitle(0, {}));

    constexpr auto missingResource = HRESULT_FROM_WIN32(ERROR_MRM_NAMED_RESOURCE_NOT_FOUND);
    assert(KeyboardPageKnownMissing(26100, missingResource));
    assert(!KeyboardPageKnownMissing(26100, E_ACCESSDENIED));
    assert(!KeyboardPageKnownMissing(26100, S_OK));
    assert(!KeyboardPageKnownMissing(26300, missingResource));
    assert(MissingDeviceSettingsPage(L"devices-keyboard", {}, true));
    assert(!MissingDeviceSettingsPage(L"devices-keyboard", {}, false));
    assert(!MissingDeviceSettingsPage(L"typing", {}, true));
    assert(MissingDeviceSettingsPage(L"network-ethernet", {}, true));
    assert(MissingDeviceSettingsPage(L"network-wifi", {}, true));
    assert(MissingDeviceSettingsPage(L"network-mobilehotspot", {}, true));
    constexpr NetworkInterfaces wired{true, false, false};
    constexpr NetworkInterfaces wireless{false, true, false};
    constexpr NetworkInterfaces mobile{false, false, true};
    assert(!MissingDeviceSettingsPage(L"network-ethernet", wired, true));
    assert(!MissingDeviceSettingsPage(L"network-wifi", wireless, true));
    assert(!MissingDeviceSettingsPage(L"network-mobilehotspot", wired, true));
    assert(!MissingDeviceSettingsPage(L"network-mobilehotspot", wireless, true));
    assert(!MissingDeviceSettingsPage(L"network-mobilehotspot", mobile, true));
}

void TestSoftwareEthernet() {
    MIB_IF_ROW2 adapter{};
    adapter.Type = IF_TYPE_ETHERNET_CSMACD;
    adapter.OperStatus = IfOperStatusUp;
    adapter.InterfaceLuid.Value = 4;

    MIB_UNICASTIPADDRESS_TABLE addresses{};
    assert(!IsAddressedSoftwareEthernet(adapter, addresses));
    addresses.NumEntries = 1;
    addresses.Table[0].InterfaceLuid.Value = 5;
    assert(!IsAddressedSoftwareEthernet(adapter, addresses));
    addresses.Table[0].InterfaceLuid.Value = 4;
    assert(IsAddressedSoftwareEthernet(adapter, addresses));

    adapter.InterfaceAndOperStatusFlags.FilterInterface = true;
    assert(!IsAddressedSoftwareEthernet(adapter, addresses));
    adapter.InterfaceAndOperStatusFlags.FilterInterface = false;
    adapter.OperStatus = IfOperStatusDown;
    assert(!IsAddressedSoftwareEthernet(adapter, addresses));
}

void TestSnapshot() {
    assert(moduleWorkQueue.Start());
    auto snapshot = ReadAvailability();
    assert(!snapshot.unavailable[0] && !snapshot.unavailable[1]);
    assert(!snapshot.credentialsRequired[0] && !snapshot.credentialsRequired[1]);

    RefreshAvailability();
    const auto deadline = GetTickCount64() + 5000;
    while (GetTickCount64() < deadline && !(snapshot = ReadAvailability()).unavailable[0]) Sleep(10);
    assert(snapshot.unavailable[0] && !snapshot.unavailable[1]);
    assert(!snapshot.credentialsRequired[0] && !snapshot.credentialsRequired[1]);
    moduleWorkQueue.Stop();
}

void TestWorkQueueStopsWithActiveCallback() {
    struct Work {
        HANDLE started = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        HANDLE proceed = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        std::atomic<bool> finished{};
    } work;
    assert(work.started && work.proceed);
    ModuleWorkQueue queue;
    assert(queue.Start());
    const auto run = [](PTP_CALLBACK_INSTANCE, void* data) {
        auto& work = *static_cast<Work*>(data);
        SetEvent(work.started);
        assert(WaitForSingleObject(work.proceed, 5000) == WAIT_OBJECT_0);
        work.finished = true;
    };
    assert(queue.Submit(run, &work));
    assert(WaitForSingleObject(work.started, 5000) == WAIT_OBJECT_0);
    std::atomic<bool> stopped{};
    std::thread stop([&] {
        queue.Stop();
        assert(work.finished);
        stopped = true;
    });
    const auto deadline = GetTickCount64() + 2000;
    while (queue.Submit([](PTP_CALLBACK_INSTANCE, void*) {})) {
        assert(GetTickCount64() < deadline);
        Sleep(1);
    }
    assert(!stopped);
    SetEvent(work.proceed);
    stop.join();
    assert(!queue.Submit(run, &work));
    CloseHandle(work.proceed);
    CloseHandle(work.started);
}

void TestPowerActions() {
    assert(MatchesPowerCaption(L"update and restart", L"update and restart"));
    assert(MatchesPowerCaption(L"update and shut down (estimate: 3 min)", L"update and shut down (estimate: %u min)"));
    assert(MatchesPowerCaption(L"update and restart (estimate: 10 - 20 min)", L"update and restart (estimate: %u - %u min)"));
    assert(!MatchesPowerCaption(L"restart", L"update and restart"));
    assert(!MatchesPowerCaption(L"update and restart (estimate: min)", L"update and restart (estimate: %u min)"));
    assert(!MatchesPowerCaption(L"update and restart (estimate: 7 min) extra", L"update and restart (estimate: %u min)"));
    assert(PlacePowerAction(false, false, true) == PowerPlacement::Submenu);
    assert(PlacePowerAction(false, true, false) == PowerPlacement::Main);
    assert(PlacePowerAction(true, true, true) == PowerPlacement::Submenu);
    assert(PlacePowerAction(true, true, false) == PowerPlacement::Hidden);
}

}  // namespace

int main() {
    TestCredentials();
    TestSettingsPolicy();
    TestPasskeysThreshold();
    TestDeviceSettings();
    TestSoftwareEthernet();
    TestSnapshot();
    TestWorkQueueStopsWithActiveCallback();
    TestPowerActions();
}

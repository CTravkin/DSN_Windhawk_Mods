// ==WindhawkMod==
// @id              quick-link-menu
// @name            Quick Link Menu
// @description     Customize the Win+X menu and add more commands (right-click the Start button)
// @description:ru-RU Кастомизация и расширение возможностей меню Win+X (правый клик по кнопке «Пуск»)
// @description:es-ES Personaliza el menú Win+X y añade más comandos (clic derecho en el botón Inicio)
// @description:de-DE Das Win+X-Menü anpassen und um weitere Befehle ergänzen (Rechtsklick auf die Startschaltfläche)
// @description:pt-BR Personalize o menu Win+X e adicione mais comandos (clique com o botão direito no botão Iniciar)
// @description:zh-CN 自定义 Win+X 菜单并添加更多命令（右键单击“开始”按钮）
// @version         1.2.1
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -DWIN32_LEAN_AND_MEAN -lcomctl32 -lruntimeobject -lole32 -loleaut32 -lshell32 -lshlwapi -ladvapi32 -liphlpapi -luuid -ldwrite
// ==/WindhawkMod==
// Build: 49

// ==WindhawkModReadme==
/*
# Quick Link Menu

**English** · [Русский](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/Quick_Link_Menu/README.ru.md)

Quick Link Menu customizes the native Windows 11 menu opened by **Win+X** or by
right-clicking the Start button.

## Features

![Windows 11 Win+X and three Quick Link Menu configurations showing the System, Shut down, and Event Viewer submenus](https://raw.githubusercontent.com/CTravkin/DSN_Windhawk_Mods/3b4e177fc36ebff3d3904821c6b85d9e9032583d/Quick_Link_Menu/Assets/Previews/Comparison.png)

### Customization and command availability

- **Menu contents.** Hide individual rows, submenu commands, or entire sections.
  Turning off a section preserves its settings. Turning off a row hides its submenu.
- **Automatic filtering.** The mod hides unavailable commands based on the Windows
  edition, installed components, hardware, and system policies. If a main action is
  unavailable, its available shortcuts remain. An empty menu falls back to standard Win+X.
- **User permissions.** An optional filter hides administrator commands that require
  credentials or whose elevation is blocked. Commands that only need UAC consent stay visible.
- **Submenu controls.** Open submenus on hover, after a delay, on arrow hover, or by
  clicking the arrow. A hand cursor can mark areas that run a command.
- **Quick launch keys.** Enable Windows letter shortcuts for supported Win+X
  commands. In **All commands** mode, a letter can open a closed submenu and
  focus the matching item.
- **Terminal.** The main Terminal action can run as administrator.

### Menu sections

- **Settings.** Open Windows Settings categories and pages such as Display,
  Sound, VPN, Default apps, and Windows Update.
- **Control Panel.** Open classic tools such as Device Manager, Network
  Connections, Power Options, and Programs and Features.
- **Administration.** Open Computer Management, Services, Event Viewer,
  Registry Editor, Terminal, and related tools such as Task Scheduler and
  Command Prompt.
- **Basic commands.** Open Run, Task Manager, Search, and File Explorer, show
  the desktop, or access power and sign-in options. File Explorer opens its
  configured start page, with a submenu for locations such as This PC,
  Downloads, and the Recycle Bin.

### Quick access to power actions

The main power row shuts down Windows in one click by default. It can run another
action, such as Restart or Lock. Choose **None** to open the power submenu without
running a command. When Windows offers **Update and shut down** or **Update and
restart** in Win+X, the corresponding Shut down or Restart switch also controls
that entry.

### Labels and icons

The menu uses Windows labels in the system display language and Segoe Fluent Icons.
Settings shortcuts use the matching Windows icons where available.

Windhawk settings are available in English, Russian, Spanish, German, Brazilian
Portuguese, and Simplified Chinese.

## Platforms

Requires Windows 11 x64 or ARM64. GitHub Actions verified native ARM64
compilation and automated command availability tests. Win+X interaction was
tested on x64.

### Compatibility

Quick Link Menu works with the standard Windows 11 Win+X menu. Mods that replace
it are incompatible. One example is [Windows 10 Win+X restorer on Win11 24H2](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/windows-10-winx-restorer.wh.cpp).

## Usage

Click a row's label or press **Enter** to run its main action. Click the arrow
or press **→** to open the submenu. Click the label if a row only has a submenu.

**Quick launch keys** is off by default under **General settings**. **Visible only**
works in the main menu and any open submenu. **All commands** also finds commands
in closed submenus and opens them for selection. When several eligible items
share a letter, press it again to cycle and press **Enter** to run the focused
command. A letter assigned to a submenu opener opens that submenu.

To change the menu, open **Quick Link Menu → Settings** in Windhawk. Section
switches are below their descriptions. Row and submenu switches follow the menu
order. Find submenu behavior and the administrator filter under **General settings**,
and Terminal and power actions under **Command settings**.

## Links

- [Quick Link Menu on GitHub](https://github.com/CTravkin/DSN_Windhawk_Mods/tree/main/Quick_Link_Menu)
- [DSN Windhawk Mods](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/README.md)
*/
// ==/WindhawkModReadme==

// GENERATED_SETTINGS

#include <winsock2.h>
#include <dwrite.h>
#include <shellapi.h>
#include <ipifcons.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <netioapi.h>
#include <uiautomationclient.h>
#include <windhawk_utils.h>
#undef GetCurrentTime
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Input.h>
#include <winrt/Windows.UI.Xaml.Automation.h>
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.Provider.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
#include <winrt/Windows.UI.Xaml.Documents.h>
#include <winrt/Windows.UI.Xaml.Input.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <chrono>
#include <cmath>
#include <memory>
#include <mutex>
#include <optional>
#include <span>
#include <string>
#include <string_view>
#include <utility>
#include <vector>

namespace X = winrt::Windows::UI::Xaml;
namespace C = X::Controls;
namespace P = C::Primitives;
namespace I = X::Input;
namespace AP = X::Automation::Peers;
namespace AV = X::Automation::Provider;
using Key = winrt::Windows::System::VirtualKey;
using PointerType = winrt::Windows::Devices::Input::PointerDeviceType;

namespace {

struct CatalogItem {
    PCWSTR key;
    int group;
    int parent;
    wchar_t glyph;
    PCWSTR action;
    PCWSTR resource;
    std::array<PCWSTR, 6> names;
    std::array<unsigned, 2> updateCaptions{};
};

// GENERATED_CATALOG

// GENERATED_POWER_ACTIONS
// GENERATED_WORK_QUEUE
// GENERATED_QUICK_KEYS

enum class HoverMode { Immediate, Delayed, Arrow, ArrowClick };
enum class HoverZone { Outside, Body, Arrow };

struct Settings {
    HoverMode mode{};
    QuickKeyMode quickKeys{};
    bool hideAdminCommands{};
    bool showHandCursor{};
    bool terminalAsAdmin{};
    std::wstring primaryPowerAction;
    std::array<bool, std::size(sectionEnabledKeys)> enabledSections{};
    std::array<bool, std::size(catalog)> enabled{};
};

struct MenuSession;

struct Destination {
    CatalogItem const* item{};
    C::MenuFlyoutItem original{nullptr};
    unsigned powerKeyCaption{};
};

struct MenuKeyRow {
    C::MenuFlyoutItemBase element{nullptr};
    C::MenuFlyoutSubItem parent{nullptr};
    Destination destination;
    wchar_t key{};
    std::optional<QuickKeyMark> mark;
    X::FrameworkElement::Loaded_revoker loaded;
};

struct MenuKeyPresenter {
    C::MenuFlyoutPresenter element{nullptr};
    X::UIElement::PreviewKeyDown_revoker keyDown;
};

struct RowInteraction {
    MenuSession* owner{};
    Destination destination;
    C::MenuFlyoutSubItem split{nullptr};
    HoverMode mode{};
    C::Grid layout{nullptr};
    X::FrameworkElement chevron{nullptr};
    C::Canvas arrowLayer{nullptr};
    C::Border arrowBackground{nullptr};
    X::VisualStateGroup commonStates{nullptr};
    AV::IExpandCollapseProvider expand{nullptr};
    X::DispatcherTimer hoverUpdate{nullptr};
    HoverZone zone{HoverZone::Outside};
    bool openOnUpdate{};
    std::optional<uint32_t> pressedPointer;
    HoverZone pressedZone{HoverZone::Outside};
    X::UIElement::PointerPressed_revoker pointerPressed;
    X::UIElement::PointerReleased_revoker pointerReleased;
    X::UIElement::PointerMoved_revoker pointerMoved;
    X::UIElement::PointerEntered_revoker pointerEntered;
    X::UIElement::PointerExited_revoker pointerExited;
    X::UIElement::PointerCanceled_revoker pointerCanceled;
    X::VisualStateGroup::CurrentStateChanged_revoker visualStateChanged;
    X::DispatcherTimer::Tick_revoker tick;
};

struct CursorEvents {
    X::UIElement::PointerEntered_revoker entered;
    X::UIElement::PointerMoved_revoker moved;
    X::UIElement::PointerExited_revoker exited;
};

struct SubmenuCleanup {
    std::vector<X::XamlRoot> roots;
    std::vector<C::MenuFlyoutItemBase> items;
    ULONGLONG retryUntil{};
};

struct MenuSession {
    C::MenuFlyout menu{nullptr};
    std::vector<C::MenuFlyoutItemBase> original;
    bool menuChanged{};
    bool closing{};
    HCURSOR commandCursor{};
    HCURSOR previousCursor{};
    X::UIElement cursorElement{nullptr};
    std::vector<std::unique_ptr<RowInteraction>> rows;
    SubmenuCleanup submenus;
    std::vector<C::MenuFlyoutItem::Click_revoker> clicks;
    std::vector<CursorEvents> cursorEvents;
    QuickKeyMode quickKeys{};
    QuickKeyCycle keyCycle;
    std::vector<MenuKeyRow> keyRows;
    std::vector<MenuKeyPresenter> keyPresenters;
    C::MenuFlyoutItemBase focusWhenLoaded{nullptr};
    X::DispatcherTimer keyLaunch{nullptr};
    X::DispatcherTimer::Tick_revoker keyLaunchTick;
    P::FlyoutBase::Opened_revoker opened;
    P::FlyoutBase::Closing_revoker closingEvent;
    P::FlyoutBase::Closed_revoker closed;
};

[[clang::no_destroy]] std::unique_ptr<MenuSession> session;
[[clang::no_destroy]] std::vector<SubmenuCleanup> pendingSubmenuCleanups;
std::mutex captionLock;
std::array<std::wstring, std::size(catalog)> systemCaptionCache;
std::mutex settingsLock;
Settings settings;
std::atomic<bool> unloading;
ModuleWorkQueue moduleWorkQueue;
std::atomic<unsigned> settingsNavigationGeneration;
HWND trayWindow;
HWND subclassRetryWindow;
std::atomic<DWORD> trayThread;
UINT setupMessage;
constexpr WPARAM sweepClosedSubmenus = 2;
constexpr WPARAM resetNativeAccessKeys = 3;
bool subclassPresent;
constexpr DWORD taskbarRetryInterval = 250;
constexpr DWORD taskbarSlowRetryInterval = 2'000;
constexpr ULONGLONG taskbarFastRetryDuration = 60'000;
PTP_TIMER taskbarStartupTimer;
ULONGLONG taskbarFastRetryDeadline;
SRWLOCK taskbarStartupLock = SRWLOCK_INIT;

using ShowAt = HRESULT(WINAPI*)(void*, void*);
using ShowAtOptions = HRESULT(WINAPI*)(void*, void*, void*);
ShowAt originalShowAt;
ShowAtOptions originalShowAtOptions;

template<typename Function>
void OnRow(RowInteraction* row, Function&& function) noexcept {
    if (unloading || !session || session.get() != row->owner) return;
    try {
        function(*row);
    } catch (...) {
        Wh_Log(L"Menu input handler failed");
    }
}

void LoadSettings() {
    Settings next;
    const auto value = WindhawkUtils::StringSetting::make(L"general.submenuHover");
    const std::wstring_view option = value.get();
    next.mode = option == L"delayed" ? HoverMode::Delayed :
                option == L"arrow" ? HoverMode::Arrow :
                option == L"click" ? HoverMode::ArrowClick : HoverMode::Immediate;
    const auto quickKeys = WindhawkUtils::StringSetting::make(L"general.quickLaunchKeys");
    const std::wstring_view keyMode = quickKeys.get();
    next.quickKeys = keyMode == L"visible" ? QuickKeyMode::Visible :
                     keyMode == L"all" ? QuickKeyMode::All : QuickKeyMode::Off;
    next.hideAdminCommands = Wh_GetIntSetting(L"general.hideAdminCommands") != 0;
    next.showHandCursor = Wh_GetIntSetting(L"general.showHandCursor") != 0;
    next.terminalAsAdmin = Wh_GetIntSetting(L"commandOptions.terminalAsAdmin") != 0;
    const auto power = WindhawkUtils::StringSetting::make(L"commandOptions.primaryPowerAction");
    const std::wstring_view action = power.get();
    if (action == L"lock") next.primaryPowerAction = L"lock:session";
    else if (!action.empty() && action != L"none") next.primaryPowerAction = L"power:" + std::wstring(action);
    for (size_t group = 0; group < std::size(sectionEnabledKeys); ++group)
        next.enabledSections[group] = Wh_GetIntSetting(sectionEnabledKeys[group]) != 0;
    for (size_t i = 0; i < std::size(catalog); ++i)
        next.enabled[i] = Wh_GetIntSetting(catalog[i].key) != 0;
    std::lock_guard guard(settingsLock);
    settings = std::move(next);
}

Settings ReadSettings() {
    std::lock_guard guard(settingsLock);
    return settings;
}

std::wstring MenuCaption(std::wstring_view text) {
    // Chinese Win+X resources put the access key at the end, for example (&R).
    auto suffix = text.rfind(L'(');
    if (suffix != text.npos && text.ends_with(L')')) {
        auto key = text.substr(suffix + 1, text.size() - suffix - 2);
        if (key.starts_with(L'&')) key.remove_prefix(1);
        if (key.size() == 1 && key.front() >= L'A' && key.front() <= L'Z') {
            while (suffix && text[suffix - 1] == L' ') --suffix;
            text = text.substr(0, suffix);
        }
    }
    std::wstring caption(text);
    std::erase(caption, L'&');
    return caption;
}

std::wstring Normalize(std::wstring_view text) {
    auto normalized = MenuCaption(text);
    std::erase_if(normalized, [](wchar_t ch) { return ch == L'\u200e' || ch == L'\u200f'; });
    CharLowerBuffW(normalized.data(), static_cast<DWORD>(normalized.size()));
    return normalized;
}

std::wstring SystemPath(std::wstring_view name, bool windowsRoot = false) {
    wchar_t directory[MAX_PATH]{};
    const auto size = windowsRoot ? GetWindowsDirectoryW(directory, ARRAYSIZE(directory)) :
                                    GetSystemDirectoryW(directory, ARRAYSIZE(directory));
    if (!size || size >= ARRAYSIZE(directory)) return {};
    auto path = std::wstring(directory) + L'\\' + std::wstring(name);
    std::replace(path.begin(), path.end(), L'/', L'\\');
    return path;
}

std::wstring Indirect(PCWSTR token, HRESULT* resultCode = nullptr) {
    wchar_t result[512]{};
    const auto status = SHLoadIndirectString(token, result, ARRAYSIZE(result), nullptr);
    if (resultCode) *resultCode = status;
    if (FAILED(status)) return {};
    return result;
}

std::wstring RawSystemString(std::wstring_view reference) {
    const auto token = L"@%SystemRoot%\\System32\\" + std::wstring(reference);
    return Indirect(token.c_str());
}

std::wstring SystemString(std::wstring_view reference) {
    auto result = RawSystemString(reference);
    if (reference.starts_with(L"twinui.pcshell.dll,")) return MenuCaption(result);
    return result;
}

std::wstring Twinui(std::wstring_view id) {
    return SystemString(L"twinui.pcshell.dll,-" + std::wstring(id));
}

size_t DisplayLanguage() {
    wchar_t locale[LOCALE_NAME_MAX_LENGTH]{};
    if (!LCIDToLocaleName(MAKELCID(GetUserDefaultUILanguage(), SORT_DEFAULT), locale, ARRAYSIZE(locale), 0))
        return 0;
    const std::wstring_view code(locale);
    constexpr std::array<std::wstring_view, 6> languages = {L"en", L"ru", L"es", L"de", L"pt", L"zh"};
    for (size_t i = 0; i < languages.size(); ++i)
        if (code.starts_with(languages[i])) return i;
    return 0;
}

struct CoTaskMemory {
    void operator()(void* pointer) const noexcept { CoTaskMemFree(pointer); }
};

using Pidl = std::unique_ptr<ITEMIDLIST, CoTaskMemory>;

Pidl FolderId(std::wstring_view guid) {
    GUID id{};
    const auto literal = L"{" + std::wstring(guid) + L"}";
    if (FAILED(CLSIDFromString(literal.c_str(), &id))) return {};
    PIDLIST_ABSOLUTE result{};
    if (FAILED(SHGetKnownFolderIDList(id, KF_FLAG_DONT_VERIFY, nullptr, &result))) return {};
    return Pidl(result);
}

Pidl ShellId(std::wstring_view name) {
    PIDLIST_ABSOLUTE id{};
    if (FAILED(SHParseDisplayName(std::wstring(name).c_str(), nullptr, &id, 0, nullptr))) return {};
    return Pidl(id);
}

std::wstring ShellName(Pidl const& pidl) {
    PWSTR result{};
    if (!pidl || FAILED(SHGetNameFromIDList(pidl.get(), SIGDN_NORMALDISPLAY, &result))) return {};
    std::unique_ptr<wchar_t, CoTaskMemory> text(result);
    return text.get();
}

std::wstring ControlPath(std::wstring_view category) {
    return L"::{26EE0668-A00A-44D7-9371-BEB064C98683}\\" + std::wstring(category);
}

std::wstring ProgramPath(std::wstring_view program) {
    return SystemPath(program, program == L"regedit.exe");
}

std::pair<std::wstring_view, std::wstring_view> Split(std::wstring_view value, wchar_t divider) {
    const size_t pos = value.find(divider);
    return {value.substr(0, pos), pos == value.npos ? std::wstring_view{} : value.substr(pos + 1)};
}

std::wstring RegistryShellName(std::wstring_view guid) {
    const auto key = L"CLSID\\" + std::wstring(guid);
    wchar_t value[512]{};
    DWORD bytes = sizeof(value);
    if (RegGetValueW(HKEY_CLASSES_ROOT, key.c_str(), L"LocalizedString", RRF_RT_REG_SZ | RRF_RT_REG_EXPAND_SZ,
                     nullptr, value, &bytes) == ERROR_SUCCESS) {
        if (auto name = Indirect(value); !name.empty()) return name;
    }
    return ShellName(ShellId(ControlPath(L"0") + L"\\::" + std::wstring(guid)));
}

std::wstring ToolsName(std::wstring_view shortcut) {
    PWSTR result{};
    if (FAILED(SHGetKnownFolderPath(FOLDERID_CommonAdminTools, KF_FLAG_DONT_VERIFY, nullptr, &result))) return {};
    std::unique_ptr<wchar_t, CoTaskMemory> root(result);
    const auto directory = std::wstring(root.get()) + L"\\";
    const auto fileName = std::wstring(shortcut) + L".lnk";
    const auto ini = directory + L"desktop.ini";
    wchar_t resource[512]{};
    if (GetPrivateProfileStringW(L"LocalizedFileNames", fileName.c_str(), L"", resource,
                                 ARRAYSIZE(resource), ini.c_str())) {
        if (auto title = Indirect(resource); !title.empty()) return title;
    }
    return ShellName(ShellId(directory + fileName));
}

constexpr bool PreferLegacyDeveloperTitle(DWORD build, std::optional<DWORD> revision);
DWORD ReadWindowsBuild();
std::optional<DWORD> ReadWindowsRevision();

std::wstring SettingsResource(std::wstring_view page, std::wstring_view field, HRESULT* resultCode = nullptr) {
    const auto token = L"@{windows?ms-resource://Windows.UI.SettingsAppThreshold/SystemSettings/Resources/" +
                       std::wstring(page) + L"/" + std::wstring(field) + L"}";
    return Indirect(token.c_str(), resultCode);
}

HRESULT ReadDeviceKeyboardResourceStatus() {
    HRESULT status;
    SettingsResource(L"SettingsPageDevicesKeyboard", L"DisplayName", &status);
    return status;
}

std::wstring SettingsCaption(std::wstring_view id) {
    // Windows uses the short title on systems without a battery.
    if (id == L"SettingsPagePowerAndBattery") {
        SYSTEM_POWER_STATUS status{};
        if (GetSystemPowerStatus(&status) && status.BatteryFlag == 128) {
            if (auto title = SettingsResource(id, L"AlternateDisplayName"); !title.empty()) return title;
        }
    }
    if (id == L"SettingsPageAdvancedSystemSettings") {
        constexpr std::wstring_view legacyId = L"SettingsPageRestoreDeveloperOptions";
        const auto build = ReadWindowsBuild();
        const auto revision = build == 26100 ? ReadWindowsRevision() : std::nullopt;
        if (PreferLegacyDeveloperTitle(build, revision)) {
            if (auto title = SettingsResource(legacyId, L"DisplayName"); !title.empty()) return title;
        }
        if (auto title = SettingsResource(id, L"DisplayName"); !title.empty()) return title;
        return SettingsResource(legacyId, L"DisplayName");
    }
    return SettingsResource(id, L"DisplayName");
}

std::wstring SystemCaption(CatalogItem const& item) {
    const auto [kind, target] = Split(item.action, L':');
    const auto [resourceKind, resource] = Split(item.resource, L':');
    if (resourceKind == L"shell") return RegistryShellName(resource);
    if (resourceKind == L"app") return ShellName(ShellId(L"shell:AppsFolder\\" + std::wstring(resource)));
    if (resourceKind == L"tools") {
        if (resource == L"Windows Tools") return RegistryShellName(L"{D20EA4E1-3957-11D2-A40B-0C5020524153}");
        return ToolsName(resource);
    }
    if (resourceKind == L"settings") return SettingsCaption(resource);
    if (resourceKind == L"pri") {
        return Indirect((L"@{windows?ms-resource://" + std::wstring(resource) + L"}").c_str());
    }
    if (*item.resource) return SystemString(item.resource);
    if (kind == L"folder") return ShellName(FolderId(target));
    if (kind == L"native") return Twinui(target);
    return {};
}

std::wstring Caption(Destination const& destination) {
    if (destination.original) return MenuCaption(destination.original.Text());
    const auto& item = *destination.item;
    {
        const std::lock_guard guard(captionLock);
        const auto& cached = systemCaptionCache[&item - catalog];
        if (!cached.empty()) return cached;
    }
    return item.names[DisplayLanguage()];
}

void CALLBACK WarmSystemCaptions(PTP_CALLBACK_INSTANCE, PVOID) noexcept {
    if (unloading) return;
    const auto initialized = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(initialized)) return;
    for (size_t index = 0; index < std::size(catalog) && !unloading; ++index) {
        try {
            auto caption = SystemCaption(catalog[index]);
            if (!caption.empty() && !unloading) {
                const std::lock_guard guard(captionLock);
                systemCaptionCache[index] = std::move(caption);
            }
        } catch (...) {
            // The catalog caption remains available.
        }
    }
    CoUninitialize();
}

template<typename Provider>
Provider GetProvider(X::UIElement const& item, AP::PatternInterface pattern) {
    auto peer = AP::FrameworkElementAutomationPeer::CreatePeerForElement(item);
    return peer ? peer.GetPattern(pattern).try_as<Provider>() : nullptr;
}

DWORD LaunchWithShell(SHELLEXECUTEINFOW& info) {
    info.cbSize = sizeof(info);
    info.nShow = SW_SHOWNORMAL;
    if (ShellExecuteExW(&info)) return ERROR_SUCCESS;
    const auto error = GetLastError();
    return error ? error : ERROR_GEN_FAILURE;
}

DWORD Launch(PCWSTR target, PCWSTR arguments = nullptr) {
    SHELLEXECUTEINFOW info{};
    info.lpVerb = L"open";
    info.lpFile = target;
    info.lpParameters = arguments;
    return LaunchWithShell(info);
}

DWORD LaunchPidl(Pidl const& target) {
    if (!target) return ERROR_FILE_NOT_FOUND;
    SHELLEXECUTEINFOW info{};
    info.fMask = SEE_MASK_IDLIST;
    info.lpIDList = target.get();
    return LaunchWithShell(info);
}

struct SettingsRoute {
    unsigned generation;
    std::wstring pageElementId;
    std::wstring appName;
};

winrt::com_ptr<IUIAutomationCondition> TextCondition(IUIAutomation& automation, PROPERTYID property, PCWSTR text) {
    VARIANT value{};
    value.vt = VT_BSTR;
    value.bstrVal = SysAllocString(text);
    winrt::com_ptr<IUIAutomationCondition> condition;
    if (value.bstrVal) automation.CreatePropertyCondition(property, value, condition.put());
    VariantClear(&value);
    return condition;
}

winrt::com_ptr<IUIAutomationInvokePattern> ParentBreadcrumb(IUIAutomationElement& bar,
                                                           IUIAutomationCondition& crumbType,
                                                           IUIAutomationCacheRequest& cache) {
    winrt::com_ptr<IUIAutomationElementArray> crumbs;
    if (FAILED(bar.FindAllBuildCache(TreeScope_Descendants, &crumbType, &cache, crumbs.put()))) {
        crumbs = nullptr;
        if (FAILED(bar.FindAll(TreeScope_Descendants, &crumbType, crumbs.put()))) return {};
    }
    int count{};
    winrt::com_ptr<IUIAutomationElement> parent;
    if (!crumbs || FAILED(crumbs->get_Length(&count)) || count != 2 ||
        FAILED(crumbs->GetElement(0, parent.put())) || !parent) return {};

    winrt::com_ptr<IUIAutomationInvokePattern> invoke;
    if (FAILED(parent->GetCachedPatternAs(UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern),
                                          invoke.put_void())) || !invoke) {
        invoke = nullptr;
        if (FAILED(parent->GetCurrentPatternAs(UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern),
                                               invoke.put_void()))) return {};
    }
    return invoke;
}

void CALLBACK SelectSettingsCategory(PTP_CALLBACK_INSTANCE, PVOID data) noexcept {
    std::unique_ptr<SettingsRoute> route(static_cast<SettingsRoute*>(data));
    if (unloading || settingsNavigationGeneration != route->generation) return;
    const HRESULT initialized = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(initialized)) return;
    struct Apartment {
        ~Apartment() { CoUninitialize(); }
    } apartment;
    try {
        winrt::com_ptr<IUIAutomation2> automation;
        winrt::check_hresult(CoCreateInstance(__uuidof(CUIAutomation8), nullptr, CLSCTX_INPROC_SERVER,
                                             __uuidof(IUIAutomation2), automation.put_void()));
        winrt::check_hresult(automation->put_ConnectionTimeout(400));
        winrt::check_hresult(automation->put_TransactionTimeout(400));
        const auto pageId = TextCondition(*automation, UIA_AutomationIdPropertyId, route->pageElementId.c_str());
        const auto barId = TextCondition(*automation, UIA_AutomationIdPropertyId, L"PermanentNavigationViewBreadcrumbBar");
        const auto crumbType = TextCondition(*automation, UIA_ClassNamePropertyId,
                                            L"Microsoft.UI.Xaml.Controls.BreadcrumbBarItem");
        if (!pageId || !barId || !crumbType) return;
        winrt::com_ptr<IUIAutomationCacheRequest> crumbCache;
        winrt::check_hresult(automation->CreateCacheRequest(crumbCache.put()));
        winrt::check_hresult(crumbCache->AddPattern(UIA_InvokePatternId));

        // Wait for the URI's landing page, then invoke its parent breadcrumb.
        // Automation identifiers are independent of the Windows display language.
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::seconds(4);
        DWORD retryDelay = 75;
        while (!unloading && settingsNavigationGeneration == route->generation && std::chrono::steady_clock::now() < deadline) {
            const HWND hwnd = FindWindowW(L"ApplicationFrameWindow", route->appName.c_str());
            winrt::com_ptr<IUIAutomationElement> window, page, bar;
            if (hwnd && SUCCEEDED(automation->ElementFromHandle(hwnd, window.put())) && window &&
                SUCCEEDED(window->FindFirst(TreeScope_Descendants, pageId.get(), page.put())) && page &&
                SUCCEEDED(window->FindFirst(TreeScope_Descendants, barId.get(), bar.put())) && bar) {
                if (const auto invoke = ParentBreadcrumb(*bar, *crumbType, *crumbCache);
                    invoke && settingsNavigationGeneration == route->generation && !unloading &&
                    std::chrono::steady_clock::now() < deadline && SUCCEEDED(invoke->Invoke())) return;
            }
            if (unloading || settingsNavigationGeneration != route->generation) return;
            Sleep(retryDelay);
            retryDelay = std::min<DWORD>(retryDelay * 2, 200);
        }
    } catch (...) {
    }
}

void RunDestination(MenuSession& active, Destination destination) {
    const auto navigationGeneration = ++settingsNavigationGeneration;
    if (destination.original) {
        if (auto invoke = GetProvider<AV::IInvokeProvider>(destination.original, AP::PatternInterface::Invoke)) {
            auto menu = active.menu;
            invoke.Invoke();
            menu.Hide();
        } else Wh_Log(L"Could not invoke the original Win+X command %s", destination.item->key);
        return;
    }

    const auto& item = *destination.item;
    const auto [kind, target] = Split(item.action, L':');
    auto menu = active.menu;
    menu.Hide();
    DWORD error = ERROR_NOT_SUPPORTED;
    if (kind == L"uri" || kind == L"category") {
        const auto [page, pageElementId] = Split(target, L';');
        const auto address = L"ms-settings:" + std::wstring(page);
        error = Launch(address.c_str());
        if (error == ERROR_SUCCESS && kind == L"category") {
            auto route = std::make_unique<SettingsRoute>();
            route->generation = navigationGeneration;
            route->pageElementId = pageElementId;
            route->appName = Twinui(L"10922");
            if (route->appName.empty()) route->appName = L"Settings";
            auto* context = route.release();
            if (!moduleWorkQueue.Submit(SelectSettingsCategory, context)) {
                delete context;
                Wh_Log(L"Could not schedule Settings category navigation for %s", item.key);
            }
        }
    } else if (kind == L"folder") {
        error = LaunchPidl(FolderId(target));
    } else if (kind == L"panel") {
        error = LaunchPidl(ShellId(ControlPath(target)));
    } else if (kind == L"lock") {
        error = LockWorkStation() ? ERROR_SUCCESS : ERROR_GEN_FAILURE;
    } else if (kind == L"control") {
        const auto command = target.starts_with(L"Microsoft.") ? L"/name " + std::wstring(target) :
                                                                  std::wstring(target);
        error = Launch(SystemPath(L"control.exe").c_str(), command.c_str());
    } else if (kind == L"exe") {
        const auto [program, argument] = Split(target, L';');
        const auto path = ProgramPath(program);
        auto parameters = std::wstring(argument);
        if (argument.ends_with(L".msc")) parameters = L"\"" + SystemPath(argument) + L"\"";
        if (program.ends_with(L".cpl")) {
            parameters = L"\"" + path + L"\"";
            error = Launch(SystemPath(L"control.exe").c_str(), parameters.c_str());
        } else {
            error = Launch(path.c_str(), parameters.empty() ? nullptr : parameters.c_str());
        }
    }
    if (error != ERROR_SUCCESS && error != ERROR_CANCELLED)
        Wh_Log(L"Could not open Win+X command %s (Win32 error %lu)", item.key, error);
}

X::FrameworkElement FindNamed(X::DependencyObject const& node, std::wstring_view name) {
    auto element = node.try_as<X::FrameworkElement>();
    if (element && element.Name() == name) return element;
    for (int index = 0; index < X::Media::VisualTreeHelper::GetChildrenCount(node); ++index) {
        if (auto child = FindNamed(X::Media::VisualTreeHelper::GetChild(node, index), name)) return child;
    }
    return nullptr;
}

constexpr double arrowHitWidth = 32;

winrt::Windows::Foundation::Point ChevronCenter(RowInteraction const& active, X::UIElement const& relativeTo) {
    return active.chevron.TransformToVisual(relativeTo).TransformPoint({
        static_cast<float>(active.chevron.ActualWidth() / 2.0),
        static_cast<float>(active.chevron.ActualHeight() / 2.0),
    });
}

HoverZone PointerZone(RowInteraction const& active, I::PointerRoutedEventArgs const& args) {
    const auto x = args.GetCurrentPoint(active.layout).Position().X;
    return std::abs(x - ChevronCenter(active, active.layout).X) <= arrowHitWidth / 2.0 ?
        HoverZone::Arrow : HoverZone::Body;
}

void ClearCommandCursor(MenuSession& active) {
    if (!active.cursorElement) return;
    active.cursorElement = nullptr;
    if (GetCursor() == active.commandCursor) SetCursor(active.previousCursor);
}

void UpdateCommandCursor(MenuSession& active, X::UIElement const& element, bool command) {
    if (!active.commandCursor) return;
    if (!command) {
        if (active.cursorElement == element) ClearCommandCursor(active);
        return;
    }
    if (!active.cursorElement) active.previousCursor = GetCursor();
    active.cursorElement = element;
    if (GetCursor() != active.commandCursor) SetCursor(active.commandCursor);
}

void UpdateHoverVisual(RowInteraction const& active) {
    if (!active.arrowBackground) return;
    if (active.zone != HoverZone::Arrow) {
        active.arrowBackground.Opacity(0);
        if (active.zone == HoverZone::Body)
            X::VisualStateManager::GoToState(active.split, L"PointerOver", false);
        return;
    }

    if (!active.arrowBackground.Background()) {
        X::VisualStateManager::GoToState(active.split, L"PointerOver", false);
        active.arrowBackground.Background(active.layout.Background());
    }
    const auto row = active.layout.TransformToVisual(active.arrowLayer).TransformBounds({
        0, 0, static_cast<float>(active.layout.ActualWidth()), static_cast<float>(active.layout.ActualHeight()),
    });
    const double center = ChevronCenter(active, active.arrowLayer).X;
    const double width = (std::min)({static_cast<double>(row.Height),
                                    2 * (center - row.X), 2 * (row.X + row.Width - center)});
    if (width <= 0 || !active.arrowBackground.Background()) return;

    active.arrowBackground.Width(width);
    active.arrowBackground.Height(row.Height);
    C::Canvas::SetLeft(active.arrowBackground, center - width / 2);
    C::Canvas::SetTop(active.arrowBackground, row.Y);
    // Only CommonStates changes here. The native focus visuals stay active.
    X::VisualStateManager::GoToState(active.split, L"Normal", false);
    active.arrowBackground.Opacity(1);
}

void SetHoverZone(RowInteraction& active, HoverZone zone) {
    if (active.owner->closing) return;
    if (std::exchange(active.zone, zone) == zone) return;
    active.openOnUpdate = active.expand &&
        ((active.mode == HoverMode::Immediate && zone != HoverZone::Outside) ||
         (active.mode == HoverMode::Arrow && zone == HoverZone::Arrow));
    UpdateHoverVisual(active);
    if (active.openOnUpdate) active.hoverUpdate.Start();
}

void UpdatePointerZone(RowInteraction& active, I::PointerRoutedEventArgs const& args) {
    const auto zone = PointerZone(active, args);
    if (args.Pointer().PointerDeviceType() == PointerType::Mouse)
        UpdateCommandCursor(*active.owner, active.split, active.destination.item && zone == HoverZone::Body);
    SetHoverZone(active, zone);
}

void AddArrowBackground(RowInteraction& active) {
    auto states = X::VisualStateManager::GetVisualStateGroups(active.layout);
    for (const auto& group : states) {
        if (group.Name() == L"CommonStates") {
            active.commonStates = group;
            break;
        }
    }
    if (!active.commonStates) return;

    // An absolutely positioned child of Canvas cannot grow the menu's grid.
    C::Canvas overlay;
    overlay.IsHitTestVisible(false);
    C::Grid::SetColumnSpan(overlay, (std::max)(1u, active.layout.ColumnDefinitions().Size()));
    C::Grid::SetRowSpan(overlay, (std::max)(1u, active.layout.RowDefinitions().Size()));
    C::Canvas::SetZIndex(overlay, -1);
    C::Border background;
    background.CornerRadius(active.layout.CornerRadius());
    background.Opacity(0);
    overlay.Children().Append(background);
    active.layout.Children().Append(overlay);
    active.arrowLayer = overlay;
    active.arrowBackground = background;
    active.visualStateChanged = active.commonStates.CurrentStateChanged(winrt::auto_revoke,
        [row = &active](auto const&, X::VisualStateChangedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                if (!active.owner->closing && active.zone == HoverZone::Arrow && args.NewState() &&
                    args.NewState().Name() != L"Normal")
                    active.hoverUpdate.Start();
            });
        });
}

bool AttachPointerEvents(RowInteraction& active) {
    active.split.ApplyTemplate();
    active.layout = FindNamed(active.split, L"LayoutRoot").try_as<C::Grid>();
    active.chevron = FindNamed(active.split, L"SubItemChevron");
    if (!active.layout || !active.chevron) return false;
    active.expand = GetProvider<AV::IExpandCollapseProvider>(active.split, AP::PatternInterface::ExpandCollapse);
    if (active.mode == HoverMode::ArrowClick && !active.expand) return false;
    if (active.mode != HoverMode::Delayed && active.expand) {
        // Let XAML finish routing the enter event before opening its submenu.
        active.hoverUpdate = X::DispatcherTimer();
        active.hoverUpdate.Interval(std::chrono::milliseconds(1));
        active.tick = active.hoverUpdate.Tick(winrt::auto_revoke, [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) {
                active.hoverUpdate.Stop();
                if (active.owner->closing) return;
                const bool open = std::exchange(active.openOnUpdate, false);
                const auto provider = active.expand;
                const auto owner = active.owner;
                UpdateHoverVisual(active);
                if (open && session.get() == owner && !owner->closing && owner->menu.IsOpen() &&
                    provider.ExpandCollapseState() == X::Automation::ExpandCollapseState::Collapsed)
                    provider.Expand();
            });
        });
        if (active.mode == HoverMode::Arrow || active.mode == HoverMode::ArrowClick) AddArrowBackground(active);
    }

    active.pointerEntered = active.layout.PointerEntered(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                // In arrow-only mode we drive the native PointerOver state explicitly
                // because the XAML handler pairs it with starting the submenu timer.
                if (active.mode == HoverMode::ArrowClick || (active.mode == HoverMode::Arrow && active.expand))
                    args.Handled(true);
                if (args.Pointer().PointerDeviceType() == PointerType::Touch) return;
                UpdatePointerZone(active, args);
            });
        });
    active.pointerMoved = active.layout.PointerMoved(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                if (args.Pointer().PointerDeviceType() != PointerType::Touch)
                    UpdatePointerZone(active, args);
            });
        });
    active.pointerExited = active.layout.PointerExited(winrt::auto_revoke,
        [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) {
                active.pressedPointer.reset();
                UpdateCommandCursor(*active.owner, active.split, false);
                SetHoverZone(active, HoverZone::Outside);
            });
        });
    if (!active.destination.item && active.mode != HoverMode::ArrowClick) return true;

    active.pointerPressed = active.layout.PointerPressed(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                const auto zone = PointerZone(active, args);
                if ((zone == HoverZone::Body ||
                     (zone == HoverZone::Arrow && active.mode == HoverMode::ArrowClick)) &&
                    (args.Pointer().PointerDeviceType() == PointerType::Touch ||
                     args.GetCurrentPoint(active.layout).Properties().IsLeftButtonPressed())) {
                    active.pressedPointer = args.Pointer().PointerId();
                    active.pressedZone = zone;
                    args.Handled(true);
                }
            });
        });
    active.pointerReleased = active.layout.PointerReleased(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                const bool pressedHere = std::exchange(active.pressedPointer, std::nullopt) == args.Pointer().PointerId();
                const auto zone = PointerZone(active, args);
                const bool clickOnly = active.mode == HoverMode::ArrowClick;
                if (pressedHere || (!clickOnly && zone == HoverZone::Body)) args.Handled(true);
                if (!pressedHere || active.owner->closing || active.pressedZone != zone) return;
                if (clickOnly && (zone == HoverZone::Arrow ||
                                  (zone == HoverZone::Body && !active.destination.item))) {
                    auto provider = active.expand;
                    if (provider.ExpandCollapseState() == X::Automation::ExpandCollapseState::Collapsed)
                        provider.Expand();
                } else if (zone == HoverZone::Body && active.destination.item) {
                    RunDestination(*active.owner, active.destination);
                }
            });
        });
    active.pointerCanceled = active.layout.PointerCanceled(winrt::auto_revoke,
        [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) {
                active.pressedPointer.reset();
                UpdateCommandCursor(*active.owner, active.split, false);
            });
        });
    return true;
}

constexpr wchar_t iconFont[] = L"Segoe Fluent Icons";
constexpr wchar_t shieldTaskGlyph = L'\uf760';

bool IconFontHasGlyph(wchar_t glyph) noexcept {
    winrt::com_ptr<IDWriteFactory> factory;
    winrt::com_ptr<IDWriteFontCollection> fonts;
    winrt::com_ptr<IDWriteFontFamily> family;
    winrt::com_ptr<IDWriteFont> font;
    UINT32 index{};
    BOOL found{}, hasGlyph{};
    return SUCCEEDED(DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory),
                                         reinterpret_cast<IUnknown**>(factory.put()))) &&
           SUCCEEDED(factory->GetSystemFontCollection(fonts.put(), FALSE)) &&
           SUCCEEDED(fonts->FindFamilyName(iconFont, &index, &found)) && found &&
           SUCCEEDED(fonts->GetFontFamily(index, family.put())) &&
           SUCCEEDED(family->GetFirstMatchingFont(DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
                                                  DWRITE_FONT_STYLE_NORMAL, font.put())) &&
           SUCCEEDED(font->HasCharacter(glyph, &hasGlyph)) && hasGlyph;
}

C::FontIcon MakeIcon(wchar_t glyph) {
    if (glyph == shieldTaskGlyph) {
        static const bool hasShieldTask = IconFontHasGlyph(shieldTaskGlyph);
        if (!hasShieldTask) glyph = L'\uea18';
    }
    C::FontIcon icon;
    icon.FontFamily(X::Media::FontFamily(iconFont));
    icon.FontSize(16);
    icon.Glyph(winrt::hstring(std::wstring_view(&glyph, 1)));
    return icon;
}

struct StockPowerAction {
    Destination destination;
    bool withUpdate;
};

struct StockMenu {
    std::vector<C::MenuFlyoutItemBase> rows;
    C::MenuFlyoutSubItem power{nullptr};
    std::vector<StockPowerAction> powerActions;
};

C::MenuFlyoutItem StockAction(StockMenu const& stock, std::wstring_view id) {
    const auto matches = [&](std::wstring_view candidate) -> C::MenuFlyoutItem {
        const auto title = Normalize(Twinui(candidate));
        if (title.empty()) return nullptr;
        for (auto const& item : stock.rows)
            if (auto command = item.try_as<C::MenuFlyoutItem>(); command && Normalize(command.Text()) == title)
                return command;
        return nullptr;
    };
    if (auto action = matches(id)) return action;
    if (id == L"10960") return matches(L"10944");
    if (id == L"10961") return matches(L"10945");
    return nullptr;
}

struct StockPowerCaption {
    CatalogItem const* item;
    std::wstring caption;
    unsigned keyCaption;
};

unsigned PowerKeyCaption(std::wstring_view action) {
    if (action == L"power:signOut") return 3041;
    if (action == L"power:sleep") return 3020;
    if (action == L"power:hibernate") return 3023;
    if (action == L"power:shutdown") return 3014;
    if (action == L"power:restart") return 3017;
    return 0;
}

std::vector<StockPowerCaption> ReadPowerCaptions(bool withUpdate) {
    std::vector<StockPowerCaption> captions;
    const auto add = [&](CatalogItem const& item, std::wstring_view caption, unsigned keyCaption) {
        if (!caption.empty()) captions.push_back({&item, Normalize(caption), keyCaption});
    };
    for (auto const& item : catalog) {
        if (!std::wstring_view(item.action).starts_with(L"power:") || !*item.resource) continue;
        if (!withUpdate) {
            const auto keyCaption = PowerKeyCaption(item.action);
            add(item, SystemString(item.resource), keyCaption);
            // Some stock captions differ from the system resource, for example Russian Sign out.
            add(item, item.names[DisplayLanguage()], keyCaption);
            continue;
        }
        const auto [plainId, estimatedId] = item.updateCaptions;
        if (!plainId) continue;
        const auto addResource = [&](unsigned id) {
            add(item, SystemString(L"shutdownux.dll,-" + std::to_wstring(id)), id + 1);
        };
        addResource(plainId);
        // Each of the eight estimates has an access-key resource next to it.
        // Shut down and restart share the block, with four IDs per estimate.
        for (unsigned offset = 0; offset < 32; offset += 4) addResource(estimatedId + offset);
    }
    return captions;
}

bool ReadStockPowerActions(StockMenu& stock) {
    static const auto regular = ReadPowerCaptions(false);
    for (auto const& row : stock.power.Items()) {
        if (row.try_as<C::MenuFlyoutSeparator>()) continue;
        const auto command = row.try_as<C::MenuFlyoutItem>();
        if (!command) return false;
        const auto caption = Normalize(command.Text());
        if (caption.empty()) return false;

        const auto matches = [&](StockPowerCaption const& entry) { return MatchesPowerCaption(caption, entry.caption); };
        auto const* candidates = &regular;
        auto found = std::ranges::find_if(*candidates, matches);
        const bool withUpdate = found == candidates->end();
        if (withUpdate) {
            static const auto updates = ReadPowerCaptions(true);
            candidates = &updates;
            found = std::ranges::find_if(*candidates, matches);
        }
        if (found == candidates->end()) return false;
        const auto* item = found->item;
        if (std::any_of(found + 1, candidates->end(), [&](StockPowerCaption const& entry) {
                return entry.item != item && matches(entry);
            }) || std::ranges::any_of(stock.powerActions, [&](StockPowerAction const& entry) {
                return entry.destination.item == item && entry.withUpdate == withUpdate;
            })) {
            return false;
        }
        stock.powerActions.push_back({{item, command, found->keyCaption}, withUpdate});
    }
    return !stock.powerActions.empty();
}

bool ExplorerStartsOnThisPc() {
    DWORD destination{};
    DWORD bytes = sizeof(destination);
    return RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                        L"LaunchTo", RRF_RT_REG_DWORD, nullptr, &destination, &bytes) == ERROR_SUCCESS &&
           destination == 1;
}

bool IsHomeEdition() {
    DWORD edition{};
    if (!GetProductInfo(10, 0, 0, 0, &edition)) return false;
    return edition == PRODUCT_CORE || edition == PRODUCT_CORE_N ||
           edition == PRODUCT_CORE_SINGLELANGUAGE || edition == PRODUCT_CORE_COUNTRYSPECIFIC;
}

// GENERATED_AVAILABILITY

std::optional<Destination> Resolve(CatalogItem const& item, StockMenu const& stock,
                                   AvailabilitySnapshot const& availability, bool hideAdminCommands) {
    const auto index = &item - catalog;
    if (availability.unavailable[index] || (hideAdminCommands && availability.credentialsRequired[index]))
        return std::nullopt;
    auto [kind, target] = Split(item.action, L':');
    Destination destination{&item};
    if (kind == L"native") {
        destination.original = StockAction(stock, target);
        if (!destination.original || !destination.original.IsEnabled()) return std::nullopt;
    }
    return destination;
}

wchar_t FoldQuickKey(wchar_t key) {
    wchar_t upper{};
    return LCMapStringEx(LOCALE_NAME_INVARIANT, LCMAP_UPPERCASE, &key, 1, &upper, 1,
                         nullptr, nullptr, 0) ? upper : key;
}

bool IsQuickKey(wchar_t key) {
    WORD type{};
    return key && GetStringTypeW(CT_CTYPE1, &key, 1, &type) && (type & (C1_ALPHA | C1_DIGIT));
}

std::wstring_view StockQuickKeyId(CatalogItem const& item) {
    const auto [kind, target] = Split(item.action, L':');
    if (kind == L"native") return target;
    if (target == L"network-status") return L"10912";
    if (target == L"about") return L"10915";
    if (target == L"powersleep") return L"10913";
    if (target == L"installed-apps") return L"10946";
    if (target == L"mmc.exe;devmgmt.msc") return L"10916";
    return {};
}

std::wstring_view FindStockQuickKeyVariant(C::MenuFlyoutItem const& original, std::wstring_view id) {
    if (!original) return id;
    const auto title = Normalize(original.Text());
    if (id == L"10960" && title != Normalize(Twinui(id))) return L"10944";
    if (id == L"10961" && title != Normalize(Twinui(id))) return L"10945";
    return id;
}

std::optional<QuickKeyMark> MenuQuickKey(StockMenu const& stock, Destination const& destination,
                                       bool powerOpener, std::wstring_view caption, wchar_t& key) {
    X::UIElement source{nullptr};
    std::wstring resource;
    if (powerOpener) {
        source = stock.power;
        resource = L"twinui.pcshell.dll,-10930";
    } else if (destination.item) {
        auto id = StockQuickKeyId(*destination.item);
        auto original = destination.original;
        if (!original && !id.empty()) {
            original = StockAction(stock, id);
            if (!original && id == L"10946") {
                id = L"10911";
                original = StockAction(stock, id);
            }
        }
        if (!original) return std::nullopt;
        source = original;
        if (destination.powerKeyCaption) {
            resource = L"shutdownux.dll,-" + std::to_wstring(destination.powerKeyCaption);
        } else if (!id.empty()) {
            resource = L"twinui.pcshell.dll,-" + std::wstring(FindStockQuickKeyVariant(original, id));
        }
    }
    if (!source) return std::nullopt;
    const auto access = source.AccessKey();
    if (access.size() != 1 || !IsQuickKey(access[0])) return std::nullopt;
    key = FoldQuickKey(access[0]);
    const auto raw = resource.empty() ? std::wstring{} : RawSystemString(resource);
    return MarkQuickKey(raw, caption, key, FoldQuickKey);
}

bool MenuInputCurrent(MenuSession const* owner) {
    return owner && !unloading && session.get() == owner && !owner->closing;
}

bool MenuInputActive(MenuSession const* owner) {
    return MenuInputCurrent(owner) && owner->menu.IsOpen();
}

AV::IExpandCollapseProvider QuickKeyExpander(MenuSession const& active,
                                            C::MenuFlyoutSubItem const& submenu) {
    const auto found = std::ranges::find_if(active.rows, [&](auto const& row) { return row->split == submenu; });
    return found == active.rows.end() ? nullptr : (*found)->expand;
}

bool QuickKeySubmenuOpen(MenuSession const& active, C::MenuFlyoutSubItem const& submenu) {
    const auto provider = QuickKeyExpander(active, submenu);
    return provider && provider.ExpandCollapseState() == X::Automation::ExpandCollapseState::Expanded;
}

void ExpandForQuickKey(MenuSession& active, C::MenuFlyoutSubItem const& submenu) {
    const auto provider = QuickKeyExpander(active, submenu);
    if (!provider) return;
    // XAML providers can dispatch menu events. Hold providers independently of the session.
    std::vector<AV::IExpandCollapseProvider> otherMenus;
    for (auto const& row : active.rows) {
        row->openOnUpdate = false;
        if (row->hoverUpdate) row->hoverUpdate.Stop();
        if (row->split != submenu && row->expand &&
            row->expand.ExpandCollapseState() == X::Automation::ExpandCollapseState::Expanded)
            otherMenus.push_back(row->expand);
    }
    auto owner = &active;
    for (auto const& other : otherMenus) {
        other.Collapse();
        if (!MenuInputActive(owner)) return;
    }
    if (provider.ExpandCollapseState() == X::Automation::ExpandCollapseState::Collapsed)
        provider.Expand();
}

void FocusQuickKeyRow(MenuSession& active, C::MenuFlyoutItemBase const& row) {
    if (!row.XamlRoot() || !row.Focus(X::FocusState::Keyboard)) return;
    if (!MenuInputActive(&active)) return;
    active.focusWhenLoaded = nullptr;
    X::BringIntoViewOptions options;
    options.AnimationDesired(false);
    row.StartBringIntoView(options);
}

void LaunchKeyboardCommand(MenuSession& active, MenuKeyRow const& row) {
    if (active.keyLaunch && active.keyLaunch.IsEnabled()) return;
    const auto destination = row.destination;
    X::VisualStateManager::GoToState(row.element, L"PointerOver", false);
    // The command may close the window synchronously. Leave time for the
    // compositor to display the same visual state as a pointer launch.
    active.keyLaunch = X::DispatcherTimer();
    active.keyLaunch.Interval(std::chrono::milliseconds(60));
    active.keyLaunchTick = active.keyLaunch.Tick(winrt::auto_revoke,
        [owner = &active, destination](auto const& sender, auto const&) {
            sender.template as<X::DispatcherTimer>().Stop();
            if (!MenuInputActive(owner)) return;
            try { RunDestination(*owner, destination); }
            catch (...) { Wh_Log(L"Win+X keyboard command failed"); }
        });
    active.keyLaunch.Start();
}

wchar_t QuickKeyFromInput(I::KeyRoutedEventArgs const& args) {
    BYTE keyboard[256]{};
    if (!GetKeyboardState(keyboard) || (keyboard[VK_CONTROL] & 0x80) || (keyboard[VK_MENU] & 0x80))
        return {};
    wchar_t text[8]{};
    // Do not alter the keyboard layout's dead-key state while translating this event.
    const auto length = ToUnicodeEx(static_cast<UINT>(args.OriginalKey()), args.KeyStatus().ScanCode,
                                    keyboard, text, ARRAYSIZE(text), 4, GetKeyboardLayout(0));
    return length == 1 && IsQuickKey(text[0]) ? FoldQuickKey(text[0]) : wchar_t{};
}

void OnQuickKey(MenuSession& active, I::KeyRoutedEventArgs const& args) {
    const wchar_t key = QuickKeyFromInput(args);
    if (!key) {
        active.keyCycle = {};
        return;
    }
    // Consume an auto-repeat before it changes the cycle or launches an action.
    if (args.KeyStatus().WasKeyDown) {
        if (active.keyCycle.key == key ||
            std::ranges::find(active.keyRows, key, &MenuKeyRow::key) != active.keyRows.end())
            args.Handled(true);
        return;
    }
    std::vector<QuickKeyTarget> targets;
    targets.reserve(active.keyRows.size());
    for (auto const& row : active.keyRows) {
        if (row.key != key) {
            targets.emplace_back();
            continue;
        }
        const bool opener = !row.destination.item;
        targets.push_back({row.key, !row.parent || QuickKeySubmenuOpen(active, row.parent),
                           static_cast<bool>(row.parent), opener,
                           opener && QuickKeySubmenuOpen(active, row.element.as<C::MenuFlyoutSubItem>())});
    }
    const auto decision = ChooseQuickKey(targets, active.quickKeys, key, active.keyCycle);
    if (decision.action == QuickKeyAction::Ignore) return;
    args.Handled(true);
    if (active.keyLaunch && active.keyLaunch.IsEnabled()) return;
    const auto& target = active.keyRows[decision.index];
    switch (decision.action) {
    case QuickKeyAction::Expand:
        active.focusWhenLoaded = nullptr;
        ExpandForQuickKey(active, target.element.as<C::MenuFlyoutSubItem>());
        break;
    case QuickKeyAction::Focus: {
        auto element = target.element;
        active.focusWhenLoaded = element;
        if (target.parent && !targets[decision.index].visible) ExpandForQuickKey(active, target.parent);
        if (MenuInputActive(&active) && active.focusWhenLoaded) FocusQuickKeyRow(active, element);
        break;
    }
    case QuickKeyAction::Launch:
        LaunchKeyboardCommand(active, target);
        break;
    case QuickKeyAction::Ignore:
        break;
    }
}

void OnMenuPreviewKey(MenuSession& active, I::KeyRoutedEventArgs const& args) {
    switch (args.Key()) {
    case Key::Up: case Key::Down: case Key::Left: case Key::Right:
    case Key::Home: case Key::End: case Key::PageUp: case Key::PageDown:
    case Key::Tab: case Key::Escape:
        active.keyCycle = {};
        active.focusWhenLoaded = nullptr;
        return;
    case Key::Enter: case Key::Space:
        break;
    default:
        if (active.quickKeys != QuickKeyMode::Off) OnQuickKey(active, args);
        return;
    }
    active.keyCycle = {};
    active.focusWhenLoaded = nullptr;
    for (auto node = args.OriginalSource().try_as<X::DependencyObject>(); node;
         node = X::Media::VisualTreeHelper::GetParent(node)) {
        auto element = node.try_as<C::MenuFlyoutItemBase>();
        if (!element) continue;
        const auto row = std::ranges::find(active.keyRows, element, &MenuKeyRow::element);
        if (row != active.keyRows.end() && row->destination.item &&
            (args.Key() == Key::Enter || element.try_as<C::MenuFlyoutItem>())) {
            args.Handled(true);
            if (!args.KeyStatus().WasKeyDown) LaunchKeyboardCommand(active, *row);
        }
        break;
    }
}

C::TextBlock FindCaptionBlock(X::DependencyObject const& node, std::wstring_view title,
                              std::wstring_view marked) {
    if (auto block = node.try_as<C::TextBlock>(); block && (block.Text() == title || block.Text() == marked))
        return block;
    for (int index = 0; index < X::Media::VisualTreeHelper::GetChildrenCount(node); ++index) {
        auto child = FindCaptionBlock(X::Media::VisualTreeHelper::GetChild(node, index), title, marked);
        if (child) return child;
    }
    return nullptr;
}

void ApplyQuickKeyMark(MenuKeyRow const& row) {
    if (!row.mark) return;
    const auto leaf = row.element.try_as<C::MenuFlyoutItem>();
    const auto caption = leaf ? leaf.Text() : row.element.as<C::MenuFlyoutSubItem>().Text();
    auto block = FindCaptionBlock(row.element, caption, row.mark->caption);
    if (!block) return;
    auto inlines = block.Inlines();
    inlines.Clear();
    const std::wstring_view text = row.mark->caption;
    const auto addRun = [&](std::wstring_view value, bool underline) {
        if (value.empty()) return;
        X::Documents::Run run;
        run.Text(winrt::hstring(value));
        if (underline) {
            X::Documents::Underline line;
            line.Inlines().Append(run);
            inlines.Append(line);
        } else {
            inlines.Append(run);
        }
    };
    addRun(text.substr(0, row.mark->index), false);
    addRun(text.substr(row.mark->index, 1), true);
    addRun(text.substr(row.mark->index + 1), false);
}

void BindMenuKeyPresenter(MenuSession& active, C::MenuFlyoutItemBase const& item) {
    for (auto node = X::Media::VisualTreeHelper::GetParent(item); node;
         node = X::Media::VisualTreeHelper::GetParent(node)) {
        auto presenter = node.try_as<C::MenuFlyoutPresenter>();
        if (!presenter) continue;
        if (std::ranges::find(active.keyPresenters, presenter, &MenuKeyPresenter::element) !=
            active.keyPresenters.end()) return;
        MenuKeyPresenter handlers;
        handlers.element = presenter;
        handlers.keyDown = presenter.PreviewKeyDown(winrt::auto_revoke,
            [owner = &active](auto const&, I::KeyRoutedEventArgs const& args) {
                if (!MenuInputActive(owner)) return;
                try { OnMenuPreviewKey(*owner, args); }
                catch (...) { Wh_Log(L"Could not process Win+X navigation key"); }
            });
        active.keyPresenters.push_back(std::move(handlers));
        return;
    }
}

void AddMenuKeyRow(MenuSession& active, C::MenuFlyoutItemBase const& element,
                   Destination const& destination, C::MenuFlyoutSubItem const& parent,
                   StockMenu const& stock, bool allowLetter, bool powerOpener = false) {
    MenuKeyRow row;
    row.element = element;
    row.parent = parent;
    row.destination = destination;
    if (active.quickKeys != QuickKeyMode::Off && allowLetter) {
        const auto leaf = element.try_as<C::MenuFlyoutItem>();
        const auto caption = leaf ? leaf.Text() : element.as<C::MenuFlyoutSubItem>().Text();
        row.mark = MenuQuickKey(stock, destination, powerOpener, caption, row.key);
        if (row.key) {
            X::Automation::AutomationProperties::SetAccessKey(
                element, winrt::hstring(std::wstring_view(&row.key, 1)));
        }
    }
    row.loaded = element.Loaded(winrt::auto_revoke,
        [owner = &active, index = active.keyRows.size()](auto const&, auto const&) {
            if (!MenuInputCurrent(owner)) return;
            try {
                auto const& row = owner->keyRows[index];
                BindMenuKeyPresenter(*owner, row.element);
                ApplyQuickKeyMark(row);
                if (MenuInputActive(owner) && owner->focusWhenLoaded == row.element)
                    FocusQuickKeyRow(*owner, row.element);
            } catch (...) { Wh_Log(L"Could not initialize Win+X keyboard controls"); }
        });
    active.keyRows.push_back(std::move(row));
}

void LeaveNativeAccessKeyMode() {
    // The shell enters native access-key mode when opening Win+X. That mode can
    // consume a letter before PreviewKeyDown, even if the cloned rows lack keys.
    // The tray message completes after the shell has shown the menu and cannot
    // call into the mod after its subclass is removed during unload.
    if (!trayWindow || !PostMessageW(trayWindow, setupMessage, resetNativeAccessKeys, 0))
        I::AccessKeyManager::ExitDisplayMode();
}

void RememberSubmenuRoot(SubmenuCleanup& cleanup, X::XamlRoot const& root) {
    if (root && std::ranges::find(cleanup.roots, root) == cleanup.roots.end()) cleanup.roots.push_back(root);
}

bool ContainsOwnedSubmenu(X::DependencyObject const& child, SubmenuCleanup const& cleanup) {
    if (!child) return false;
    if (const auto presenter = child.try_as<C::MenuFlyoutPresenter>()) {
        // Match cloned submenu entries by identity. An outer menu presenter
        // and an unrelated submenu with the same captions must stay untouched.
        for (auto const& item : presenter.Items()) {
            if (std::ranges::find(cleanup.items, item) != cleanup.items.end()) return true;
        }
        return false;
    }
    for (int index = 0; index < X::Media::VisualTreeHelper::GetChildrenCount(child); ++index) {
        if (ContainsOwnedSubmenu(X::Media::VisualTreeHelper::GetChild(child, index), cleanup)) return true;
    }
    return false;
}

std::vector<P::Popup> FindSubmenuPopups(SubmenuCleanup& cleanup) {
    std::vector<P::Popup> owned;
    if (cleanup.items.empty()) return owned;
    for (auto const& item : cleanup.items) RememberSubmenuRoot(cleanup, item.XamlRoot());
    for (auto const& root : cleanup.roots) {
        for (auto const& popup : X::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(root)) {
            if (std::ranges::find(owned, popup) == owned.end() && ContainsOwnedSubmenu(popup.Child(), cleanup))
                owned.push_back(popup);
        }
    }
    return owned;
}

void ClosePopupWindows(std::vector<P::Popup> const& popups) noexcept {
    for (auto const& popup : popups) {
        try { if (popup.IsOpen()) popup.IsOpen(false); }
        catch (...) { Wh_Log(L"Could not close the Win+X submenu popup"); }
    }
}

constexpr UINT submenuSweepInterval = 100;
constexpr ULONGLONG submenuCleanupWindow = 750;

UINT_PTR SubmenuCleanupTimerId() noexcept {
    return reinterpret_cast<UINT_PTR>(&pendingSubmenuCleanups);
}

void ClosePendingSubmenuPopups(bool retry = true) noexcept {
    // Closing a XAML popup may dispatch another flyout event.
    auto pending = std::exchange(pendingSubmenuCleanups, {});
    for (auto& cleanup : pending) {
        try { ClosePopupWindows(FindSubmenuPopups(cleanup)); }
        catch (...) { Wh_Log(L"Could not identify the detached Win+X submenu popups"); }

        // XAML can publish the popup after this message has already run.
        if (retry && !unloading && GetTickCount64() < cleanup.retryUntil) {
            try { pendingSubmenuCleanups.push_back(std::move(cleanup)); }
            catch (...) { Wh_Log(L"Could not retain the detached Win+X submenu cleanup"); }
        }
    }
    if (pendingSubmenuCleanups.empty() && trayWindow)
        KillTimer(trayWindow, SubmenuCleanupTimerId());
}

void QueueSubmenuCleanup(SubmenuCleanup&& cleanup) noexcept {
    if (cleanup.items.empty()) return;
    try {
        const bool first = pendingSubmenuCleanups.empty();
        cleanup.retryUntil = GetTickCount64() + submenuCleanupWindow;
        pendingSubmenuCleanups.push_back(std::move(cleanup));
        if (unloading || !trayWindow ||
            (first && !SetTimer(trayWindow, SubmenuCleanupTimerId(), submenuSweepInterval, nullptr))) {
            ClosePendingSubmenuPopups(false);
        } else if (first && !PostMessageW(trayWindow, setupMessage, sweepClosedSubmenus, 0)) {
            ClosePendingSubmenuPopups();
        }
    } catch (...) {
        Wh_Log(L"Could not schedule closing the detached Win+X submenus");
    }
}

void CloseSubmenus(MenuSession& active) noexcept {
    const bool firstClose = !std::exchange(active.closing, true);
    std::vector<P::Popup> popups;
    try { popups = FindSubmenuPopups(active.submenus); }
    catch (...) { Wh_Log(L"Could not identify the open Win+X submenu popups"); }

    std::vector<AV::IExpandCollapseProvider> expanded;
    if (firstClose) {
        for (auto const& row : active.rows) {
            row->openOnUpdate = false;
            try {
                if (row->hoverUpdate) row->hoverUpdate.Stop();
                if (row->expand && row->expand.ExpandCollapseState() == X::Automation::ExpandCollapseState::Expanded)
                    expanded.push_back(row->expand);
            } catch (...) {
                Wh_Log(L"Could not prepare the Win+X submenu to close");
            }
        }
    }
    // Closing can synchronously raise Closed on the parent. Use the references
    // captured above once XAML can dispatch events.
    for (auto const& provider : expanded) {
        try { provider.Collapse(); }
        catch (...) { Wh_Log(L"Could not close the Win+X submenu"); }
    }
    // XAML can hide the submenu HWND and report Collapsed while its Popup is
    // still open. Close that popup before restoring the original menu rows.
    ClosePopupWindows(popups);
}

void RemoveRowVisual(RowInteraction& row) {
    if (row.arrowLayer && row.layout) {
        uint32_t index{};
        auto children = row.layout.Children();
        if (children.IndexOf(row.arrowLayer, index)) children.RemoveAt(index);
    }
}

void Restore() noexcept {
    auto old = std::move(session);
    if (!old) return;
    CloseSubmenus(*old);
    ClearCommandCursor(*old);
    for (auto const& row : old->rows) {
        try { RemoveRowVisual(*row); } catch (...) {}
    }
    try { if (old->keyLaunch) old->keyLaunch.Stop(); }
    catch (...) { Wh_Log(L"Could not cancel the pending Win+X keyboard command"); }
    try {
        if (old->menuChanged) old->menu.Items().ReplaceAll(old->original);
    } catch (...) {
        Wh_Log(L"Could not restore the original Win+X menu");
    }
    // Keep tracking these exact items while XAML settles the closed menu.
    QueueSubmenuCleanup(std::move(old->submenus));
}

C::MenuFlyoutItem CreateLeaf(MenuSession& pending, Destination const& destination) {
    const auto caption = Caption(destination);
    if (caption.empty()) return nullptr;
    C::MenuFlyoutItem leaf;
    leaf.Text(caption);
    if (destination.item->glyph) leaf.Icon(MakeIcon(destination.item->glyph));
    auto owner = &pending;
    pending.clicks.push_back(leaf.Click(winrt::auto_revoke, [owner, destination](auto const&, auto const&) {
        if (unloading || session.get() != owner) return;
        try { RunDestination(*owner, destination); } catch (...) { Wh_Log(L"Win+X command failed"); }
    }));
    if (pending.commandCursor) {
        const auto enterOrMove = [owner](auto const& sender, I::PointerRoutedEventArgs const& args) {
            if (unloading || session.get() != owner) return;
            try {
                if (args.Pointer().PointerDeviceType() == PointerType::Mouse)
                    UpdateCommandCursor(*owner, sender.template as<X::UIElement>(), true);
            } catch (...) {
                Wh_Log(L"Could not update the Win+X command cursor");
            }
        };
        pending.cursorEvents.push_back({
            leaf.PointerEntered(winrt::auto_revoke, enterOrMove),
            leaf.PointerMoved(winrt::auto_revoke, enterOrMove),
            leaf.PointerExited(winrt::auto_revoke, [owner](auto const& sender, auto const&) {
                if (unloading || session.get() != owner) return;
                UpdateCommandCursor(*owner, sender.template try_as<X::UIElement>(), false);
            }),
        });
    }
    return leaf;
}

void AddInteraction(MenuSession& pending, C::MenuFlyoutSubItem const& split,
                    Destination const& destination, HoverMode mode) {
    auto row = std::make_unique<RowInteraction>();
    row->owner = &pending;
    row->split = split;
    row->destination = destination;
    row->mode = mode;
    pending.rows.push_back(std::move(row));
}

std::optional<StockMenu> ReadStockMenu(C::MenuFlyout const& menu) {
    const auto items = menu.Items();
    if (items.Size() < 10) return std::nullopt;

    StockMenu stock;
    const auto powerTitle = Normalize(Twinui(L"10930"));
    if (powerTitle.empty()) return std::nullopt;
    for (auto const& item : items) {
        stock.rows.push_back(item);
        if (auto sub = item.try_as<C::MenuFlyoutSubItem>(); sub && Normalize(sub.Text()) == powerTitle)
            stock.power = sub;
    }
    if (!stock.power || !StockAction(stock, L"10921") || !StockAction(stock, L"10923") ||
        !StockAction(stock, L"10925")) return std::nullopt;
    return stock;
}

void TryPatchMenu(C::MenuFlyout const& menu) {
    auto nativeMenu = ReadStockMenu(menu);
    if (!nativeMenu) return;
    const bool recognizedPowerMenu = ReadStockPowerActions(*nativeMenu);
    const auto& stock = *nativeMenu;
    const auto items = menu.Items();
    const auto snapshot = ReadSettings();

    RefreshAvailability();
    const auto availability = ReadAvailability();

    auto pending = std::make_unique<MenuSession>();
    pending->menu = menu;
    pending->original = stock.rows;
    pending->quickKeys = snapshot.quickKeys;
    if (snapshot.showHandCursor) pending->commandCursor = LoadCursorW(nullptr, IDC_HAND);

    struct ChildRow {
        C::MenuFlyoutItem element;
        Destination destination;
    };
    std::vector<C::MenuFlyoutItemBase> visible;
    int previousGroup = -1;
    for (size_t index = 0; index < std::size(catalog); ++index) {
        const auto& item = catalog[index];
        if (item.parent >= 0 || !snapshot.enabledSections[item.group] || !snapshot.enabled[index]) continue;
        const std::wstring_view action(item.action);
        const bool power = action == L"power:menu";
        const bool elevateTerminal = snapshot.terminalAsAdmin && action == L"native:10960";
        auto destination = power ? std::optional<Destination>{} :
            Resolve(item, stock, availability, snapshot.hideAdminCommands);

        std::vector<ChildRow> children;
        for (size_t child = index + 1; child < std::size(catalog) && catalog[child].parent == static_cast<int>(index); ++child) {
            const auto& command = catalog[child];
            // Stock power commands are appended below in the order Windows presents them.
            if (power && (!recognizedPowerMenu || std::wstring_view(command.action).starts_with(L"power:"))) continue;
            const bool elevatedTerminal = elevateTerminal && std::wstring_view(command.action) == L"native:10961";
            const bool primaryPower = power && snapshot.primaryPowerAction == command.action;
            if (!snapshot.enabled[child] && !elevatedTerminal && !primaryPower) continue;
            auto resolved = Resolve(command, stock, availability, snapshot.hideAdminCommands);
            if (primaryPower) {
                destination = std::move(resolved);
                continue;
            }
            if (!resolved) continue;
            if (elevatedTerminal) std::swap(resolved, destination);
            if (snapshot.enabled[child] && resolved) {
                if (auto leaf = CreateLeaf(*pending, *resolved)) children.push_back({leaf, *resolved});
            }
        }
        if (power && recognizedPowerMenu) {
            for (auto const& command : stock.powerActions) {
                auto const& native = command.destination;
                if (!native.original.IsEnabled()) continue;
                const auto placement = PlacePowerAction(command.withUpdate,
                    snapshot.primaryPowerAction == native.item->action, snapshot.enabled[native.item - catalog]);
                if (placement == PowerPlacement::Main) destination = native;
                if (placement == PowerPlacement::Submenu) {
                    if (auto leaf = CreateLeaf(*pending, native)) children.push_back({leaf, native});
                }
            }
        }

        C::MenuFlyoutItemBase entry{nullptr};
        C::MenuFlyoutSubItem submenu{nullptr};
        if (power && !recognizedPowerMenu) {
            entry = stock.power;
        } else if (children.empty()) {
            if (!destination) continue;
            entry = CreateLeaf(*pending, *destination);
        } else {
            const auto caption = power && !destination ? MenuCaption(stock.power.Text()) :
                Caption(destination.value_or(Destination{&item}));
            if (caption.empty()) continue;
            C::MenuFlyoutSubItem split;
            split.Text(caption);
            const auto glyph = destination ? destination->item->glyph : item.glyph;
            if (glyph) split.Icon(MakeIcon(glyph));
            for (auto const& child : children) split.Items().Append(child.element);
            AddInteraction(*pending, split, destination.value_or(Destination{}), snapshot.mode);
            entry = split;
            submenu = split;
        }
        if (!entry) continue;
        const bool powerOpener = power && recognizedPowerMenu && !destination;
        AddMenuKeyRow(*pending, entry, destination.value_or(Destination{}), nullptr, stock,
                      !power || powerOpener, powerOpener);
        for (auto const& child : children) {
            pending->submenus.items.push_back(child.element);
            AddMenuKeyRow(*pending, child.element, child.destination, submenu, stock, true);
        }
        if (previousGroup >= 0 && previousGroup != item.group) visible.push_back(C::MenuFlyoutSeparator());
        visible.push_back(entry);
        previousGroup = item.group;
    }
    if (visible.empty()) return;

    pending->opened = menu.Opened(winrt::auto_revoke, [owner = pending.get()](auto const&, auto const&) {
        if (unloading || session.get() != owner) return;
        try {
            for (auto const& row : owner->rows) {
                RememberSubmenuRoot(owner->submenus, row->split.XamlRoot());
                if (!AttachPointerEvents(*row)) {
                    Wh_Log(L"Incompatible Win+X submenu template; restoring the Windows menu");
                    Restore();
                    return;
                }
            }
            for (auto const& item : owner->keyRows) {
                if (!item.parent) {
                    BindMenuKeyPresenter(*owner, item.element);
                    ApplyQuickKeyMark(item);
                }
            }
            LeaveNativeAccessKeyMode();
        } catch (...) {
            Wh_Log(L"Could not initialize Win+X submenu interactions; restoring the Windows menu");
            Restore();
        }
    });
    pending->closingEvent = menu.Closing(winrt::auto_revoke, [owner = pending.get()](auto const&, auto const&) {
        if (session.get() == owner && !owner->closing) CloseSubmenus(*owner);
    });
    pending->closed = menu.Closed(winrt::auto_revoke, [owner = pending.get()](auto const&, auto const&) {
        if (session.get() == owner) Restore();
    });
    session = std::move(pending);
    session->menuChanged = true;
    items.ReplaceAll(visible);
}

winrt::weak_ref<C::MenuFlyout> observedMenu;
[[clang::no_destroy]] P::FlyoutBase::Opening_revoker openingHandler;

template<typename Interface>
C::MenuFlyout Inspect(void* abi) noexcept {
    if (unloading || GetCurrentThreadId() != trayThread) return nullptr;
    C::MenuFlyout menu{nullptr};
    try {
        Interface flyout{nullptr};
        winrt::copy_from_abi(flyout, abi);
        menu = flyout.template try_as<C::MenuFlyout>();
        if (!menu || menu == observedMenu.get() || (session && menu == session->menu) ||
            menu.IsOpen() || !ReadStockMenu(menu)) return menu;

        // A new Win+X flyout can be shown before the old one has raised Closed.
        // Observe the new flyout's Opening independently of the closing session.
        auto weak = winrt::make_weak(menu);
        openingHandler = menu.Opening(winrt::auto_revoke, [weak](auto const&, auto const&) {
            if (unloading) return;
            C::MenuFlyout menu{nullptr};
            try {
                menu = weak.get();
                if (!menu) return;
                if (session && session->closing) Restore();
                if (!session) TryPatchMenu(menu);
            } catch (...) {
                if (menu && session && session->menu == menu) Restore();
                Wh_Log(L"Could not customize the Quick Link menu");
            }
        });
        observedMenu = std::move(weak);
    } catch (...) {
        Wh_Log(L"Could not observe the Quick Link menu");
    }
    return menu;
}

HRESULT WINAPI HookShowAt(void* self, void* target) {
    const auto menu = Inspect<P::IFlyoutBase>(self);
    const auto result = originalShowAt(self, target);
    if (FAILED(result) && menu && session && session->menu == menu) Restore();
    return result;
}

HRESULT WINAPI HookShowAtOptions(void* self, void* target, void* options) {
    const auto menu = Inspect<P::IFlyoutBase5>(self);
    const auto result = originalShowAtOptions(self, target, options);
    if (FAILED(result) && menu && session && session->menu == menu) Restore();
    return result;
}

enum class TaskbarSetup : LRESULT { Unanswered, Failed, Ready, WaitForXaml };

TaskbarSetup InstallHooks() {
    void* show;
    void* showOptions;
    try {
        C::MenuFlyout sample;
        const auto base = sample.as<P::IFlyoutBase>();
        const auto options = sample.as<P::IFlyoutBase5>();
        // The slots are the two ShowAt signatures in the Windows Runtime interfaces.
        show = (*reinterpret_cast<void***>(winrt::get_abi(base)))[14];
        showOptions = (*reinterpret_cast<void***>(winrt::get_abi(options)))[12];
    } catch (winrt::hresult_error const& error) {
        if (error.code() == RPC_E_WRONG_THREAD || error.code() == CO_E_NOTINITIALIZED)
            return TaskbarSetup::WaitForXaml;
        throw;
    }

    if (show == showOptions) {
        Wh_Log(L"Quick Link Menu hook targets unexpectedly share the same address");
        return TaskbarSetup::Failed;
    }
    if (!WindhawkUtils::SetFunctionHook(reinterpret_cast<ShowAt>(show), HookShowAt, &originalShowAt)) {
        Wh_Log(L"Could not register the Quick Link Menu ShowAt hook");
        return TaskbarSetup::Failed;
    }
    const bool optionsHook = WindhawkUtils::SetFunctionHook(
        reinterpret_cast<ShowAtOptions>(showOptions), HookShowAtOptions, &originalShowAtOptions);
    if (optionsHook && Wh_ApplyHookOperations()) return TaskbarSetup::Ready;

    Wh_Log(L"Quick Link Menu hook setup failed during %ls",
           optionsHook ? L"hook application" : L"ShowAt with options registration");
    const bool removedOptions = !optionsHook || Wh_RemoveFunctionHook(showOptions);
    const bool removedShow = Wh_RemoveFunctionHook(show);
    const bool appliedRemoval = Wh_ApplyHookOperations();
    if (!removedOptions || !removedShow || !appliedRemoval) Wh_Log(L"Could not roll back the Quick Link Menu hooks");
    return TaskbarSetup::Failed;
}

LRESULT CALLBACK TraySubclass(HWND window, UINT message, WPARAM action, LPARAM data, DWORD_PTR) {
    if (message == setupMessage) {
        if (action == sweepClosedSubmenus) {
            ClosePendingSubmenuPopups();
            return 0;
        }
        if (action == resetNativeAccessKeys) {
            try {
                if (MenuInputActive(session.get())) I::AccessKeyManager::ExitDisplayMode();
            } catch (...) { Wh_Log(L"Could not leave native Win+X access-key mode"); }
            return 0;
        }
        if (action) {
            try {
                return static_cast<LRESULT>(unloading ? TaskbarSetup::Failed : InstallHooks());
            } catch (winrt::hresult_error const& error) {
                Wh_Log(L"Quick Link Menu XAML hook setup failed (HRESULT 0x%08lX)",
                       static_cast<unsigned long>(error.code().value));
            } catch (...) {
                Wh_Log(L"Quick Link Menu hook setup failed with an unexpected exception");
            }
            return static_cast<LRESULT>(TaskbarSetup::Failed);
        }

        try {
            openingHandler.revoke();
            observedMenu = {};
            if (session) {
                auto menu = session->menu;
                menu.Hide();
            }
        } catch (...) {
            Wh_Log(L"Quick Link Menu taskbar cleanup failed");
        }
        Restore();
        ClosePendingSubmenuPopups();
        return 0;
    }
    if (window == trayWindow) {
        if (message == WM_TIMER && action == SubmenuCleanupTimerId()) {
            ClosePendingSubmenuPopups();
            return 0;
        }
        if (message == WM_NCDESTROY) {
            KillTimer(window, SubmenuCleanupTimerId());
            pendingSubmenuCleanups.clear();
        }
    }
    return DefSubclassProc(window, message, action, data);
}

bool FinishTaskbarStartup() noexcept {
    const HWND window = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid{};
    const DWORD thread = window ? GetWindowThreadProcessId(window, &pid) : 0;
    if (!thread) {
        subclassRetryWindow = nullptr;
        return false;
    }
    // A separate Explorer window process has no Win+X menu to customize.
    if (pid != GetCurrentProcessId() || unloading) return true;
    if (!GetModuleHandleW(L"Windows.UI.Xaml.dll")) return false;

    subclassPresent = WindhawkUtils::SetWindowSubclassFromAnyThread(window, TraySubclass, 0);
    if (!subclassPresent) {
        if (FindWindowW(L"Shell_TrayWnd", nullptr) != window) {
            subclassRetryWindow = nullptr;
            return false;
        }
        // A stable taskbar window gets one retry after a subclass failure.
        if (std::exchange(subclassRetryWindow, window) != window) return false;
        Wh_Log(L"Could not attach Quick Link Menu to the taskbar after retrying");
        return true;
    }
    subclassRetryWindow = nullptr;

    const auto result = static_cast<TaskbarSetup>(SendMessageW(window, setupMessage, 1, 0));
    if (result == TaskbarSetup::Ready) {
        trayWindow = window;
        trayThread = thread;
        RefreshAvailability();
        if (!moduleWorkQueue.Submit(WarmSystemCaptions)) Wh_Log(L"Could not schedule Quick Link menu caption resolution");
        return true;
    }
    WindhawkUtils::RemoveWindowSubclassFromAnyThread(window, TraySubclass);
    subclassPresent = false;
    if (result == TaskbarSetup::WaitForXaml) return false;
    if (result != TaskbarSetup::Failed) {
        if (FindWindowW(L"Shell_TrayWnd", nullptr) != window) return false;
        Wh_Log(L"Quick Link Menu setup message returned an unexpected result (%lld)", static_cast<long long>(result));
    }
    return true;
}

void ArmTaskbarTimer(PTP_TIMER timer, DWORD interval) {
    LARGE_INTEGER delay;
    delay.QuadPart = -static_cast<LONGLONG>(interval) * 10'000;
    FILETIME due{delay.LowPart, static_cast<DWORD>(delay.HighPart)};
    SetThreadpoolTimer(timer, &due, interval, 50);
}

void CALLBACK WaitForTaskbar(PTP_CALLBACK_INSTANCE, PVOID, PTP_TIMER timer) noexcept {
    if (!TryAcquireSRWLockExclusive(&taskbarStartupLock)) return;
    if (!unloading && IsThreadpoolTimerSet(timer)) {
        if (FinishTaskbarStartup()) {
            SetThreadpoolTimer(timer, nullptr, 0, 0);
        } else if (taskbarFastRetryDeadline && GetTickCount64() >= taskbarFastRetryDeadline) {
            taskbarFastRetryDeadline = 0;
            Wh_Log(L"Quick Link menu initialization is still pending; retrying every two seconds");
            ArmTaskbarTimer(timer, taskbarSlowRetryInterval);
        }
    }
    ReleaseSRWLockExclusive(&taskbarStartupLock);
}

}  // namespace

BOOL Wh_ModInit() {
    const HWND window = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid{};
    if (window && GetWindowThreadProcessId(window, &pid) && pid != GetCurrentProcessId()) return FALSE;
    setupMessage = RegisterWindowMessageW(L"Windhawk.QuickLinkMenu.Setup");
    if (!setupMessage) return FALSE;
    LoadSettings();
    return moduleWorkQueue.Start();
}

void Wh_ModAfterInit() {
    if (FinishTaskbarStartup()) return;

    // At sign-in Windhawk can initialize this mod before Explorer creates the taskbar.
    taskbarStartupTimer = CreateThreadpoolTimer(WaitForTaskbar, nullptr, nullptr);
    if (!taskbarStartupTimer) {
        Wh_Log(L"Could not schedule Quick Link menu initialization");
        return;
    }
    taskbarFastRetryDeadline = GetTickCount64() + taskbarFastRetryDuration;
    ArmTaskbarTimer(taskbarStartupTimer, taskbarRetryInterval);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    RefreshAvailability(true);
}

void Wh_ModBeforeUninit() {
    unloading = true;
    if (taskbarStartupTimer) {
        AcquireSRWLockExclusive(&taskbarStartupLock);
        SetThreadpoolTimer(taskbarStartupTimer, nullptr, 0, 0);
        ReleaseSRWLockExclusive(&taskbarStartupLock);
        WaitForThreadpoolTimerCallbacks(taskbarStartupTimer, TRUE);
        CloseThreadpoolTimer(taskbarStartupTimer);
        taskbarStartupTimer = nullptr;
    }
    if (subclassPresent && IsWindow(trayWindow)) {
        SendMessageW(trayWindow, setupMessage, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(trayWindow, TraySubclass);
    }
}

void Wh_ModUninit() {
    moduleWorkQueue.Stop();
}

// ==WindhawkMod==
// @id              quick-link-menu
// @name            Quick Link Menu
// @description     Customize the Win+X menu and add more commands (right-click the Start button)
// @description:ru-RU Кастомизация и расширение возможностей меню Win+X (правый клик по кнопке «Пуск»)
// @description:es-ES Personaliza el menú Win+X y añade más comandos (clic derecho en el botón Inicio)
// @description:de-DE Das Win+X-Menü anpassen und um weitere Befehle ergänzen (Rechtsklick auf die Startschaltfläche)
// @description:pt-BR Personalize o menu Win+X e adicione mais comandos (clique com o botão direito no botão Iniciar)
// @description:zh-CN 自定义 Win+X 菜单并添加更多命令（右键单击“开始”按钮）
// @version         0.5.7
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lruntimeobject -lole32 -loleaut32 -lshell32 -lshlwapi -ladvapi32 -liphlpapi -luuid -ldwrite
// ==/WindhawkMod==
// Build: 39

// ==WindhawkModReadme==
/*
# Quick Link Menu

**English** · [Русский](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/Quick_Link_Menu/README.ru.md)

Quick Link Menu customizes the native Windows 11 menu opened by **Win+X** or by
right-clicking the Start button. It adds shortcuts to system settings and tools
and lets you choose which commands appear.

## Features

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
- **Windows labels and icons.** Menu labels follow the Windows display language
  and entries use Segoe Fluent Icons. Windhawk settings are available in English,
  Russian, Spanish, German, Brazilian Portuguese, and Simplified Chinese.

## Platforms

Requires Windows 11 x64. Incompatible with Windows 10 Win+X Restorer.

## Usage

Click a row's label or press **Enter** to run its main action. Click the arrow
or press **→** to open the submenu. A row with only a submenu opens it when
you click the label.

- **Menu contents.** Use the switches in the mod's **Settings** to choose
  which sections, rows, and submenu commands appear. Turning off a row hides
  its submenu. If no commands remain, Windows opens its standard Win+X menu.
- **General settings.** Choose immediate hover, a delay, arrow hover, or arrow
  click to open a submenu. The hand cursor marks areas that run a command.
  The optional administrator filter hides known commands that require
  administrator credentials or whose elevation is blocked. Commands that only
  need UAC consent stay visible.
- **Command settings.** The main Terminal action can run as administrator.
  Choose a main power action or leave only the power submenu. The default
  main action is **Shut down**.

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
#include <winrt/Windows.UI.Xaml.Automation.Peers.h>
#include <winrt/Windows.UI.Xaml.Automation.Provider.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Controls.Primitives.h>
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
};

// GENERATED_CATALOG

enum class HoverMode { Immediate, Delayed, Arrow, ArrowClick };
enum class HoverZone { Outside, Body, Arrow };

struct Settings {
    HoverMode mode{};
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
    X::UIElement::PreviewKeyDown_revoker keyDown;
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

struct MenuSession {
    C::MenuFlyout menu{nullptr};
    std::vector<C::MenuFlyoutItemBase> original;
    C::MenuFlyoutSubItem power{nullptr};
    std::vector<C::MenuFlyoutItemBase> originalPower;
    winrt::Windows::Foundation::IInspectable oldPowerIcon{nullptr};
    bool powerChanged{};
    bool menuChanged{};
    bool closing{};
    HCURSOR commandCursor{};
    HCURSOR previousCursor{};
    X::UIElement cursorElement{nullptr};
    std::vector<std::unique_ptr<RowInteraction>> rows;
    std::vector<C::MenuFlyoutItem::Click_revoker> clicks;
    std::vector<CursorEvents> cursorEvents;
    P::FlyoutBase::Opened_revoker opened;
    P::FlyoutBase::Closing_revoker closingEvent;
    P::FlyoutBase::Closed_revoker closed;
};

[[clang::no_destroy]] std::unique_ptr<MenuSession> session;
std::array<std::optional<std::wstring>, std::size(catalog)> systemCaptionCache;
std::mutex settingsLock;
Settings settings;
std::atomic<bool> unloading;
std::atomic<unsigned> settingsNavigationGeneration;
HWND trayWindow;
std::atomic<DWORD> trayThread;
UINT setupMessage;
bool subclassPresent;
constexpr DWORD taskbarRetryInterval = 250;
constexpr ULONGLONG taskbarStartupTimeout = 60'000;
PTP_TIMER taskbarStartupTimer;
ULONGLONG taskbarStartupDeadline;
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
    const auto value = Wh_GetStringSetting(L"general.submenuHover");
    const std::wstring_view option = value ? value : L"";
    next.mode = option == L"delayed" ? HoverMode::Delayed :
                option == L"arrow" ? HoverMode::Arrow :
                option == L"click" ? HoverMode::ArrowClick : HoverMode::Immediate;
    Wh_FreeStringSetting(value);
    next.hideAdminCommands = Wh_GetIntSetting(L"general.hideAdminCommands") != 0;
    next.showHandCursor = Wh_GetIntSetting(L"general.showHandCursor") != 0;
    next.terminalAsAdmin = Wh_GetIntSetting(L"commandOptions.terminalAsAdmin") != 0;
    const auto power = Wh_GetStringSetting(L"commandOptions.primaryPowerAction");
    if (!power || !*power) next.primaryPowerAction = L"power:shutdown";
    else if (std::wstring_view(power) == L"lock") next.primaryPowerAction = L"lock:session";
    else if (std::wstring_view(power) != L"none") next.primaryPowerAction = L"power:" + std::wstring(power);
    Wh_FreeStringSetting(power);
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

std::wstring SystemString(std::wstring_view reference) {
    const auto token = L"@%SystemRoot%\\System32\\" + std::wstring(reference);
    auto result = Indirect(token.c_str());
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
    auto& cached = systemCaptionCache[&item - catalog];
    if (!cached) {
        auto caption = SystemCaption(item);
        if (caption.empty()) caption = item.names[DisplayLanguage()];
        cached = std::move(caption);
    }
    return *cached;
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
    HMODULE module{};
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

void CALLBACK SelectSettingsCategory(PTP_CALLBACK_INSTANCE callback, PVOID data) noexcept {
    std::unique_ptr<SettingsRoute> route(static_cast<SettingsRoute*>(data));
    FreeLibraryWhenCallbackReturns(callback, route->module);
    // The module reference keeps the code alive, but Windhawk's mod instance may be gone.
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

        // Wait for the URI's landing page, then invoke its parent breadcrumb.
        // Automation identifiers are independent of the Windows display language.
        const auto deadline = std::chrono::steady_clock::now() + std::chrono::milliseconds(2200);
        while (!unloading && settingsNavigationGeneration == route->generation && std::chrono::steady_clock::now() < deadline) {
            const HWND hwnd = FindWindowW(L"ApplicationFrameWindow", route->appName.c_str());
            winrt::com_ptr<IUIAutomationElement> window, page, bar, parent;
            winrt::com_ptr<IUIAutomationElementArray> crumbs;
            winrt::com_ptr<IUIAutomationInvokePattern> invoke;
            int count{};
            if (hwnd && SUCCEEDED(automation->ElementFromHandle(hwnd, window.put())) && window &&
                SUCCEEDED(window->FindFirst(TreeScope_Descendants, pageId.get(), page.put())) && page &&
                SUCCEEDED(window->FindFirst(TreeScope_Descendants, barId.get(), bar.put())) && bar &&
                SUCCEEDED(bar->FindAll(TreeScope_Descendants, crumbType.get(), crumbs.put())) && crumbs &&
                SUCCEEDED(crumbs->get_Length(&count)) && count == 2 &&
                SUCCEEDED(crumbs->GetElement(0, parent.put())) && parent &&
                SUCCEEDED(parent->GetCurrentPatternAs(UIA_InvokePatternId, __uuidof(IUIAutomationInvokePattern),
                                                     invoke.put_void())) && invoke &&
                settingsNavigationGeneration == route->generation && !unloading &&
                std::chrono::steady_clock::now() < deadline &&
                SUCCEEDED(invoke->Invoke())) return;
            Sleep(90);
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
            if (GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS,
                    reinterpret_cast<PCWSTR>(&SelectSettingsCategory), &route->module)) {
                if (TrySubmitThreadpoolCallback(SelectSettingsCategory, route.get(), nullptr))
                    route.release();
                else
                    FreeLibrary(route->module);
            }
            if (route) Wh_Log(L"Could not schedule Settings category navigation for %s", item.key);
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

struct StockMenu {
    std::vector<C::MenuFlyoutItemBase> rows;
    C::MenuFlyoutSubItem power{nullptr};
    std::vector<Destination> powerActions;
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

bool ReadStockPowerActions(StockMenu& stock) {
    const auto language = DisplayLanguage();
    for (auto const& row : stock.power.Items()) {
        if (row.try_as<C::MenuFlyoutSeparator>()) continue;
        const auto command = row.try_as<C::MenuFlyoutItem>();
        if (!command) return false;
        const auto caption = Normalize(command.Text());
        if (caption.empty()) return false;

        CatalogItem const* match{};
        for (auto const& item : catalog) {
            if (!std::wstring_view(item.action).starts_with(L"power:") || !*item.resource) continue;
            // Some Win+X labels differ from the system resource, for example Russian Sign out.
            if (caption != Normalize(Caption(Destination{&item})) && caption != Normalize(item.names[language]))
                continue;
            if (match) return false;
            match = &item;
        }
        if (!match || std::ranges::find(stock.powerActions, match, &Destination::item) != stock.powerActions.end())
            return false;
        stock.powerActions.push_back({match, command});
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
    } else if (kind == L"power") {
        const auto match = std::ranges::find(stock.powerActions, &item, &Destination::item);
        if (match == stock.powerActions.end() || !match->original.IsEnabled()) return std::nullopt;
        destination.original = match->original;
    }
    return destination;
}

std::vector<P::Popup> FindSubmenuPopups(MenuSession const& active) {
    std::vector<P::Popup> owned;
    if (active.rows.empty()) return owned;
    const auto root = active.rows.front()->split.XamlRoot();
    if (!root) return owned;
    for (auto const& popup : X::Media::VisualTreeHelper::GetOpenPopupsForXamlRoot(root)) {
        const auto presenter = popup.Child().try_as<C::MenuFlyoutPresenter>();
        if (!presenter || presenter.Items().Size() == 0) continue;
        const auto first = presenter.Items().GetAt(0).try_as<C::MenuFlyoutItemBase>();
        if (!first) continue;
        // The root lists rows, while a submenu lists the children of one row.
        uint32_t index;
        if (std::any_of(active.rows.begin(), active.rows.end(), [&](auto const& row) {
                return row->split.Items().IndexOf(first, index);
            })) owned.push_back(popup);
    }
    return owned;
}

void CloseSubmenus(MenuSession& active) noexcept {
    const bool firstClose = !std::exchange(active.closing, true);
    std::vector<P::Popup> popups;
    try { popups = FindSubmenuPopups(active); }
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
    for (auto const& popup : popups) {
        try { if (popup.IsOpen()) popup.IsOpen(false); }
        catch (...) { Wh_Log(L"Could not close the Win+X submenu popup"); }
    }
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
    try {
        if (old->powerChanged) {
            old->power.Items().ReplaceAll(old->originalPower);
            if (old->oldPowerIcon == X::DependencyProperty::UnsetValue())
                old->power.ClearValue(C::MenuFlyoutSubItem::IconProperty());
            else
                old->power.SetValue(C::MenuFlyoutSubItem::IconProperty(), old->oldPowerIcon);
        }
    } catch (...) {
        Wh_Log(L"Could not restore the original Win+X power submenu");
    }
    try {
        if (old->menuChanged) old->menu.Items().ReplaceAll(old->original);
    } catch (...) {
        Wh_Log(L"Could not restore the original Win+X menu");
    }
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
    if (destination.item) {
        row->keyDown = split.PreviewKeyDown(winrt::auto_revoke,
            [ptr = row.get()](auto const&, I::KeyRoutedEventArgs const& args) {
                if (args.Key() != Key::Enter) return;
                OnRow(ptr, [&](auto& active) {
                    args.Handled(true);
                    RunDestination(*active.owner, active.destination);
                });
            });
    }
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
    if (!nativeMenu || !ReadStockPowerActions(*nativeMenu)) return;
    const auto& stock = *nativeMenu;
    const auto items = menu.Items();
    const auto snapshot = ReadSettings();

    RefreshAvailability();
    const auto availability = ReadAvailability();

    auto pending = std::make_unique<MenuSession>();
    pending->menu = menu;
    pending->original = stock.rows;
    if (snapshot.showHandCursor) pending->commandCursor = LoadCursorW(nullptr, IDC_HAND);
    pending->power = stock.power;
    for (auto const& item : stock.power.Items()) pending->originalPower.push_back(item);
    pending->oldPowerIcon = stock.power.ReadLocalValue(C::MenuFlyoutSubItem::IconProperty());

    std::vector<C::MenuFlyoutItemBase> visible;
    std::vector<C::MenuFlyoutItemBase> powerChildren;
    wchar_t powerGlyph{};
    int previousGroup = -1;
    for (size_t index = 0; index < std::size(catalog); ++index) {
        const auto& item = catalog[index];
        if (item.parent >= 0 || !snapshot.enabledSections[item.group] || !snapshot.enabled[index]) continue;
        const std::wstring_view action(item.action);
        const bool power = action == L"power:menu";
        const bool elevateTerminal = snapshot.terminalAsAdmin && action == L"native:10960";
        auto destination = power ? std::optional<Destination>{} :
            Resolve(item, stock, availability, snapshot.hideAdminCommands);

        std::vector<C::MenuFlyoutItem> children;
        for (size_t child = index + 1; child < std::size(catalog) && catalog[child].parent == static_cast<int>(index); ++child) {
            const auto& command = catalog[child];
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
                if (auto leaf = CreateLeaf(*pending, *resolved)) children.push_back(leaf);
            }
        }

        C::MenuFlyoutItemBase entry{nullptr};
        if (power && !destination) {
            if (children.empty()) continue;
            powerChildren.assign(children.begin(), children.end());
            powerGlyph = item.glyph;
            AddInteraction(*pending, stock.power, {}, snapshot.mode);
            entry = stock.power;
        } else if (children.empty()) {
            if (!destination) continue;
            entry = CreateLeaf(*pending, *destination);
        } else {
            const auto caption = Caption(destination.value_or(Destination{&item}));
            if (caption.empty()) continue;
            C::MenuFlyoutSubItem split;
            split.Text(caption);
            const auto glyph = destination ? destination->item->glyph : item.glyph;
            if (glyph) split.Icon(MakeIcon(glyph));
            for (auto const& child : children) split.Items().Append(child);
            AddInteraction(*pending, split, destination.value_or(Destination{}), snapshot.mode);
            entry = split;
        }
        if (!entry) continue;
        if (previousGroup >= 0 && previousGroup != item.group) visible.push_back(C::MenuFlyoutSeparator());
        visible.push_back(entry);
        previousGroup = item.group;
    }
    if (visible.empty()) return;

    pending->opened = menu.Opened(winrt::auto_revoke, [owner = pending.get()](auto const&, auto const&) {
        if (unloading || session.get() != owner) return;
        try {
            for (auto const& row : owner->rows) {
                if (!AttachPointerEvents(*row)) {
                    Wh_Log(L"Incompatible Win+X submenu template; restoring the Windows menu");
                    Restore();
                    return;
                }
            }
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
    if (!powerChildren.empty()) {
        session->powerChanged = true;
        stock.power.Items().ReplaceAll(powerChildren);
        if (powerGlyph) stock.power.Icon(MakeIcon(powerGlyph));
    }
    session->menuChanged = true;
    items.ReplaceAll(visible);
}

[[clang::no_destroy]] winrt::weak_ref<C::MenuFlyout> observedMenu;
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

enum class TaskbarSetup : LRESULT { Failed, Ready, WaitForXaml };

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

    if (show == showOptions ||
        !Wh_SetFunctionHook(show, reinterpret_cast<void*>(HookShowAt), reinterpret_cast<void**>(&originalShowAt)))
        return TaskbarSetup::Failed;
    const bool optionsHook = Wh_SetFunctionHook(showOptions, reinterpret_cast<void*>(HookShowAtOptions),
                                                reinterpret_cast<void**>(&originalShowAtOptions));
    if (optionsHook && Wh_ApplyHookOperations()) return TaskbarSetup::Ready;

    if (optionsHook) Wh_RemoveFunctionHook(showOptions);
    Wh_RemoveFunctionHook(show);
    Wh_ApplyHookOperations();
    return TaskbarSetup::Failed;
}

LRESULT CALLBACK TraySubclass(HWND window, UINT message, WPARAM action, LPARAM data, DWORD_PTR) {
    if (message == setupMessage) {
        try {
            if (action) return unloading ? 0 : static_cast<LRESULT>(InstallHooks());
            openingHandler.revoke();
            observedMenu = {};
            if (session) {
                auto menu = session->menu;
                menu.Hide();
            }
        } catch (...) {
            Wh_Log(L"Quick Link menu taskbar operation failed");
        }
        if (!action) Restore();
        return 0;
    }
    return DefSubclassProc(window, message, action, data);
}

bool FinishTaskbarStartup() noexcept {
    const HWND window = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid{};
    const DWORD thread = window ? GetWindowThreadProcessId(window, &pid) : 0;
    if (!thread) return false;
    // A separate Explorer window process has no Win+X menu to customize.
    if (pid != GetCurrentProcessId() || unloading) return true;
    if (!GetModuleHandleW(L"Windows.UI.Xaml.dll")) return false;

    trayWindow = window;
    trayThread = thread;
    subclassPresent = WindhawkUtils::SetWindowSubclassFromAnyThread(trayWindow, TraySubclass, 0);
    const auto result = subclassPresent ? static_cast<TaskbarSetup>(SendMessageW(trayWindow, setupMessage, 1, 0))
                                        : TaskbarSetup::Failed;
    if (result == TaskbarSetup::WaitForXaml) {
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(trayWindow, TraySubclass);
        subclassPresent = false;
        return false;
    }
    if (result == TaskbarSetup::Ready) {
        RefreshAvailability();
    } else {
        Wh_Log(L"Could not initialize the Quick Link menu hooks");
    }
    return true;
}

void CALLBACK WaitForTaskbar(PTP_CALLBACK_INSTANCE, PVOID, PTP_TIMER timer) noexcept {
    if (!TryAcquireSRWLockExclusive(&taskbarStartupLock)) return;
    if (!unloading && IsThreadpoolTimerSet(timer)) {
        const bool finished = FinishTaskbarStartup();
        if (finished || GetTickCount64() >= taskbarStartupDeadline) {
            SetThreadpoolTimer(timer, nullptr, 0, 0);
            if (!finished) Wh_Log(L"Timed out waiting for the taskbar and XAML to initialize the Quick Link menu");
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
    // A pending callback can keep this DLL loaded across a disable and enable.
    ++settingsNavigationGeneration;
    unloading = false;
    LoadSettings();
    return TRUE;
}

void Wh_ModAfterInit() {
    if (FinishTaskbarStartup()) return;

    // At sign-in Windhawk can initialize this mod before Explorer creates the taskbar.
    taskbarStartupTimer = CreateThreadpoolTimer(WaitForTaskbar, nullptr, nullptr);
    if (!taskbarStartupTimer) {
        Wh_Log(L"Could not schedule Quick Link menu initialization");
        return;
    }
    taskbarStartupDeadline = GetTickCount64() + taskbarStartupTimeout;
    LARGE_INTEGER delay;
    delay.QuadPart = -static_cast<LONGLONG>(taskbarRetryInterval) * 10'000;
    FILETIME due{delay.LowPart, static_cast<DWORD>(delay.HighPart)};
    SetThreadpoolTimer(taskbarStartupTimer, &due, taskbarRetryInterval, 50);
}

void Wh_ModSettingsChanged() {
    LoadSettings();
    RefreshAvailability(true);
}

void Wh_ModBeforeUninit() {
    unloading = true;
    if (taskbarStartupTimer) {
        SetThreadpoolTimer(taskbarStartupTimer, nullptr, 0, 0);
        WaitForThreadpoolTimerCallbacks(taskbarStartupTimer, TRUE);
        CloseThreadpoolTimer(taskbarStartupTimer);
        taskbarStartupTimer = nullptr;
    }
    if (subclassPresent && IsWindow(trayWindow)) {
        SendMessageW(trayWindow, setupMessage, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(trayWindow, TraySubclass);
    }
}

// ==WindhawkMod==
// @id              quick-link-menu
// @name            Quick Link Menu
// @description     Customize and extend the Win+X menu (right-click the Start button)
// @description:ru-RU Настройка и расширение меню Win+X (правый клик по кнопке «Пуск»)
// @description:es-ES Personaliza y amplía el menú Win+X (clic derecho en el botón Inicio)
// @description:de-DE Das Win+X-Menü anpassen und erweitern (Rechtsklick auf die Startschaltfläche)
// @description:pt-BR Personalize e amplie o menu Win+X (clique direito no botão Iniciar)
// @description:zh-CN 自定义并扩展 Win+X 菜单（右键单击“开始”按钮）
// @version         0.2.12
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lruntimeobject -lole32 -loleaut32 -lshell32 -lshlwapi -ladvapi32 -liphlpapi -luuid
// ==/WindhawkMod==
// Build: 18

// ==WindhawkModReadme==
/*
# Quick Link Menu

Customize the native Windows 11 menu opened by **Win+X** or by right-clicking
the Start button.

Quick Link Menu organizes shortcuts into Windows Settings, Control Panel,
Administration, and Basic commands. The row opens its main destination.
A submenu offers related shortcuts and can be opened with the arrow or **Right**.
**Enter** runs the main action. The power row opens the Windows power submenu,
with **Lock** as its first command.

In the mod settings, the related shortcuts are listed with their main row.
Choose which rows and submenu items appear, and change how
submenus open on hover at the top of the settings page. Disabling all entries
restores the stock Win+X menu.

Windows 11 x64. Incompatible with Windows 10 Win+X Restorer.

[Project documentation](https://github.com/CTravkin/DSN_Windhawk_Mods/tree/main/Quick_Link_Menu)
*/
// ==/WindhawkModReadme==

// GENERATED_SETTINGS

#include <winsock2.h>
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
    bool enabledByDefault;
    wchar_t glyph;
    PCWSTR action;
    PCWSTR resource;
    std::array<PCWSTR, 6> names;
};

// GENERATED_CATALOG

enum class HoverMode { Immediate, Delayed, Arrow };
enum class HoverZone { Outside, Body, Arrow };

struct Settings {
    HoverMode mode{};
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

struct MenuSession {
    C::MenuFlyout menu{nullptr};
    std::vector<C::MenuFlyoutItemBase> original;
    C::MenuFlyoutSubItem power{nullptr};
    std::vector<C::MenuFlyoutItemBase> originalPower;
    winrt::Windows::Foundation::IInspectable oldPowerIcon{nullptr};
    bool powerChanged{};
    bool menuChanged{};
    std::vector<std::unique_ptr<RowInteraction>> rows;
    std::vector<C::MenuFlyoutItem::Click_revoker> clicks;
    P::FlyoutBase::Opened_revoker opened;
    P::FlyoutBase::Closed_revoker closed;
};

[[clang::no_destroy]] std::unique_ptr<MenuSession> session;
[[clang::no_destroy]] std::array<std::optional<std::wstring>, std::size(catalog)> captionCache;
std::mutex settingsLock;
Settings settings;
std::atomic<bool> unloading;
std::atomic<unsigned> settingsNavigation;
HWND trayWindow;
DWORD trayThread;
UINT setupMessage;
bool subclassPresent;

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
    const auto value = Wh_GetStringSetting(L"submenuHover");
    const std::wstring_view option = value ? value : L"";
    next.mode = option == L"delayed" ? HoverMode::Delayed :
                option == L"arrow" ? HoverMode::Arrow : HoverMode::Immediate;
    Wh_FreeStringSetting(value);
    for (size_t i = 0; i < std::size(catalog); ++i)
        next.enabled[i] = Wh_GetIntSetting(catalog[i].key) != 0;
    std::lock_guard guard(settingsLock);
    settings = next;
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

std::wstring Indirect(PCWSTR token) {
    wchar_t result[512]{};
    if (FAILED(SHLoadIndirectString(token, result, ARRAYSIZE(result), nullptr))) return {};
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

bool Exists(std::wstring const& path) {
    return !path.empty() && GetFileAttributesW(path.c_str()) != INVALID_FILE_ATTRIBUTES;
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

std::wstring SettingsCaption(std::wstring_view id) {
    const auto titleFor = [](std::wstring_view page, std::wstring_view field) {
        return Indirect((L"@{windows?ms-resource://Windows.UI.SettingsAppThreshold/SystemSettings/Resources/" +
                         std::wstring(page) + L"/" + std::wstring(field) + L"}").c_str());
    };
    // Windows uses the short title on systems without a battery.
    if (id == L"SettingsPagePowerAndBattery") {
        SYSTEM_POWER_STATUS status{};
        if (GetSystemPowerStatus(&status) && status.BatteryFlag == 128) {
            if (auto title = titleFor(id, L"AlternateDisplayName"); !title.empty()) return title;
        }
    }
    auto title = titleFor(id, L"DisplayName");
    if (title.empty() && id == L"SettingsPageAdvancedSystemSettings")
        return titleFor(L"SettingsPageRestoreDeveloperOptions", L"DisplayName");
    return title;
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
    auto& cached = captionCache[&item - catalog];
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

bool Launch(PCWSTR target, PCWSTR arguments = nullptr) {
    return reinterpret_cast<INT_PTR>(ShellExecuteW(nullptr, L"open", target, arguments, nullptr, SW_SHOWNORMAL)) > 32;
}

bool LaunchPidl(Pidl const& target) {
    if (!target) return false;
    SHELLEXECUTEINFOW info{};
    info.cbSize = sizeof(info);
    info.fMask = SEE_MASK_IDLIST;
    info.lpIDList = target.get();
    info.nShow = SW_SHOWNORMAL;
    return ShellExecuteExW(&info) != FALSE;
}

struct SettingsRoute {
    HMODULE module{};
    unsigned serial;
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
    const HRESULT initialized = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(initialized)) return;
    struct Apartment {
        ~Apartment() { CoUninitialize(); }
    } apartment;
    try {
        winrt::com_ptr<IUIAutomation> automation;
        if (FAILED(CoCreateInstance(__uuidof(CUIAutomation), nullptr, CLSCTX_INPROC_SERVER,
                                   __uuidof(IUIAutomation), automation.put_void()))) return;
        const auto pageId = TextCondition(*automation, UIA_AutomationIdPropertyId, route->pageElementId.c_str());
        const auto barId = TextCondition(*automation, UIA_AutomationIdPropertyId, L"PermanentNavigationViewBreadcrumbBar");
        const auto crumbType = TextCondition(*automation, UIA_ClassNamePropertyId,
                                            L"Microsoft.UI.Xaml.Controls.BreadcrumbBarItem");
        if (!pageId || !barId || !crumbType) return;

        // Wait for the URI's landing page, then invoke its parent breadcrumb.
        // Automation identifiers are independent of the Windows display language.
        for (int i = 0; i < 24 && !unloading && settingsNavigation == route->serial; ++i) {
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
                settingsNavigation == route->serial && !unloading && SUCCEEDED(invoke->Invoke())) return;
            Sleep(90);
        }
    } catch (...) {
    }
}

void RunDestination(MenuSession& active, Destination destination) {
    ++settingsNavigation;
    if (destination.original) {
        if (auto invoke = GetProvider<AV::IInvokeProvider>(destination.original, AP::PatternInterface::Invoke)) {
            auto menu = active.menu;
            invoke.Invoke();
            menu.Hide();
        }
        return;
    }

    const auto& item = *destination.item;
    const auto [kind, target] = Split(item.action, L':');
    auto menu = active.menu;
    menu.Hide();
    bool opened = false;
    if (kind == L"uri" || kind == L"category") {
        const auto [page, pageElementId] = Split(target, L';');
        const auto address = L"ms-settings:" + std::wstring(page);
        opened = Launch(address.c_str());
        if (opened && kind == L"category") {
            auto route = std::make_unique<SettingsRoute>();
            route->serial = settingsNavigation;
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
        }
    } else if (kind == L"folder") {
        opened = LaunchPidl(FolderId(target));
    } else if (kind == L"panel") {
        opened = LaunchPidl(ShellId(ControlPath(target)));
    } else if (kind == L"lock") {
        opened = LockWorkStation() != FALSE;
    } else if (kind == L"control") {
        const auto command = target.starts_with(L"Microsoft.") ? L"/name " + std::wstring(target) :
                                                                  std::wstring(target);
        opened = Launch(SystemPath(L"control.exe").c_str(), command.c_str());
    } else if (kind == L"exe") {
        const auto [program, argument] = Split(target, L';');
        const auto path = ProgramPath(program);
        auto parameters = std::wstring(argument);
        if (argument.ends_with(L".msc")) parameters = L"\"" + SystemPath(argument) + L"\"";
        if (program.ends_with(L".cpl")) {
            parameters = L"\"" + path + L"\"";
            opened = Launch(SystemPath(L"control.exe").c_str(), parameters.c_str());
        } else {
            opened = Launch(path.c_str(), parameters.empty() ? nullptr : parameters.c_str());
        }
    }
    if (!opened) Wh_Log(L"Could not open Quick Link menu item %s", item.key);
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
    if (std::exchange(active.zone, zone) == zone) return;
    active.openOnUpdate = active.expand &&
        ((active.mode == HoverMode::Immediate && zone != HoverZone::Outside) ||
         (active.mode == HoverMode::Arrow && zone == HoverZone::Arrow));
    UpdateHoverVisual(active);
    if (active.openOnUpdate) active.hoverUpdate.Start();
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
                if (active.zone == HoverZone::Arrow && args.NewState() &&
                    args.NewState().Name() != L"Normal")
                    active.hoverUpdate.Start();
            });
        });
}

void AttachPointerEvents(RowInteraction& active) {
    active.split.ApplyTemplate();
    active.layout = FindNamed(active.split, L"LayoutRoot").try_as<C::Grid>();
    active.chevron = FindNamed(active.split, L"SubItemChevron");
    if (!active.layout || !active.chevron) return;
    active.expand = GetProvider<AV::IExpandCollapseProvider>(active.split, AP::PatternInterface::ExpandCollapse);
    if (active.mode != HoverMode::Delayed && active.expand) {
        // Let XAML finish routing the enter event before opening its submenu.
        active.hoverUpdate = X::DispatcherTimer();
        active.hoverUpdate.Interval(std::chrono::milliseconds(1));
        active.tick = active.hoverUpdate.Tick(winrt::auto_revoke, [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) {
                active.hoverUpdate.Stop();
                if (std::exchange(active.openOnUpdate, false) &&
                    active.expand.ExpandCollapseState() == X::Automation::ExpandCollapseState::Collapsed)
                    active.expand.Expand();
                UpdateHoverVisual(active);
            });
        });
        if (active.mode == HoverMode::Arrow) AddArrowBackground(active);
    }

    active.pointerEntered = active.layout.PointerEntered(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                if (args.Pointer().PointerDeviceType() == PointerType::Touch) return;
                // In arrow-only mode we drive the native PointerOver state explicitly
                // because the XAML handler pairs it with starting the submenu timer.
                if (active.mode == HoverMode::Arrow && active.expand) args.Handled(true);
                SetHoverZone(active, PointerZone(active, args));
            });
        });
    active.pointerMoved = active.layout.PointerMoved(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                if (args.Pointer().PointerDeviceType() != PointerType::Touch)
                    SetHoverZone(active, PointerZone(active, args));
            });
        });
    active.pointerExited = active.layout.PointerExited(winrt::auto_revoke,
        [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) {
                active.pressedPointer.reset();
                SetHoverZone(active, HoverZone::Outside);
            });
        });
    if (!active.destination.item) return;

    active.pointerPressed = active.layout.PointerPressed(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                if (PointerZone(active, args) == HoverZone::Body &&
                    (args.Pointer().PointerDeviceType() == PointerType::Touch ||
                     args.GetCurrentPoint(active.layout).Properties().IsLeftButtonPressed())) {
                    active.pressedPointer = args.Pointer().PointerId();
                    args.Handled(true);
                }
            });
        });
    active.pointerReleased = active.layout.PointerReleased(winrt::auto_revoke,
        [row = &active](auto const&, I::PointerRoutedEventArgs const& args) {
            OnRow(row, [&](auto& active) {
                const bool pressedHere = std::exchange(active.pressedPointer, std::nullopt) == args.Pointer().PointerId();
                const bool body = PointerZone(active, args) == HoverZone::Body;
                if (pressedHere || body) args.Handled(true);
                if (pressedHere && body) RunDestination(*active.owner, active.destination);
            });
        });
    active.pointerCanceled = active.layout.PointerCanceled(winrt::auto_revoke,
        [row = &active](auto const&, auto const&) {
            OnRow(row, [](auto& active) { active.pressedPointer.reset(); });
        });
}

C::FontIcon MakeIcon(wchar_t glyph) {
    C::FontIcon icon;
    icon.FontFamily(X::Media::FontFamily(L"Segoe Fluent Icons"));
    icon.FontSize(16);
    icon.Glyph(winrt::hstring(std::wstring_view(&glyph, 1)));
    return icon;
}

struct StockMenu {
    std::vector<C::MenuFlyoutItemBase> rows;
    C::MenuFlyoutSubItem power{nullptr};
    std::vector<C::MenuFlyoutItemBase> powerRows;
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

C::MenuFlyoutItem StockPowerAction(StockMenu const& stock, std::wstring_view action) {
    const auto count = stock.powerRows.size();
    if (count < 3) return nullptr;

    // These commands have fixed places even when the two sleep modes are omitted.
    if (action == L"signOut") return stock.powerRows[0].try_as<C::MenuFlyoutItem>();
    if (action == L"shutdown") return stock.powerRows[count - 2].try_as<C::MenuFlyoutItem>();
    if (action == L"restart") return stock.powerRows[count - 1].try_as<C::MenuFlyoutItem>();
    if (count == 5) return stock.powerRows[action == L"sleep" ? 1 : 2].try_as<C::MenuFlyoutItem>();
    if (count != 4) return nullptr;

    const auto item = stock.powerRows[1].try_as<C::MenuFlyoutItem>();
    const auto expected = SystemString(action == L"sleep" ? L"authui.dll,-3019" : L"authui.dll,-3022");
    return item && !expected.empty() && Normalize(item.Text()) == Normalize(expected) ? item : nullptr;
}

bool ControlRegistered(std::wstring_view resource) {
    const auto [kind, guid] = Split(resource, L':');
    if (kind != L"shell") return true;
    const auto key = L"SOFTWARE\\Microsoft\\Windows\\CurrentVersion\\Explorer\\ControlPanel\\NameSpace\\" +
                     std::wstring(guid);
    HKEY handle{};
    if (RegOpenKeyExW(HKEY_LOCAL_MACHINE, key.c_str(), 0, KEY_READ, &handle) != ERROR_SUCCESS)
        return false;
    RegCloseKey(handle);
    return true;
}

bool ExplorerStartsOnThisPc() {
    DWORD destination{};
    DWORD bytes = sizeof(destination);
    return RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\CurrentVersion\\Explorer\\Advanced",
                        L"LaunchTo", RRF_RT_REG_DWORD, nullptr, &destination, &bytes) == ERROR_SUCCESS &&
           destination == 1;
}

bool HasWirelessInterface() {
    MIB_IF_TABLE2* table{};
    if (GetIfTable2(&table) != NO_ERROR) return true;
    const bool found = std::any_of(table->Table, table->Table + table->NumEntries,
                                  [](auto const& entry) { return entry.Type == IF_TYPE_IEEE80211; });
    FreeMibTable(table);
    return found;
}

bool IsHomeEdition() {
    DWORD edition{};
    if (!GetProductInfo(10, 0, 0, 0, &edition)) return false;
    return edition == PRODUCT_CORE || edition == PRODUCT_CORE_N ||
           edition == PRODUCT_CORE_SINGLELANGUAGE || edition == PRODUCT_CORE_COUNTRYSPECIFIC;
}

std::optional<Destination> Resolve(CatalogItem const& item, StockMenu const& stock) {
    auto [kind, target] = Split(item.action, L':');
    Destination destination{&item};
    if (kind == L"native" || kind == L"power") {
        destination.original = kind == L"native" ? StockAction(stock, target) : StockPowerAction(stock, target);
        if (!destination.original || !destination.original.IsEnabled()) return std::nullopt;
    } else if (kind == L"exe") {
        const auto [name, args] = Split(target, L';');
        if (!Exists(ProgramPath(name)) || (args.ends_with(L".msc") && !Exists(SystemPath(args))))
            return std::nullopt;
        if ((args == L"gpedit.msc" || args == L"secpol.msc" || args == L"lusrmgr.msc") && IsHomeEdition())
            return std::nullopt;
    } else if (kind == L"control") {
        if (!ControlRegistered(item.resource) || (target == L"Microsoft.BitLockerDriveEncryption" && IsHomeEdition()))
            return std::nullopt;
    } else if (kind == L"folder") {
        if ((std::wstring_view(item.key) == L"commands.submenus.explorer.computer" && ExplorerStartsOnThisPc()) ||
            !FolderId(target)) return std::nullopt;
    } else if (kind == L"uri") {
        if (target == L"network-wifi" && !HasWirelessInterface()) return std::nullopt;
        if (target == L"devices-keyboard" && SystemCaption(item).empty()) return std::nullopt;
    }
    return destination;
}

void RemoveRowVisual(RowInteraction& row) {
    if (row.hoverUpdate) row.hoverUpdate.Stop();
    if (row.arrowLayer && row.layout) {
        uint32_t index{};
        auto children = row.layout.Children();
        if (children.IndexOf(row.arrowLayer, index)) children.RemoveAt(index);
    }
}

void Restore() noexcept {
    auto old = std::move(session);
    if (!old) return;
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

void TryPatchMenu(C::MenuFlyout const& menu) {
    const auto items = menu.Items();
    if (items.Size() < 10) return;
    const auto snapshot = ReadSettings();

    StockMenu stock;
    const auto powerTitle = Normalize(Twinui(L"10930"));
    if (powerTitle.empty()) return;
    for (auto const& item : items) {
        stock.rows.push_back(item);
        if (auto sub = item.try_as<C::MenuFlyoutSubItem>(); sub && Normalize(sub.Text()) == powerTitle)
            stock.power = sub;
    }
    if (!stock.power || !StockAction(stock, L"10921") || !StockAction(stock, L"10923") ||
        !StockAction(stock, L"10925")) return;
    for (auto const& item : stock.power.Items()) {
        if (item.try_as<C::MenuFlyoutItem>()) stock.powerRows.push_back(item);
    }
    if (stock.powerRows.size() < 3 || stock.powerRows.size() > 5) return;

    auto pending = std::make_unique<MenuSession>();
    pending->menu = menu;
    pending->original = stock.rows;
    pending->power = stock.power;
    for (auto const& item : stock.power.Items()) pending->originalPower.push_back(item);
    pending->oldPowerIcon = stock.power.ReadLocalValue(C::MenuFlyoutSubItem::IconProperty());

    std::vector<C::MenuFlyoutItemBase> visible;
    std::vector<C::MenuFlyoutItemBase> powerChildren;
    wchar_t powerGlyph{};
    int previousGroup = -1;
    for (size_t index = 0; index < std::size(catalog); ++index) {
        const auto& item = catalog[index];
        if (item.parent >= 0 || !snapshot.enabled[index]) continue;
        const bool power = std::wstring_view(item.action) == L"power:menu";
        auto destination = power ? std::optional<Destination>{} : Resolve(item, stock);
        if (!power && !destination) continue;

        std::vector<C::MenuFlyoutItem> children;
        for (size_t child = index + 1; child < std::size(catalog) && catalog[child].parent == static_cast<int>(index); ++child) {
            if (!snapshot.enabled[child]) continue;
            if (const auto resolved = Resolve(catalog[child], stock)) {
                if (auto leaf = CreateLeaf(*pending, *resolved)) children.push_back(leaf);
            }
        }

        C::MenuFlyoutItemBase entry{nullptr};
        if (power) {
            if (children.empty()) continue;
            powerChildren.assign(children.begin(), children.end());
            powerGlyph = item.glyph;
            AddInteraction(*pending, stock.power, {}, snapshot.mode);
            entry = stock.power;
        } else if (children.empty()) {
            entry = CreateLeaf(*pending, *destination);
        } else {
            const auto caption = Caption(*destination);
            if (caption.empty()) continue;
            C::MenuFlyoutSubItem split;
            split.Text(caption);
            if (item.glyph) split.Icon(MakeIcon(item.glyph));
            for (auto const& child : children) split.Items().Append(child);
            AddInteraction(*pending, split, *destination, snapshot.mode);
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
        for (auto const& row : owner->rows)
            OnRow(row.get(), AttachPointerEvents);
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

template<typename Interface>
void Inspect(void* abi) {
    if (unloading || GetCurrentThreadId() != trayThread || session) return;
    try {
        Interface flyout{nullptr};
        winrt::copy_from_abi(flyout, abi);
        if (auto menu = flyout.template try_as<C::MenuFlyout>()) TryPatchMenu(menu);
    } catch (...) {
        Restore();
        Wh_Log(L"Could not customize the Quick Link menu");
    }
}

HRESULT WINAPI HookShowAt(void* self, void* target) {
    Inspect<P::IFlyoutBase>(self);
    const auto result = originalShowAt(self, target);
    if (FAILED(result) && GetCurrentThreadId() == trayThread) Restore();
    return result;
}

HRESULT WINAPI HookShowAtOptions(void* self, void* target, void* options) {
    Inspect<P::IFlyoutBase5>(self);
    const auto result = originalShowAtOptions(self, target, options);
    if (FAILED(result) && GetCurrentThreadId() == trayThread) Restore();
    return result;
}

bool InstallHooks() {
    C::MenuFlyout sample;
    const auto base = sample.as<P::IFlyoutBase>();
    const auto options = sample.as<P::IFlyoutBase5>();
    // The slots are the two ShowAt signatures in the Windows Runtime interfaces.
    void* show = (*reinterpret_cast<void***>(winrt::get_abi(base)))[14];
    void* showOptions = (*reinterpret_cast<void***>(winrt::get_abi(options)))[12];
    return show != showOptions &&
        Wh_SetFunctionHook(show, reinterpret_cast<void*>(HookShowAt), reinterpret_cast<void**>(&originalShowAt)) &&
        Wh_SetFunctionHook(showOptions, reinterpret_cast<void*>(HookShowAtOptions),
                           reinterpret_cast<void**>(&originalShowAtOptions)) &&
        Wh_ApplyHookOperations();
}

LRESULT CALLBACK TraySubclass(HWND window, UINT message, WPARAM action, LPARAM data, DWORD_PTR) {
    if (message == setupMessage) {
        try {
            if (action) return InstallHooks();
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

}  // namespace

BOOL Wh_ModInit() {
    trayWindow = FindWindowW(L"Shell_TrayWnd", nullptr);
    DWORD pid = 0;
    trayThread = GetWindowThreadProcessId(trayWindow, &pid);
    if (!trayThread || pid != GetCurrentProcessId()) return FALSE;
    setupMessage = RegisterWindowMessageW(L"Windhawk.QuickLinkMenu.Setup");
    LoadSettings();
    return setupMessage != 0;
}

void Wh_ModAfterInit() {
    subclassPresent = WindhawkUtils::SetWindowSubclassFromAnyThread(trayWindow, TraySubclass, 0);
    if (!subclassPresent || !SendMessageW(trayWindow, setupMessage, 1, 0))
        Wh_Log(L"Could not initialize the Quick Link menu hooks");
}

void Wh_ModSettingsChanged() {
    LoadSettings();
}

void Wh_ModBeforeUninit() {
    unloading = true;
    if (subclassPresent && IsWindow(trayWindow)) {
        SendMessageW(trayWindow, setupMessage, 0, 0);
        WindhawkUtils::RemoveWindowSubclassFromAnyThread(trayWindow, TraySubclass);
    }
}

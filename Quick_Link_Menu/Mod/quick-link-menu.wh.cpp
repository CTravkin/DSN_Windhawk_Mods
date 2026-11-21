// ==WindhawkMod==
// @id              quick-link-menu
// @name            Quick Link Menu
// @description     Customize and extend the Win+X menu (right-click the Start button)
// @description:ru-RU Настройка и расширение меню Win+X (правый клик по кнопке «Пуск»)
// @description:es-ES Personaliza y amplía el menú Win+X (clic derecho en el botón Inicio)
// @description:de-DE Das Win+X-Menü anpassen und erweitern (Rechtsklick auf die Startschaltfläche)
// @description:pt-BR Personalize e amplie o menu Win+X (clique direito no botão Iniciar)
// @description:zh-CN 自定义并扩展 Win+X 菜单（右键单击“开始”按钮）
// @version         0.1.4
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         explorer.exe
// @architecture    x86-64
// @compilerOptions -lcomctl32 -lruntimeobject -lole32 -loleaut32 -lshell32 -luuid
// ==/WindhawkMod==
// Build: 5

// ==WindhawkModReadme==
/*
# Quick Link Menu

This prototype adds a native submenu to the Terminal row in the Windows 11
Win+X menu. Click the row or press Enter to start Terminal. Use the arrow or
press Right to open its submenu.

The **Open submenus on hover** setting controls whether hovering over the row
opens the submenu immediately, uses the Windows delay, or opens immediately
only over the arrow. The arrow has its own hover background in the last mode.

The prototype recognizes the English and Russian Windows menu. It is
incompatible with Windows 10 Win+X Restorer.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- submenuHover: immediate
  $name: Open submenus on hover
  $name:ru-RU: Раскрытие подменю при наведении
  $name:es-ES: Apertura de submenús al pasar el puntero
  $name:de-DE: Untermenüs beim Darüberfahren öffnen
  $name:pt-BR: Abrir submenus ao passar o ponteiro
  $name:zh-CN: 悬停时打开子菜单
  $options:
  - immediate: Immediately
  - delayed: With the Windows delay
  - arrow: When hovering over the arrow
  $options:ru-RU:
  - immediate: Сразу
  - delayed: С задержкой Windows
  - arrow: При наведении на стрелку
  $options:es-ES:
  - immediate: Inmediatamente
  - delayed: Con el retardo de Windows
  - arrow: Al pasar el puntero sobre la flecha
  $options:de-DE:
  - immediate: Sofort
  - delayed: Mit Windows-Verzögerung
  - arrow: Beim Darüberfahren über den Pfeil
  $options:pt-BR:
  - immediate: Imediatamente
  - delayed: Com o atraso do Windows
  - arrow: Ao passar o ponteiro sobre a seta
  $options:zh-CN:
  - immediate: 立即打开
  - delayed: 使用 Windows 延迟
  - arrow: 悬停在箭头上时打开
*/
// ==/WindhawkModSettings==

#include <shellapi.h>
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
#include <atomic>
#include <chrono>
#include <cmath>
#include <memory>
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

enum class HoverMode { Immediate, Delayed, Arrow };
enum class HoverZone { Outside, Body, Arrow };

struct MenuSession {
    C::MenuFlyout menu{nullptr};
    C::MenuFlyoutItem terminal{nullptr};
    C::MenuFlyoutItem elevated{nullptr};
    C::MenuFlyoutSubItem split{nullptr};
    uint32_t elevatedIndex{};
    bool elevatedRemoved{};
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

    std::vector<C::MenuFlyoutItem> decoratedItems;
    P::FlyoutBase::Opened_revoker opened;
    P::FlyoutBase::Closed_revoker closed;
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

[[clang::no_destroy]] std::unique_ptr<MenuSession> session;
std::atomic<HoverMode> hoverMode{HoverMode::Immediate};
std::atomic<bool> unloading;
HWND trayWindow;
DWORD trayThread;
UINT setupMessage;
bool subclassPresent;

using ShowAt = HRESULT(WINAPI*)(void*, void*);
using ShowAtOptions = HRESULT(WINAPI*)(void*, void*, void*);
ShowAt originalShowAt;
ShowAtOptions originalShowAtOptions;

template<typename Function>
void OnActiveMenu(Function&& function) noexcept {
    if (unloading || !session) return;
    try {
        function(*session);
    } catch (...) {
        Wh_Log(L"Menu event failed");
    }
}

void LoadSettings() {
    const auto value = Wh_GetStringSetting(L"submenuHover");
    const std::wstring_view option = value ? value : L"";
    hoverMode = option == L"delayed" ? HoverMode::Delayed :
                option == L"arrow" ? HoverMode::Arrow : HoverMode::Immediate;
    Wh_FreeStringSetting(value);
}

std::wstring Normalize(winrt::hstring const& text) {
    std::wstring normalized;
    for (wchar_t ch : text) {
        if (ch != L'&' && ch != L'\u200e' && ch != L'\u200f')
            normalized.push_back(ch);
    }
    CharLowerBuffW(normalized.data(), static_cast<DWORD>(normalized.size()));
    return normalized;
}

bool IsTerminal(std::wstring_view name) {
    return name == L"терминал" || name == L"terminal" || name == L"windows terminal";
}

bool IsElevatedTerminal(std::wstring_view name) {
    return name == L"терминал (администратор)" ||
           name == L"терминал (от имени администратора)" ||
           name == L"terminal (admin)" || name == L"windows terminal (admin)";
}

void Restore() noexcept {
    auto old = std::move(session);
    if (!old) return;
    try {
        if (old->hoverUpdate) old->hoverUpdate.Stop();
        auto items = old->menu.Items();
        uint32_t index;
        if (items.IndexOf(old->split, index)) items.SetAt(index, old->terminal);
        if (old->elevatedRemoved) {
            auto children = old->split.Items();
            if (children.IndexOf(old->elevated, index)) children.RemoveAt(index);
            items.InsertAt(old->elevatedIndex, old->elevated);
        }
        for (const auto& item : old->decoratedItems)
            item.ClearValue(C::MenuFlyoutItem::IconProperty());
    } catch (...) {
        Wh_Log(L"Could not restore the original Quick Link menu items");
    }
}

template<typename Provider>
Provider GetProvider(X::UIElement const& item, AP::PatternInterface pattern) {
    auto peer = AP::FrameworkElementAutomationPeer::CreatePeerForElement(item);
    return peer ? peer.GetPattern(pattern).try_as<Provider>() : nullptr;
}

void RunTerminal(MenuSession const& active) {
    auto menu = active.menu;
    auto invoke = GetProvider<AV::IInvokeProvider>(active.terminal, AP::PatternInterface::Invoke);
    if (invoke) {
        invoke.Invoke();
    } else if (reinterpret_cast<INT_PTR>(ShellExecuteW(
                   nullptr, L"open", L"wt.exe", nullptr, nullptr, SW_SHOWNORMAL)) <= 32) {
        Wh_Log(L"Could not start Terminal");
    }
    menu.Hide();
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

winrt::Windows::Foundation::Point ChevronCenter(MenuSession const& active, X::UIElement const& relativeTo) {
    return active.chevron.TransformToVisual(relativeTo).TransformPoint({
        static_cast<float>(active.chevron.ActualWidth() / 2.0),
        static_cast<float>(active.chevron.ActualHeight() / 2.0),
    });
}

HoverZone PointerZone(MenuSession const& active, I::PointerRoutedEventArgs const& args) {
    const auto x = args.GetCurrentPoint(active.layout).Position().X;
    return std::abs(x - ChevronCenter(active, active.layout).X) <= arrowHitWidth / 2.0 ?
        HoverZone::Arrow : HoverZone::Body;
}

void UpdateHoverVisual(MenuSession const& active) {
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

void SetHoverZone(MenuSession& active, HoverZone zone) {
    if (std::exchange(active.zone, zone) == zone) return;
    active.openOnUpdate = active.expand &&
        ((active.mode == HoverMode::Immediate && zone != HoverZone::Outside) ||
         (active.mode == HoverMode::Arrow && zone == HoverZone::Arrow));
    UpdateHoverVisual(active);
    if (active.openOnUpdate) active.hoverUpdate.Start();
}

void AddArrowBackground(MenuSession& active) {
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
        [](auto const&, X::VisualStateChangedEventArgs const& args) {
            OnActiveMenu([&](auto& active) {
                if (active.zone == HoverZone::Arrow && args.NewState() &&
                    args.NewState().Name() != L"Normal")
                    active.hoverUpdate.Start();
            });
        });
}

void AttachPointerEvents(MenuSession& active) {
    active.split.ApplyTemplate();
    active.layout = FindNamed(active.split, L"LayoutRoot").try_as<C::Grid>();
    active.chevron = FindNamed(active.split, L"SubItemChevron");
    if (!active.layout || !active.chevron) return;
    active.expand = GetProvider<AV::IExpandCollapseProvider>(active.split, AP::PatternInterface::ExpandCollapse);
    if (active.mode != HoverMode::Delayed && active.expand) {
        // Let XAML finish routing the enter event before opening its submenu.
        active.hoverUpdate = X::DispatcherTimer();
        active.hoverUpdate.Interval(std::chrono::milliseconds(1));
        active.tick = active.hoverUpdate.Tick(winrt::auto_revoke, [](auto const&, auto const&) {
            OnActiveMenu([](auto& active) {
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
        [](auto const&, I::PointerRoutedEventArgs const& args) {
            OnActiveMenu([&](auto& active) {
                if (args.Pointer().PointerDeviceType() == PointerType::Touch) return;
                // In arrow-only mode we drive the native PointerOver state explicitly
                // because the XAML handler pairs it with starting the submenu timer.
                if (active.mode == HoverMode::Arrow && active.expand) args.Handled(true);
                SetHoverZone(active, PointerZone(active, args));
            });
        });
    active.pointerMoved = active.layout.PointerMoved(winrt::auto_revoke,
        [](auto const&, I::PointerRoutedEventArgs const& args) {
            OnActiveMenu([&](auto& active) {
                if (args.Pointer().PointerDeviceType() != PointerType::Touch)
                    SetHoverZone(active, PointerZone(active, args));
            });
        });
    active.pointerExited = active.layout.PointerExited(winrt::auto_revoke,
        [](auto const&, auto const&) {
            OnActiveMenu([](auto& active) {
                active.pressedPointer.reset();
                SetHoverZone(active, HoverZone::Outside);
            });
        });
    active.pointerPressed = active.layout.PointerPressed(winrt::auto_revoke,
        [](auto const&, I::PointerRoutedEventArgs const& args) {
            OnActiveMenu([&](auto& active) {
                if (PointerZone(active, args) == HoverZone::Body &&
                    (args.Pointer().PointerDeviceType() == PointerType::Touch ||
                     args.GetCurrentPoint(active.layout).Properties().IsLeftButtonPressed())) {
                    active.pressedPointer = args.Pointer().PointerId();
                    args.Handled(true);
                }
            });
        });
    active.pointerReleased = active.layout.PointerReleased(winrt::auto_revoke,
        [](auto const&, I::PointerRoutedEventArgs const& args) {
            OnActiveMenu([&](auto& active) {
                const bool pressedHere = std::exchange(active.pressedPointer, std::nullopt) == args.Pointer().PointerId();
                const bool body = PointerZone(active, args) == HoverZone::Body;
                if (pressedHere || body) args.Handled(true);
                if (pressedHere && body) RunTerminal(active);
            });
        });
    active.pointerCanceled = active.layout.PointerCanceled(winrt::auto_revoke,
        [](auto const&, auto const&) {
            OnActiveMenu([](auto& active) { active.pressedPointer.reset(); });
        });
}

C::FontIcon MakeIcon(PCWSTR glyph) {
    C::FontIcon icon;
    icon.FontFamily(X::Media::FontFamily(L"Segoe Fluent Icons"));
    icon.FontSize(16);
    icon.Glyph(glyph);
    return icon;
}

void AddPrototypeIcons(MenuSession& active) {
    for (const auto& item : active.menu.Items()) {
        const auto command = item.try_as<C::MenuFlyoutItem>();
        if (!command || command.Icon() ||
            command.ReadLocalValue(C::MenuFlyoutItem::IconProperty()) != X::DependencyProperty::UnsetValue())
            continue;
        const auto name = Normalize(command.Text());
        PCWSTR glyph = nullptr;
        if (name == L"параметры" || name == L"settings") glyph = L"\ue713";
        if (name == L"проводник" || name == L"file explorer") glyph = L"\ue8b7";
        if (glyph) {
            active.decoratedItems.push_back(command);
            command.Icon(MakeIcon(glyph));
        }
    }
}

void TryPatchMenu(C::MenuFlyout const& menu) {
    auto items = menu.Items();
    if (items.Size() < 12) return;

    C::MenuFlyoutItem terminal{nullptr};
    C::MenuFlyoutItem elevated{nullptr};
    uint32_t index = 0;
    uint32_t elevatedIndex = 0;
    bool power = false;
    for (uint32_t i = 0; i < items.Size(); ++i) {
        const auto item = items.GetAt(i);
        if (const auto sub = item.try_as<C::MenuFlyoutSubItem>()) {
            const auto name = Normalize(sub.Text());
            power |= name == L"shut down or sign out" ||
                     name == L"завершение работы или выход из системы";
        }
        if (const auto command = item.try_as<C::MenuFlyoutItem>()) {
            const auto name = Normalize(command.Text());
            if (IsTerminal(name)) {
                if (terminal) return;
                terminal = command;
                index = i;
            }
            if (IsElevatedTerminal(name)) {
                if (elevated) return;
                elevated = command;
                elevatedIndex = i;
            }
        }
    }
    if (!power || !terminal || !elevated || elevatedIndex != index + 1) return;

    auto pending = std::make_unique<MenuSession>();
    pending->menu = menu;
    pending->terminal = terminal;
    pending->elevated = elevated;
    pending->elevatedIndex = elevatedIndex;
    pending->mode = hoverMode.load();
    pending->split = C::MenuFlyoutSubItem();
    pending->split.Text(terminal.Text());
    pending->split.IsEnabled(terminal.IsEnabled());
    pending->split.Icon(MakeIcon(L"\ue756"));
    pending->keyDown = pending->split.PreviewKeyDown(winrt::auto_revoke,
        [](auto const&, I::KeyRoutedEventArgs const& args) {
            if (args.Key() != Key::Enter) return;
            OnActiveMenu([&](auto& active) {
                args.Handled(true);
                RunTerminal(active);
            });
        });
    pending->opened = menu.Opened(winrt::auto_revoke, [](auto const&, auto const&) {
        OnActiveMenu(AttachPointerEvents);
    });
    pending->closed = menu.Closed(winrt::auto_revoke, [](auto const&, auto const&) { Restore(); });

    session = std::move(pending);
    items.RemoveAt(elevatedIndex);
    session->elevatedRemoved = true;
    session->split.Items().Append(elevated);
    items.SetAt(index, session->split);
    AddPrototypeIcons(*session);
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

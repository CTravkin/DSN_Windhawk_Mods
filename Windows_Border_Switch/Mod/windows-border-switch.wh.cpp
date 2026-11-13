// ==WindhawkMod==
// @id              windows-border-switch
// @name            Windows Border Switch
// @description     Control borders of windows, dialogs, Start, and the taskbar
// @description:ru-RU Управление рамками окон, диалогов, меню «Пуск» и панели задач
// @description:es-ES Controla los bordes de las ventanas, los diálogos, Inicio y la barra de tareas
// @description:de-DE Steuert die Rahmen von Fenstern, Dialogen, Startmenü und Taskleiste
// @description:pt-BR Controla as bordas das janelas, das caixas de diálogo, do menu Iniciar e da barra de tarefas
// @description:zh-CN 控制窗口、对话框、开始菜单和任务栏的边框
// @version         0.1.1
// @author          CTravkin
// @github          https://github.com/CTravkin
// @homepage        https://ctravkin.ru
// @license         MIT
// @include         explorer.exe
// @include         StartMenuExperienceHost.exe
// @architecture    x86-64
// @compilerOptions -ldwmapi -lole32 -loleaut32 -lruntimeobject -luuid -ladvapi32
// ==/WindhawkMod==
// Build: 2

// ==WindhawkModReadme==
/*
# Windows Border Switch

**English** · [Русский](https://github.com/CTravkin/DSN_Windhawk_Mods/blob/main/Windows_Border_Switch/README.ru.md)

Control the borders of active windows, inactive windows, Win32 dialogs, Start,
and the taskbar independently. The Start setting also applies to its Phone Link
panel. The active-window setting can also remove the system-drawn title bar
accent while keeping the window border.

By default, the mod removes the active title bar accent and hides the borders
of inactive windows, dialogs, Start, and the taskbar. Window settings affect
system-drawn frames. Applications can draw their own frames and title bars.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- activeWindow: caption
  $name: Active window border
  $name:ru-RU: Рамка активного окна
  $name:es-ES: Borde de la ventana activa
  $name:de-DE: Rahmen des aktiven Fensters
  $name:pt-BR: Borda da janela ativa
  $name:zh-CN: 活动窗口边框
  $options:
  - default: Default
  - hide: Hide border
  - caption: Disable title bar accent
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  - caption: Отключить акцент заголовка
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  - caption: Desactivar el color de énfasis de la barra de título
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  - caption: Akzentfarbe der Titelleiste deaktivieren
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  - caption: Desativar a cor de destaque da barra de título
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
  - caption: 关闭标题栏强调色
- inactiveWindow: hide
  $name: Inactive window borders
  $name:ru-RU: Рамки неактивных окон
  $name:es-ES: Bordes de las ventanas inactivas
  $name:de-DE: Rahmen inaktiver Fenster
  $name:pt-BR: Bordas das janelas inativas
  $name:zh-CN: 非活动窗口边框
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
- dialog: hide
  $name: Dialog borders
  $name:ru-RU: Рамки диалоговых окон
  $name:es-ES: Bordes de los cuadros de diálogo
  $name:de-DE: Dialograhmen
  $name:pt-BR: Bordas das caixas de diálogo
  $name:zh-CN: 对话框边框
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
- startMenu: hide
  $name: Start menu border
  $name:ru-RU: Рамка меню «Пуск»
  $name:es-ES: Borde del menú Inicio
  $name:de-DE: Startmenürahmen
  $name:pt-BR: Borda do menu Iniciar
  $name:zh-CN: 开始菜单边框
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
- taskbar: hide
  $name: Taskbar border
  $name:ru-RU: Рамка панели задач
  $name:es-ES: Borde de la barra de tareas
  $name:de-DE: Taskleistenrahmen
  $name:pt-BR: Borda da barra de tarefas
  $name:zh-CN: 任务栏边框
  $options:
  - default: Default
  - hide: Hide border
  $options:ru-RU:
  - default: По умолчанию
  - hide: Скрыть рамку
  $options:es-ES:
  - default: Predeterminado
  - hide: Ocultar borde
  $options:de-DE:
  - default: Standard
  - hide: Rahmen ausblenden
  $options:pt-BR:
  - default: Padrão
  - hide: Ocultar borda
  $options:zh-CN:
  - default: 默认
  - hide: 隐藏边框
*/
// ==/WindhawkModSettings==

#include <string_view>

namespace {

enum class WindowEffect { Default, HideBorder, RemoveCaptionAccent };

struct WindowSettings {
    WindowEffect active = WindowEffect::Default;
    bool hideInactive = false;
    bool hideDialog = false;

    bool AnyEnabled() const {
        return active != WindowEffect::Default || hideInactive || hideDialog;
    }
};

constexpr WindowEffect WindowEffectFor(WindowSettings settings, bool active, bool dialog) {
    if (dialog) {
        return settings.hideDialog ? WindowEffect::HideBorder : WindowEffect::Default;
    }
    if (active) {
        return settings.active;
    }
    return settings.hideInactive ? WindowEffect::HideBorder : WindowEffect::Default;
}

enum class ShellBorder { None, Start, Taskbar };

constexpr ShellBorder ShellBorderFor(bool startHost,
                                     std::wstring_view type,
                                     std::wstring_view name) {
    if (startHost && name == L"AcrylicBorder" && type == L"Windows.UI.Xaml.Controls.Border") {
        return ShellBorder::Start;
    }
    if (!startHost && name == L"BackgroundStroke" && type == L"Windows.UI.Xaml.Shapes.Rectangle") {
        return ShellBorder::Taskbar;
    }
    return ShellBorder::None;
}

constexpr bool IsStartPane(std::wstring_view name) {
    return name == L"MainMenu" || name == L"CompanionRoot";
}

}  // namespace

#ifndef BORDER_SWITCH_POLICY_TEST
#define WIN32_LEAN_AND_MEAN
#include <windows.h>
#include <dwmapi.h>
#include <ocidl.h>
#include <xamlom.h>
#include <windhawk_api.h>

#undef GetCurrentTime
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.System.h>
#include <winrt/Windows.UI.Core.h>
#include <winrt/Windows.UI.Xaml.Controls.h>
#include <winrt/Windows.UI.Xaml.Media.h>

#include <algorithm>
#include <array>
#include <atomic>
#include <mutex>
#include <unordered_map>
#include <utility>
#include <vector>

namespace {

namespace Foundation = winrt::Windows::Foundation;
namespace Xaml = winrt::Windows::UI::Xaml;
namespace Core = winrt::Windows::UI::Core;
namespace System = winrt::Windows::System;

bool g_startHost;

struct Worker {
    HANDLE thread = nullptr;
    HANDLE stop = nullptr;

    void Start(LPTHREAD_START_ROUTINE run) {
        stop = CreateEventW(nullptr, TRUE, FALSE, nullptr);
        if (!stop) {
            return;
        }
        thread = CreateThread(nullptr, 0, run, stop, 0, nullptr);
        if (!thread) {
            CloseHandle(std::exchange(stop, nullptr));
        }
    }

    void Stop() {
        if (!thread) {
            return;
        }
        SetEvent(stop);
        WaitForSingleObject(thread, INFINITE);
        CloseHandle(std::exchange(thread, nullptr));
        CloseHandle(std::exchange(stop, nullptr));
    }
};

WindowSettings g_windowSettings;
Worker g_windowWorker;
std::unordered_map<HWND, WindowEffect> g_changedWindows;
HWND g_foreground;
bool g_ownsDesktop;

bool ClassIs(HWND window, std::wstring_view name) {
    WCHAR buffer[96];
    int length = GetClassNameW(window, buffer, ARRAYSIZE(buffer));
    return std::wstring_view(buffer, length) == name;
}

bool IsMainExplorer() {
    DWORD pid = 0;
    GetWindowThreadProcessId(GetShellWindow(), &pid);
    return pid == GetCurrentProcessId();
}

bool CaptionAccentEnabled() {
    DWORD value = 0;
    DWORD size = sizeof(value);
    return RegGetValueW(HKEY_CURRENT_USER, L"Software\\Microsoft\\Windows\\DWM",
                        L"ColorPrevalence", RRF_RT_REG_DWORD, nullptr, &value,
                        &size) == ERROR_SUCCESS &&
           value != 0;
}

bool SetWindowEffect(HWND window, WindowEffect effect, COLORREF color) {
    DWORD attribute = effect == WindowEffect::HideBorder ? DWMWA_BORDER_COLOR
                                                         : DWMWA_CAPTION_COLOR;
    HRESULT result = DwmSetWindowAttribute(window, attribute, &color, sizeof(color));
    if (SUCCEEDED(result)) {
        Wh_Log(L"DWM window %p attribute %lu = %08lX", window, attribute, color);
    }
    return SUCCEEDED(result);
}

void ApplyWindowBorder(HWND window) {
    if (!window || GetAncestor(window, GA_ROOT) != window) {
        return;
    }

    auto changed = g_changedWindows.find(window);
    bool visible = IsWindowVisible(window);
    if (!visible && changed == g_changedWindows.end()) {
        return;
    }
    auto style = static_cast<DWORD>(GetWindowLongPtrW(window, GWL_STYLE));
    bool caption = (style & WS_CAPTION) == WS_CAPTION;
    bool dialog = ClassIs(window, L"#32770");
    bool decorated = !(style & WS_CHILD) && (dialog || caption || (style & WS_THICKFRAME));
    WindowEffect effect = WindowEffect::Default;
    if (visible && decorated) {
        effect = WindowEffectFor(g_windowSettings, window == g_foreground, dialog);
    }
    if (effect == WindowEffect::RemoveCaptionAccent && (!caption || !CaptionAccentEnabled())) {
        effect = WindowEffect::Default;
    }

    if (changed != g_changedWindows.end()) {
        if (changed->second == effect) {
            return;
        }
        if (!SetWindowEffect(window, changed->second, DWMWA_COLOR_DEFAULT)) {
            return;
        }
        g_changedWindows.erase(changed);
    }
    if (effect != WindowEffect::Default && SetWindowEffect(window, effect, DWMWA_COLOR_NONE)) {
        g_changedWindows.emplace(window, effect);
    }
}

void RefreshWindows() {
    g_foreground = GetForegroundWindow();
    EnumWindows([](HWND window, LPARAM) -> BOOL {
        ApplyWindowBorder(window);
        return TRUE;
    }, 0);
}

void CALLBACK OnWindowEvent(HWINEVENTHOOK, DWORD event, HWND window, LONG object, LONG child,
                            DWORD, DWORD) {
    if (object != OBJID_WINDOW || child != CHILDID_SELF) {
        return;
    }
    if (!g_ownsDesktop) {
        if (IsMainExplorer()) {
            g_ownsDesktop = true;
            RefreshWindows();
        }
        return;
    }
    if (event == EVENT_OBJECT_DESTROY) {
        g_changedWindows.erase(window);
    } else if (event == EVENT_OBJECT_SHOW || event == EVENT_OBJECT_HIDE) {
        ApplyWindowBorder(window);
    } else if (event == EVENT_SYSTEM_FOREGROUND) {
        HWND previous = std::exchange(g_foreground, GetForegroundWindow());
        if (previous != g_foreground) {
            ApplyWindowBorder(previous);
        }
        ApplyWindowBorder(g_foreground);
    }
}

DWORD WINAPI RunWindowWorker(void* stopParameter) {
    HANDLE stop = static_cast<HANDLE>(stopParameter);
    const std::array hooks = {
        SetWinEventHook(EVENT_SYSTEM_FOREGROUND, EVENT_SYSTEM_FOREGROUND, nullptr,
                        OnWindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT),
        SetWinEventHook(EVENT_OBJECT_DESTROY, EVENT_OBJECT_HIDE, nullptr,
                        OnWindowEvent, 0, 0, WINEVENT_OUTOFCONTEXT),
    };

    if (hooks[0] && hooks[1]) {
        g_ownsDesktop = IsMainExplorer();
        if (g_ownsDesktop) {
            RefreshWindows();
        }
        while (MsgWaitForMultipleObjects(1, &stop, FALSE, INFINITE, QS_ALLINPUT) ==
               WAIT_OBJECT_0 + 1) {
            MSG message;
            while (WaitForSingleObject(stop, 0) == WAIT_TIMEOUT &&
                   PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
                TranslateMessage(&message);
                DispatchMessageW(&message);
            }
        }
    } else {
        Wh_Log(L"Unable to subscribe to window events");
    }

    for (HWINEVENTHOOK hook : hooks) {
        if (hook) {
            UnhookWinEvent(hook);
        }
    }
    for (auto [window, effect] : g_changedWindows) {
        SetWindowEffect(window, effect, DWMWA_COLOR_DEFAULT);
    }
    g_changedWindows.clear();
    return 0;
}

constexpr CLSID kTapClass = {0x50127d3e, 0xe980, 0x47b2,
                            {0xa4, 0xde, 0x33, 0xa0, 0x27, 0x85, 0xad, 0x67}};
constexpr GUID kDiagnosticsTestHooks = {0x735941a2, 0x3ee3, 0x495a,
                                        {0x8d, 0xa9, 0x97, 0x26, 0x27, 0x00, 0x30, 0x75}};

struct DiagnosticsTestHooks : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE UnregisterInstance(InstanceHandle handle) = 0;
};

HMODULE OwnModule() {
    HMODULE module = nullptr;
    GetModuleHandleExW(GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS |
                           GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT,
                       reinterpret_cast<PCWSTR>(&OwnModule), &module);
    return module;
}

using XamlInitialize = decltype(&InitializeXamlDiagnosticsEx);
using OpenKey = decltype(&RegOpenKeyExW);
using QueryValue = decltype(&RegQueryValueExW);

OpenKey g_openKey;
QueryValue g_queryValue;
thread_local HKEY g_compositionDebugKey;
thread_local bool g_advising;

LSTATUS WINAPI OpenKeyHook(HKEY key, PCWSTR name, DWORD options, REGSAM access, HKEY* result) {
    LSTATUS status = g_openKey(key, name, options, access, result);
    if (g_advising && key == HKEY_LOCAL_MACHINE && name && result &&
        _wcsicmp(name, L"Software\\Microsoft\\XAML\\Debug") == 0) {
        if (status == ERROR_FILE_NOT_FOUND) {
            status = g_openKey(key, L"Software\\Microsoft", options, access, result);
        }
        if (status == ERROR_SUCCESS) {
            g_compositionDebugKey = *result;
        }
    }
    return status;
}

LSTATUS WINAPI QueryValueHook(HKEY key, PCWSTR name, DWORD* reserved, DWORD* type,
                             BYTE* data, DWORD* size) {
    // Element callbacks do not need the process-wide composition diagnostics.
    if (!g_advising || !key || key != g_compositionDebugKey || !name ||
        _wcsicmp(name, L"DisableCompositionDiag") != 0 || !size) {
        return g_queryValue(key, name, reserved, type, data, size);
    }
    if (type) {
        *type = REG_DWORD;
    }
    if (data && *size < sizeof(DWORD)) {
        *size = sizeof(DWORD);
        return ERROR_MORE_DATA;
    }
    if (data) {
        DWORD disabled = 1;
        memcpy(data, &disabled, sizeof(disabled));
    }
    *size = sizeof(DWORD);
    return ERROR_SUCCESS;
}

struct ShellEdit {
    winrt::weak_ref<Xaml::FrameworkElement> element;
    Core::CoreDispatcher dispatcher;
    ShellBorder border;
    Foundation::IInspectable original;

    Xaml::DependencyProperty Property() const {
        return border == ShellBorder::Start ? Xaml::Controls::Border::BorderThicknessProperty()
                                           : Xaml::UIElement::VisibilityProperty();
    }

    bool Restore() const {
        auto target = element.get();
        if (!target) {
            return false;
        }
        auto property = Property();
        if (original) {
            target.SetValue(property, original);
        } else {
            target.ClearValue(property);
        }
        return true;
    }
};

struct TreeWatcher : winrt::implements<TreeWatcher, IVisualTreeServiceCallback2> {
    explicit TreeWatcher(winrt::com_ptr<IXamlDiagnostics> diagnostics)
        : m_diagnostics(std::move(diagnostics)) {
        m_diagnostics->QueryInterface(kDiagnosticsTestHooks, m_testHooks.put_void());
    }

    HRESULT Advise() {
        // Without UnregisterInstance, visiting an active shell can retain its XAML objects.
        if (!m_testHooks) {
            return E_NOINTERFACE;
        }
        auto service = m_diagnostics.as<IVisualTreeService3>();
        g_advising = true;
        HRESULT result = service->AdviseVisualTreeChange(this);
        g_advising = false;
        g_compositionDebugKey = nullptr;
        return result;
    }

    void Stop(bool advised) {
        m_stopping = true;
        if (advised) {
            HRESULT result = m_diagnostics.as<IVisualTreeService3>()->UnadviseVisualTreeChange(this);
            if (FAILED(result)) {
                Wh_Log(L"Unable to unsubscribe from XAML notifications: %08X", result);
            }
        }

        DrainReleaseCallbacks();
        std::unordered_map<InstanceHandle, ShellEdit> edits;
        {
            std::lock_guard guard(m_mutex);
            edits.swap(m_edits);
        }
        for (auto& [handle, edit] : edits) {
            try {
                edit.dispatcher.RunAsync(Core::CoreDispatcherPriority::Normal, [&edit] {
                    if (edit.Restore()) {
                        Wh_Log(L"Restored shell border");
                    }
                }).get();
            } catch (...) {
                Wh_Log(L"Unable to restore XAML border: %08X", winrt::to_hresult());
            }
        }
    }

    HRESULT STDMETHODCALLTYPE OnVisualTreeChange(ParentChildRelation relation,
                                                 VisualElement element,
                                                 VisualMutationType mutation) noexcept override {
        if (m_stopping) {
            return S_OK;
        }
        try {
            if (mutation == Add && element.Type && element.Name) {
                auto border = ShellBorderFor(g_startHost, element.Type, element.Name);
                if (border != ShellBorder::None) {
                    Apply(element.Handle, border);
                }
            }
            // XAML reports both handles and may keep them alive even if they were
            // never resolved. Release after the current visual-tree walk returns.
            QueueRelease(element.Handle, relation.Parent);
        } catch (...) {
            Wh_Log(L"Unable to update XAML border: %08X", winrt::to_hresult());
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE OnElementStateChanged(InstanceHandle, VisualElementState,
                                                    PCWSTR) noexcept override {
        return S_OK;
    }

private:
    void Apply(InstanceHandle handle, ShellBorder border) {
        Foundation::IInspectable inspectable;
        if (FAILED(m_diagnostics->GetIInspectableFromHandle(
                handle, reinterpret_cast<::IInspectable**>(winrt::put_abi(inspectable))))) {
            return;
        }
        auto target = inspectable.try_as<Xaml::FrameworkElement>();
        if (!target) {
            return;
        }

        auto parent = Xaml::Media::VisualTreeHelper::GetParent(target)
                          .try_as<Xaml::FrameworkElement>();
        auto parentName = parent ? parent.Name() : winrt::hstring{};
        auto parentType = parent ? winrt::get_class_name(parent) : winrt::hstring{};
        if (border == ShellBorder::Start && !IsStartPane(parentName) &&
            !(parentName.empty() && parentType == L"Windows.UI.Xaml.Controls.Grid")) {
            return;
        }

        std::lock_guard guard(m_mutex);
        if (m_stopping) {
            return;
        }
        std::erase_if(m_edits, [](const auto& entry) {
            // Weak references can only be resolved on their own dispatcher.
            return entry.second.dispatcher.HasThreadAccess() && !entry.second.element.get();
        });
        if (m_edits.contains(handle)) {
            return;
        }

        ShellEdit edit{winrt::make_weak(target), target.Dispatcher(), border, nullptr};
        auto property = edit.Property();
        auto original = target.ReadLocalValue(property);
        if (original != Xaml::DependencyProperty::UnsetValue()) {
            edit.original = original;
        }

        auto [entry, inserted] = m_edits.emplace(handle, std::move(edit));
        try {
            if (border == ShellBorder::Start) {
                target.as<Xaml::Controls::Border>().BorderThickness(Xaml::Thickness{});
            } else {
                target.Visibility(Xaml::Visibility::Collapsed);
            }
        } catch (...) {
            m_edits.erase(entry);
            throw;
        }
        Wh_Log(L"Updated shell border: %s (parent %s#%s)",
               border == ShellBorder::Start ? L"Start" : L"taskbar",
               parentType.c_str(), parentName.c_str());
    }

    void QueueRelease(InstanceHandle element, InstanceHandle parent) {
        DWORD thread = GetCurrentThreadId();
        std::lock_guard guard(m_releaseMutex);
        if (m_stopping) {
            return;
        }
        auto& batch = m_releases[thread];
        if (!batch.queue) {
            batch.queue = System::DispatcherQueue::GetForCurrentThread();
            if (!batch.queue) {
                m_releases.erase(thread);
                return;
            }
        }
        if (element) {
            batch.pending.push_back(element);
        }
        if (parent && parent != element) {
            batch.pending.push_back(parent);
        }
        if (batch.pending.empty() || batch.scheduled) {
            return;
        }
        batch.scheduled = batch.queue.TryEnqueue(
            System::DispatcherQueuePriority::Low,
            [self = get_strong(), thread] { self->ReleaseHandles(thread); });
    }

    void ReleaseHandles(DWORD thread) noexcept {
        std::vector<InstanceHandle> handles;
        {
            std::lock_guard guard(m_releaseMutex);
            auto& batch = m_releases.at(thread);
            handles.swap(batch.pending);
            batch.scheduled = false;
        }

        std::sort(handles.begin(), handles.end());
        handles.erase(std::unique(handles.begin(), handles.end()), handles.end());
        for (InstanceHandle handle : handles) {
            m_testHooks->UnregisterInstance(handle);
        }
    }

    void DrainReleaseCallbacks() {
        std::vector<std::pair<DWORD, System::DispatcherQueue>> queues;
        {
            std::lock_guard guard(m_releaseMutex);
            for (auto& [thread, batch] : m_releases) {
                queues.emplace_back(thread, batch.queue);
            }
        }
        for (auto& [thread, queue] : queues) {
            HANDLE uiThread = OpenThread(SYNCHRONIZE, FALSE, thread);
            if (!uiThread) {
                continue;
            }
            HANDLE done = CreateEventW(nullptr, TRUE, FALSE, nullptr);
            try {
                if (done && queue.TryEnqueue(System::DispatcherQueuePriority::Low,
                                             [done] { SetEvent(done); })) {
                    HANDLE events[] = {done, uiThread};
                    WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, INFINITE);
                }
            } catch (...) {
                Wh_Log(L"XAML dispatcher is unavailable: %08X", winrt::to_hresult());
            }
            if (done) {
                CloseHandle(done);
            }
            CloseHandle(uiThread);
        }
    }

    struct ReleaseBatch {
        System::DispatcherQueue queue{nullptr};
        std::vector<InstanceHandle> pending;
        bool scheduled = false;
    };

    winrt::com_ptr<IXamlDiagnostics> m_diagnostics;
    winrt::com_ptr<DiagnosticsTestHooks> m_testHooks;
    std::atomic<bool> m_stopping = false;
    std::mutex m_mutex;
    std::unordered_map<InstanceHandle, ShellEdit> m_edits;
    std::mutex m_releaseMutex;
    std::unordered_map<DWORD, ReleaseBatch> m_releases;
};

Worker g_shellWorker;
std::mutex g_siteMutex;
winrt::com_ptr<IXamlDiagnostics> g_site;
HANDLE g_siteReady;

struct Tap : winrt::implements<Tap, IObjectWithSite, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE SetSite(IUnknown* site) noexcept override {
        m_site.copy_from(site);
        if (site) {
            // The TAP loader adds a reference independently of Windhawk.
            FreeLibrary(OwnModule());
            std::lock_guard guard(g_siteMutex);
            if (g_siteReady) {
                g_site = m_site.try_as<IXamlDiagnostics>();
                SetEvent(g_siteReady);
            }
        }
        return S_OK;
    }

    HRESULT STDMETHODCALLTYPE GetSite(REFIID iid, void** result) noexcept override {
        return m_site ? m_site->QueryInterface(iid, result) : E_FAIL;
    }

    winrt::com_ptr<IUnknown> m_site;
};

struct TapFactory : winrt::implements<TapFactory, IClassFactory, winrt::non_agile> {
    HRESULT STDMETHODCALLTYPE CreateInstance(IUnknown* outer, REFIID iid,
                                             void** result) noexcept override try {
        *result = nullptr;
        if (outer) {
            return CLASS_E_NOAGGREGATION;
        }
        return winrt::make_self<Tap>()->QueryInterface(iid, result);
    } catch (...) {
        return winrt::to_hresult();
    }

    HRESULT STDMETHODCALLTYPE LockServer(BOOL lock) noexcept override {
        if (lock) {
            ++winrt::get_module_lock();
        } else {
            --winrt::get_module_lock();
        }
        return S_OK;
    }
};

HRESULT AttachTap() {
    auto xaml = GetModuleHandleW(L"Windows.UI.Xaml.dll");
    if (!xaml) {
        return HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND);
    }
    auto initialize = reinterpret_cast<XamlInitialize>(
        GetProcAddress(xaml, "InitializeXamlDiagnosticsEx"));
    if (!initialize) {
        return HRESULT_FROM_WIN32(ERROR_PROC_NOT_FOUND);
    }
    WCHAR path[32768];
    DWORD length = GetModuleFileNameW(OwnModule(), path, ARRAYSIZE(path));
    if (!length || length == ARRAYSIZE(path)) {
        return HRESULT_FROM_WIN32(GetLastError());
    }
    // The default connection is normally the first one. Multiple XAML islands
    // can have higher-numbered endpoints.
    HRESULT result = HRESULT_FROM_WIN32(ERROR_NOT_FOUND);
    for (unsigned index = 1; index <= 64 && result == HRESULT_FROM_WIN32(ERROR_NOT_FOUND); ++index) {
        WCHAR connection[40];
        swprintf_s(connection, L"VisualDiagConnection%u", index);
        result = initialize(connection, GetCurrentProcessId(), L"", path, kTapClass, nullptr);
    }
    return result;
}

DWORD WINAPI RunShellWorker(void* stopParameter) {
    HANDLE stop = static_cast<HANDLE>(stopParameter);
    HRESULT apartment = CoInitializeEx(nullptr, COINIT_MULTITHREADED);
    if (FAILED(apartment)) {
        return 0;
    }

    HANDLE siteReady = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    if (!siteReady) {
        CoUninitialize();
        return 0;
    }
    {
        std::lock_guard guard(g_siteMutex);
        g_siteReady = siteReady;
    }
    HRESULT attach = E_FAIL;
    // Only on enable or process startup. No timer or retries after attach.
    for (int attempt = 0; attempt < 25 && WaitForSingleObject(stop, 0) == WAIT_TIMEOUT; ++attempt) {
        attach = AttachTap();
        if (SUCCEEDED(attach) ||
            (attach != HRESULT_FROM_WIN32(ERROR_MOD_NOT_FOUND) &&
             attach != HRESULT_FROM_WIN32(ERROR_NOT_FOUND))) {
            break;
        }
        WaitForSingleObject(stop, 200);
    }

    // InitializeXamlDiagnosticsEx can return before the UI thread calls SetSite.
    if (SUCCEEDED(attach)) {
        const HANDLE events[] = {stop, siteReady};
        WaitForMultipleObjects(ARRAYSIZE(events), events, FALSE, 5000);
    }
    winrt::com_ptr<IXamlDiagnostics> site;
    {
        std::lock_guard guard(g_siteMutex);
        g_siteReady = nullptr;
        site = std::move(g_site);
    }
    CloseHandle(siteReady);
    try {
        if (site && WaitForSingleObject(stop, 0) == WAIT_TIMEOUT) {
            auto watcher = winrt::make_self<TreeWatcher>(std::move(site));
            HRESULT result = watcher->Advise();
            if (SUCCEEDED(result)) {
                WaitForSingleObject(stop, INFINITE);
            } else {
                Wh_Log(L"Unable to subscribe to XAML notifications: %08X", result);
            }
            watcher->Stop(SUCCEEDED(result));
        } else if (WaitForSingleObject(stop, 0) == WAIT_TIMEOUT) {
            Wh_Log(L"Unable to attach to shell XAML: %08X",
                   SUCCEEDED(attach) ? HRESULT_FROM_WIN32(ERROR_TIMEOUT) : attach);
        }
    } catch (...) {
        Wh_Log(L"Shell worker failed: %08X", winrt::to_hresult());
    }
    site = nullptr;
    CoUninitialize();
    return 0;
}

bool g_shellHooks;

bool InstallShellHooks() {
    HMODULE kernelBase = GetModuleHandleW(L"kernelbase.dll");
    if (!kernelBase) {
        return false;
    }
    auto openKey = reinterpret_cast<void*>(GetProcAddress(kernelBase, "RegOpenKeyExW"));
    auto queryValue = reinterpret_cast<void*>(GetProcAddress(kernelBase, "RegQueryValueExW"));
    return openKey && queryValue &&
           Wh_SetFunctionHook(openKey, reinterpret_cast<void*>(&OpenKeyHook),
                              reinterpret_cast<void**>(&g_openKey)) &&
           Wh_SetFunctionHook(queryValue, reinterpret_cast<void*>(&QueryValueHook),
                              reinterpret_cast<void**>(&g_queryValue));
}

WindowEffect ReadSetting(PCWSTR name) {
    PCWSTR value = Wh_GetStringSetting(name);
    std::wstring_view setting = value ? value : L"";
    WindowEffect effect = WindowEffect::Default;
    if (setting == L"hide") {
        effect = WindowEffect::HideBorder;
    } else if (setting == L"caption") {
        effect = WindowEffect::RemoveCaptionAccent;
    }
    if (value) {
        Wh_FreeStringSetting(value);
    }
    return effect;
}

void ApplySettings() {
    if (!g_startHost) {
        WindowSettings settings{
            ReadSetting(L"activeWindow"),
            ReadSetting(L"inactiveWindow") == WindowEffect::HideBorder,
            ReadSetting(L"dialog") == WindowEffect::HideBorder,
        };
        if (settings.active != g_windowSettings.active ||
            settings.hideInactive != g_windowSettings.hideInactive ||
            settings.hideDialog != g_windowSettings.hideDialog) {
            g_windowWorker.Stop();
            g_windowSettings = settings;
            if (settings.AnyEnabled() && (!GetShellWindow() || IsMainExplorer())) {
                g_windowWorker.Start(RunWindowWorker);
            }
        }
    }

    bool shellEnabled = ReadSetting(g_startHost ? L"startMenu" : L"taskbar") ==
                        WindowEffect::HideBorder;
    if (!shellEnabled) {
        g_shellWorker.Stop();
    } else if (!g_shellWorker.thread && (g_startHost || !GetShellWindow() || IsMainExplorer())) {
        if (!g_shellHooks) {
            g_shellHooks = InstallShellHooks() && Wh_ApplyHookOperations();
        }
        if (g_shellHooks) {
            g_shellWorker.Start(RunShellWorker);
        }
    }
}

}  // namespace

#pragma clang diagnostic push
#pragma clang diagnostic ignored "-Wdll-attribute-on-redeclaration"

__declspec(dllexport) STDAPI DllGetClassObject(REFCLSID clsid, REFIID iid, void** result) try {
    *result = nullptr;
    if (clsid != kTapClass) {
        return CLASS_E_CLASSNOTAVAILABLE;
    }
    return winrt::make_self<TapFactory>()->QueryInterface(iid, result);
} catch (...) {
    return winrt::to_hresult();
}

__declspec(dllexport) STDAPI DllCanUnloadNow() {
    return winrt::get_module_lock() ? S_FALSE : S_OK;
}

#pragma clang diagnostic pop

BOOL Wh_ModInit() {
    WCHAR executable[MAX_PATH];
    DWORD length = GetModuleFileNameW(nullptr, executable, ARRAYSIZE(executable));
    if (!length || length == ARRAYSIZE(executable)) {
        return FALSE;
    }
    PCWSTR name = wcsrchr(executable, L'\\');
    if (!name) {
        return FALSE;
    }
    g_startHost = _wcsicmp(name + 1, L"StartMenuExperienceHost.exe") == 0;
    return g_startHost || _wcsicmp(name + 1, L"explorer.exe") == 0;
}

void Wh_ModAfterInit() {
    ApplySettings();
}

void Wh_ModSettingsChanged() {
    ApplySettings();
}

void Wh_ModBeforeUninit() {
    g_shellWorker.Stop();
    g_windowWorker.Stop();
}
#endif

#include <windhawk_api.h>
#include <atomic>
#include <cstdio>
#include <string>

HCURSOR WINAPI ObservedSetCursor(HCURSOR cursor);
HWND HostLookupReentry();
unsigned ownModuleReferences;
unsigned threadHookRegistrations;
unsigned backgroundThreadStarts;
unsigned helperWindowCreates;
bool simulateHostLookup;
bool reenterHostLookup;
unsigned fullWindowScans;
ULONGLONG simulatedTime = 1000;
HWND simulatedWindowAtPointer;
HWND observedHostRoot;
bool simulateScheme;
HCURSOR schemeCursor;
unsigned schemeBitmapReads;
PCWSTR simulatedCommandLine;
int selectionArrowsSetting = 1;

LPWSTR WINAPI TestCommandLine() {
    return simulatedCommandLine ? const_cast<LPWSTR>(simulatedCommandLine) : GetCommandLineW();
}

HCURSOR WINAPI TestLoadCursor(HINSTANCE module, LPCWSTR name) {
    if (simulateScheme && !module && name == IDC_UPARROW) return schemeCursor;
    return LoadCursorW(module, name);
}

BOOL WINAPI TestIconInfo(HICON cursor, PICONINFO info) {
    if (simulateScheme && cursor == schemeCursor) ++schemeBitmapReads;
    return GetIconInfo(cursor, info);
}

int TestIntSetting(PCWSTR name, ...) {
    return wcscmp(name, L"cursors.selectionArrows") == 0 ? selectionArrowsSetting : 1;
}

PCWSTR TestStringSetting(PCWSTR, ...) { return L""; }
void TestFreeStringSetting(PCWSTR) {}

HWND WINAPI TestGetAncestor(HWND window, UINT flags) {
    if (simulateHostLookup) observedHostRoot = window;
    return GetAncestor(window, flags);
}

HWND WINAPI TestWindowFromPoint(POINT point) {
    return simulateHostLookup ? HostLookupReentry() : WindowFromPoint(point);
}

BOOL WINAPI TestGuiThreadInfo(DWORD thread, PGUITHREADINFO info) {
    if (!simulateHostLookup) return GetGUIThreadInfo(thread, info);
    *info = {sizeof(*info)};
    return TRUE;
}

BOOL WINAPI TestEnumThreadWindows(DWORD thread, WNDENUMPROC callback, LPARAM parameter) {
    return simulateHostLookup ? TRUE : EnumThreadWindows(thread, callback, parameter);
}

BOOL WINAPI TestEnumWindows(WNDENUMPROC callback, LPARAM parameter) {
    if (!simulateHostLookup) return EnumWindows(callback, parameter);
    ++fullWindowScans;
    return TRUE;
}

ULONGLONG WINAPI TestTickCount() {
    return simulateHostLookup || simulateScheme ? simulatedTime : GetTickCount64();
}

BOOL WINAPI TestModuleHandle(DWORD flags, LPCWSTR address, HMODULE* module) {
    if ((flags & GET_MODULE_HANDLE_EX_FLAG_FROM_ADDRESS) &&
        !(flags & GET_MODULE_HANDLE_EX_FLAG_UNCHANGED_REFCOUNT)) {
        ++ownModuleReferences;
    }
    return GetModuleHandleExW(flags, address, module);
}

HHOOK WINAPI TestWindowsHook(int type, HOOKPROC procedure, HINSTANCE module, DWORD thread) {
    ++threadHookRegistrations;
    return SetWindowsHookExW(type, procedure, module, thread);
}

HANDLE WINAPI TestCreateThread(LPSECURITY_ATTRIBUTES attributes, SIZE_T stackSize,
                               LPTHREAD_START_ROUTINE procedure, LPVOID parameter,
                               DWORD flags, LPDWORD thread) {
    ++backgroundThreadStarts;
    return CreateThread(attributes, stackSize, procedure, parameter, flags, thread);
}

HWND WINAPI TestCreateWindow(DWORD extended, LPCWSTR className, LPCWSTR title, DWORD style,
                             int x, int y, int width, int height, HWND parent, HMENU menu,
                             HINSTANCE module, LPVOID parameter) {
    ++helperWindowCreates;
    return CreateWindowExW(extended, className, title, style, x, y, width, height,
                           parent, menu, module, parameter);
}

#define GetModuleHandleExW TestModuleHandle
#define SetWindowsHookExW TestWindowsHook
#define CreateThread TestCreateThread
#define CreateWindowExW TestCreateWindow
#define WindowFromPoint TestWindowFromPoint
#define GetGUIThreadInfo TestGuiThreadInfo
#define EnumThreadWindows TestEnumThreadWindows
#define EnumWindows TestEnumWindows
#define GetTickCount64 TestTickCount
#define GetAncestor TestGetAncestor
#define GetCommandLineW TestCommandLine
#define LoadCursorW TestLoadCursor
#define GetIconInfo TestIconInfo
#define Wh_GetIntSetting TestIntSetting
#define Wh_GetStringSetting TestStringSetting
#define Wh_FreeStringSetting TestFreeStringSetting
#include "../Mod/cursor-scheme-extension.wh.cpp"
#undef Wh_FreeStringSetting
#undef Wh_GetStringSetting
#undef Wh_GetIntSetting
#undef GetIconInfo
#undef LoadCursorW
#undef GetCommandLineW
#undef GetAncestor
#undef GetTickCount64
#undef EnumWindows
#undef EnumThreadWindows
#undef GetGUIThreadInfo
#undef WindowFromPoint
#undef CreateWindowExW
#undef CreateThread
#undef SetWindowsHookExW
#undef GetModuleHandleExW

namespace {
int failures;
HCURSOR expectedInObserver;
HCURSOR nestedInput;
HCURSOR nestedPrevious;
bool observing;
bool insideObserver;
unsigned nativeCreates;
HCURSOR hostNestedSource;
HCURSOR hostNestedPrevious;
HCURSOR hostNestedDisplayed;

void Check(bool value, const char* name) {
    if (!value) {
        ++failures;
        std::printf("FAIL %s\n", name);
    }
}

HCURSOR Fixture(uint32_t color) {
    const auto& shape = kOfficeShapes[2].second;
    CursorImage image;
    image.width = shape.width;
    image.height = shape.height;
    image.hotX = shape.width / 2;
    image.hotY = shape.height / 2;
    image.mask.resize(size_t(shape.width) * shape.height, 0xffffff);
    image.color.resize(image.mask.size());
    for (int y = 0; y < shape.height; ++y) {
        for (int x = 0; x < shape.width; ++x) {
            if (shape.rows[y] & (1u << x)) {
                const size_t index = size_t(y) * shape.width + x;
                image.mask[index] = 0;
                image.color[index] = color;
            }
        }
    }
    ++nativeCreates;
    return MakeCursor(image);
}

HWND AppWindow() {
    return CreateWindowExW(WS_EX_NOACTIVATE, L"STATIC", L"CSE test", WS_POPUP,
                           0, 0, 40, 40, nullptr, nullptr, GetModuleHandleW(nullptr), nullptr);
}

void Start() {
    g_stopping = false;
    g_watchStarted = false;
    g_setCursorOriginal = ObservedSetCursor;
    g_getCursorOriginal = GetCursor;
    g_enabledTypes = UINT32_MAX;
    g_enabledTargets = UINT32_MAX;
    g_isOffice = true;
    g_smoothingTypes = TypeBit(CursorType::SelectionArrows);
    g_needsRefresh.fill(true);
    g_nextSourceProbe.fill(0);
}

void Finish() {
    observing = false;
    Wh_ModBeforeUninit();
    Wh_ModUninit();
}

bool Readable(HCURSOR cursor) {
    IconBitmaps bitmap;
    return cursor && GetIconInfo(cursor, &bitmap);
}

void UnloadContract() {
    HWND window = AppWindow();
    HCURSOR original = Fixture(0xff456789);
    SetCursorHook(original);
    HCURSOR displayed = GetCursor();
    Check(window && displayed && displayed != original, "themed unload fixture");
    Finish();
    Check(ownModuleReferences == 0,
          "unload never retains the mod image");
    Check(threadHookRegistrations == 0, "no thread hooks survive mod removal");
    Check(backgroundThreadStarts == 0, "unload starts no background callback");
    Check(helperWindowCreates == 0, "no mod-owned window procedure survives removal");
    Check(Readable(displayed), "published Win32 handle survives module cleanup");
    SetCursor(original);
    Check(GetCursor() == original, "next native SetCursor restores application pointer");
    SetCursor(nullptr);
    if (Readable(displayed)) DestroyCursor(displayed);
    DestroyCursor(original);
    DestroyWindow(window);
}

void Reentrant() {
    HWND window = AppWindow();
    HCURSOR a = Fixture(0xff234567), b = Fixture(0xff345678);
    Check(window && a && b, "fixture");
    expectedInObserver = a;
    observing = true;
    SetCursorHook(a);
    Check(GetCursorHook() == a, "same-thread logical read");
    nestedInput = b;
    SetCursorHook(a);
    Check(nestedPrevious == a, "nested SetCursor returns old logical source");
    Check(GetCursorHook() == b, "nested SetCursor wins");
    Check(g_activeCalls == 0, "all transactions unpinned");
    observing = false;
    SetCursorHook(nullptr);
    Finish();
    DestroyCursor(a);
    DestroyCursor(b);
    DestroyWindow(window);
}

struct SharedTest {
    HANDLE ready;
    HANDLE next;
    HANDLE done;
    HCURSOR first;
    HCURSOR second;
    HCURSOR read = nullptr;
    HCURSOR previous = nullptr;
    HCURSOR saved = nullptr;
    bool recovered = false;
    HANDLE exit = nullptr;
};

DWORD WINAPI SharedReader(void* argument) {
    auto& test = *static_cast<SharedTest*>(argument);
    HWND window = AppWindow();
    SetEvent(test.ready);
    if (WaitForSingleObject(test.next, 5000) == WAIT_OBJECT_0) {
        test.read = GetCursorHook();
        test.previous = SetCursorHook(test.second);
        test.saved = GetCursorHook();
        SetEvent(test.done);
    }
    if (WaitForSingleObject(test.next, 5000) == WAIT_OBJECT_0) {
        const bool readable = Readable(test.saved);
        const HCURSOR before = GetCursorHook();
        const HCURSOR previous = SetCursorHook(test.saved);
        const HCURSOR after = GetCursorHook();
        test.recovered = readable && previous == before && after == test.saved;
        if (!test.recovered) {
            std::printf("saved=%p valid=%d before=%p previous=%p after=%p\n",
                        test.saved, readable, before, previous, after);
        }
        SetEvent(test.done);
    }
    WaitForSingleObject(test.exit, 5000);
    if (window) DestroyWindow(window);
    return 0;
}

void SharedQueues() {
    HWND window = AppWindow();
    HCURSOR a = Fixture(0xff234567), b = Fixture(0xff345678);
    SharedTest test{CreateEventW(nullptr, FALSE, FALSE, nullptr),
                    CreateEventW(nullptr, FALSE, FALSE, nullptr),
                    CreateEventW(nullptr, FALSE, FALSE, nullptr), a, b};
    test.exit = CreateEventW(nullptr, TRUE, FALSE, nullptr);
    DWORD id = 0;
    HANDLE reader = CreateThread(nullptr, 0, SharedReader, &test, 0, &id);
    Check(reader && WaitForSingleObject(test.ready, 5000) == WAIT_OBJECT_0, "reader ready");
    Check(AttachThreadInput(GetCurrentThreadId(), id, TRUE), "queues joined");
    SetCursorHook(a);
    const HCURSOR firstDisplay = GetCursor();
    SetEvent(test.next);
    Check(WaitForSingleObject(test.done, 5000) == WAIT_OBJECT_0, "reader completed");
    Check(test.read == a && test.previous == a && test.saved == b,
          "attached thread reads and returns exact source");
    Check(GetCursorHook() == b, "origin thread observes second source");
    const HCURSOR sharedDisplay = GetCursor();
    {
        std::lock_guard lock(g_mutex);
        g_smoothingTypes = 0;
        g_needsRefresh.fill(true);
    }
    Check(AttachThreadInput(GetCurrentThreadId(), id, FALSE), "queues detached");
    SetEvent(test.next);
    Check(WaitForSingleObject(test.done, 5000) == WAIT_OBJECT_0 && test.recovered,
          "saved logical cursor survives generation change and detach");
    const HCURSOR actualDisplay = GetCursor();
    const HCURSOR expectedQuiet = actualDisplay == firstDisplay ? a
        : actualDisplay == sharedDisplay ? b : actualDisplay;
    Check(Readable(sharedDisplay) && Readable(actualDisplay) &&
              GetCursorHook() == expectedQuiet,
          "detached quiet queue keeps its current instance and logical identity");
    SetEvent(test.exit);
    Check(WaitForSingleObject(reader, 5000) == WAIT_OBJECT_0, "reader exited");
    CloseHandle(reader);
    CloseHandle(test.ready);
    CloseHandle(test.next);
    CloseHandle(test.done);
    CloseHandle(test.exit);
    SetCursorHook(nullptr);
    Finish();
    DestroyCursor(a);
    DestroyCursor(b);
    DestroyWindow(window);
}

void Alternating() {
    HWND window = AppWindow();
    HCURSOR a = Fixture(0xff234567), b = Fixture(0xff345678);
    SetCursorHook(a);
    HCURSOR outputA = GetCursor();
    SetCursorHook(b);
    HCURSOR outputB = GetCursor();
    Check(outputA && outputB && outputA != outputB, "each source has unambiguous identity");
    const auto count = g_instances.size();
    const uint64_t generation = g_nextGeneration;
    for (unsigned i = 0; i < 5000; ++i) {
        HCURSOR source = i % 2 ? b : a;
        SetCursorHook(source);
        if (GetCursorHook() != source) {
            Check(false, "alternating logical cursor");
            break;
        }
    }
    Check(g_instances.size() == count && g_nextGeneration == generation,
          "5000 alternating calls reuse existing handles and image");
    SetCursorHook(nullptr);
    Finish();
    DestroyCursor(a);
    DestroyCursor(b);
    DestroyWindow(window);
}

struct QuietThread {
    HANDLE ready = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    HANDLE resume = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    HANDLE checked = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    HANDLE exit = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    HCURSOR original;
    HCURSOR displayed = nullptr;
    bool inherited = false;
    bool survived = false;
    bool restored = false;

    ~QuietThread() {
        for (HANDLE event : {ready, resume, checked, exit}) CloseHandle(event);
    }
};

DWORD WINAPI QuietOwner(void* argument) {
    auto& test = *static_cast<QuietThread*>(argument);
    HWND window = AppWindow();
    if (!test.inherited) {
        SetCursorHook(test.original);
        test.displayed = GetCursor();
    }
    SetEvent(test.ready);
    WaitForSingleObject(test.resume, 5000);
    const bool readable = Readable(test.displayed);
    if (test.inherited && readable) {
        // Detaching input queues can select a different cursor. A borrowed
        // native handle must still be safe for the application to reuse later.
        SetCursor(test.displayed);
    }
    test.survived = readable && GetCursor() == test.displayed;
    // The application, rather than any retained mod callback, selects its pointer.
    SetCursor(test.original);
    test.restored = GetCursor() == test.original;
    SetEvent(test.checked);
    WaitForSingleObject(test.exit, 5000);
    SetCursor(nullptr);
    DestroyWindow(window);
    return 0;
}

void QuietUnload(bool inherited, bool reload) {
    HCURSOR original = Fixture(0xff456789);
    QuietThread test;
    test.original = original;
    test.inherited = inherited;
    DWORD threadId = 0;
    HANDLE owner = CreateThread(nullptr, 0, QuietOwner, &test, 0, &threadId);
    Check(owner && WaitForSingleObject(test.ready, 5000) == WAIT_OBJECT_0, "quiet owner ready");
    HWND window = AppWindow();
    if (inherited) {
        Check(AttachThreadInput(GetCurrentThreadId(), threadId, TRUE), "queues joined");
        SetCursorHook(original);
        test.displayed = GetCursor();
        Check(test.displayed != original, "quiet queue inherits themed handle");
        Check(AttachThreadInput(GetCurrentThreadId(), threadId, FALSE), "queues detached");
        SetCursorHook(nullptr);
    }
    const auto before = GetTickCount64();
    Finish();
    Check(GetTickCount64() - before < 1000, "unload does not wait for another GUI thread");
    Check(Readable(test.displayed), "unresponsive queue retains valid native cursor");

    HCURSOR next = nullptr, displayed = nullptr;
    if (reload) {
        Start();
        next = Fixture(0xff56789a);
        SetCursorHook(next);
        displayed = GetCursor();
        Check(displayed != next && GetCursorHook() == next, "new load replaces its own cursor");
    }
    SetEvent(test.resume);
    Check(WaitForSingleObject(test.checked, 5000) == WAIT_OBJECT_0 &&
              test.survived && test.restored,
          "old queue survives unload and resumes with native SetCursor");
    if (reload) {
        Check(Readable(displayed) && GetCursorHook() == next, "old queue does not affect new load");
        SetCursorHook(nullptr);
        Finish();
    }
    SetEvent(test.exit);
    Check(WaitForSingleObject(owner, 5000) == WAIT_OBJECT_0, "quiet owner exited");
    CloseHandle(owner);
    SetCursor(nullptr);
    DestroyCursor(original);
    if (next) DestroyCursor(next);
    DestroyWindow(window);
}

void StoppingPassthrough() {
    HCURSOR original = Fixture(0xff6789ab);
    SetCursorHook(original);
    const HCURSOR displayed = GetCursor();
    const size_t instances = g_instances.size();
    Wh_ModBeforeUninit();
    Check(SetCursorHook(original) == original && GetCursor() == original,
          "stopping hook returns logical previous cursor and passes native source");
    Check(g_instances.size() == instances, "stopping creates no replacement");
    Wh_ModUninit();
    Check(Readable(displayed), "native published handle survives stopped hook");
    SetCursor(nullptr);
    DestroyCursor(original);
}

void InstanceLimit() {
    std::vector<HCURSOR> originals;
    for (unsigned i = 0; i < 129; ++i) {
        HCURSOR original = Fixture(0xff000001 + i);
        Check(original != nullptr, "instance-limit fixture");
        originals.push_back(original);
        SetCursorHook(original);
        Check(GetCursorHook() == original, "instance-limit logical identity");
        if (i == 128) {
            Check(GetCursor() == original, "instance cap falls back to application cursor");
        }
    }
    Check(g_instances.size() == 128, "instance cap bounds native handles per load");
    SetCursorHook(nullptr);
    Finish();
    for (HCURSOR original : originals) DestroyCursor(original);
}

void ReuseInstances() {
    HCURSOR original = Fixture(0xff234567);
    SetCursorHook(original);
    const HCURSOR smooth = GetCursor();
    g_smoothingTypes = 0;
    g_needsRefresh.fill(true);
    SetCursorHook(original);
    const HCURSOR sharp = GetCursor();
    Check(smooth != original && sharp != original && smooth != sharp,
          "both smoothing variants have native instances");
    for (unsigned i = 0; i < 150; ++i) {
        g_smoothingTypes = i % 2 ? 0 : TypeBit(CursorType::SelectionArrows);
        g_needsRefresh.fill(true);
        SetCursorHook(original);
        Check(GetCursor() == (i % 2 ? sharp : smooth),
              "returning to a prior appearance reuses its published instance");
        Check(GetCursorHook() == original, "reused instance keeps logical identity");
    }
    Check(g_instances.size() == 2, "smoothing cycles do not consume the instance budget");
    Check(Readable(smooth) && Readable(sharp), "both published variants remain valid");
    SetCursorHook(nullptr);
    Finish();
    DestroyCursor(original);
}

void SourceRefresh() {
    simulateScheme = true;
    g_watchStarted = true;  // Exercise the fallback with no registry subscription.
    HCURSOR original = Fixture(0xff123456);
    HCURSOR firstSource = Fixture(0xff456789);
    HCURSOR secondSource = Fixture(0xff789abc);
    schemeCursor = firstSource;
    SetCursorHook(original);
    const HCURSOR first = GetCursor();
    const unsigned initialReads = schemeBitmapReads;
    for (unsigned i = 0; i < 1000; ++i) {
        SetCursorHook(LoadCursorW(nullptr, IDC_ARROW));
        SetCursorHook(original);
    }
    Check(schemeBitmapReads == initialReads,
          "ordinary cursor transitions do not reread scheme bitmaps");
    schemeCursor = secondSource;
    simulatedTime += 1001;
    SetCursorHook(original);
    Check(GetCursor() != first && GetCursor() != original,
          "unnotified scheme change is discovered while the same role stays active");
    g_changeEvent = CreateEventW(nullptr, FALSE, TRUE, nullptr);
    schemeCursor = firstSource;
    SetCursorHook(original);
    Check(GetCursor() == first, "registry notification refreshes immediately and reuses old scheme");
    SetCursorHook(nullptr);
    Finish();
    simulateScheme = false;
    DestroyCursor(original);
    DestroyCursor(firstSource);
    DestroyCursor(secondSource);
}

void ChromiumCommandLines() {
    const std::pair<PCWSTR, bool> cases[] = {
        {L"app.exe --type=renderer --field-trial-handle=123", true},
        {L"\"C:\\Program Files\\app.exe\"\t\"--type=gpu-process\" --mojo-platform-channel-handle=456", true},
        {L"app.exe --type=utility --field-trial-handle=123", true},
        {L"app.exe --type=renderer", false},
        {L"app.exe --type=browser --field-trial-handle=123", false},
        {L"app.exe \"document --type=renderer --field-trial-handle=123\"", false},
        {L"app.exe --type=renderer-extra --field-trial-handle=123", false},
        {L"app.exe --type=renderer --other=--field-trial-handle=123", false},
        {L"app.exe --type=renderer \"--field-trial-handle=123\"", true},
        {L"app.exe --type=ren\"derer\" --field-trial-handle=123", true},
    };
    for (const auto& [line, expected] : cases) {
        simulatedCommandLine = line;
        Check(IsChromiumChildProcess() == expected, "Chromium switches respect argument boundaries and quotes");
    }
    simulatedCommandLine = nullptr;
    Finish();
}

void SelectionSetting() {
    selectionArrowsSetting = 0;
    UpdateSettings();
    Check(!(g_enabledTypes & TypeBit(CursorType::SelectionArrows)),
          "renamed selection setting preserves disabled value");
    selectionArrowsSetting = 1;
    UpdateSettings();
    Check((g_enabledTypes & TypeBit(CursorType::SelectionArrows)) != 0,
          "renamed selection setting enables selection arrows");
    Finish();
}

void HostLookup(bool reentrant) {
    HCURSOR original = Fixture(0xff123456);
    HCURSOR nested = Fixture(0xff234567);
    SetCursorHook(original);
    g_excludedApplications = {L"excluded-host.exe"};
    g_hasExclusions = true;
    simulateHostLookup = true;
    reenterHostLookup = reentrant;
    hostNestedSource = nested;
    const CursorMatch chromium{SelectionSlot(4), Target::Chromium};
    SetCursorForMatch(original, chromium);
    if (reentrant) {
        Check(hostNestedDisplayed == nested,
              "nested cursor passes through while host exclusion is unresolved");
        Check(hostNestedPrevious == original, "nested host lookup returns logical prior cursor");
    } else {
        for (unsigned i = 0; i < 1000; ++i) SetCursorForMatch(original, chromium);
        Check(fullWindowScans == 1, "negative host lookup scans desktop once within 250 ms");
        simulatedWindowAtPointer = AppWindow();
        observedHostRoot = nullptr;
        SetCursorForMatch(original, chromium);
        Check(simulatedWindowAtPointer && observedHostRoot == simulatedWindowAtPointer &&
                  fullWindowScans == 1,
              "new pointer window is checked immediately during throttle interval");
        DestroyWindow(simulatedWindowAtPointer);
        simulatedWindowAtPointer = nullptr;
        simulatedTime += 250;
        const unsigned before = fullWindowScans;
        SetCursorForMatch(original, chromium);
        Check(fullWindowScans == before + 1, "host lookup retries after the throttle interval");
    }
    simulateHostLookup = false;
    SetCursorHook(nullptr);
    Finish();
    DestroyCursor(original);
    DestroyCursor(nested);
}
}  // namespace

HWND HostLookupReentry() {
    if (reenterHostLookup) {
        reenterHostLookup = false;
        hostNestedPrevious = SetCursorForMatch(
            hostNestedSource, {SelectionSlot(4), Target::Chromium});
        hostNestedDisplayed = GetCursor();
    }
    return simulatedWindowAtPointer;
}

HCURSOR WINAPI ObservedSetCursor(HCURSOR cursor) {
    HCURSOR previous = SetCursor(cursor);
    if (observing && !insideObserver) {
        insideObserver = true;
        Check(GetCursorHook() == expectedInObserver, "logical read inside native notification");
        if (nestedInput) {
            nestedPrevious = SetCursorHook(nestedInput);
        }
        insideObserver = false;
    }
    return previous;
}

int main(int argc, char** argv) {
    if (argc != 2) return 2;
    Start();
    std::string_view test = argv[1];
    if (test == "unload-contract") UnloadContract();
    else if (test == "reentrant") Reentrant();
    else if (test == "shared") SharedQueues();
    else if (test == "cache") Alternating();
    else if (test == "slow") QuietUnload(false, false);
    else if (test == "reload") QuietUnload(false, true);
    else if (test == "inherited") QuietUnload(true, false);
    else if (test == "stopping") StoppingPassthrough();
    else if (test == "limit") InstanceLimit();
    else if (test == "reuse") ReuseInstances();
    else if (test == "refresh") SourceRefresh();
    else if (test == "command-line") ChromiumCommandLines();
    else if (test == "selection-setting") SelectionSetting();
    else if (test == "host-reentrant") HostLookup(true);
    else if (test == "host-throttle") HostLookup(false);
    else return 2;
    std::printf("%s: %s\n", argv[1], failures ? "FAIL" : "PASS");
    return failures ? 1 : 0;
}

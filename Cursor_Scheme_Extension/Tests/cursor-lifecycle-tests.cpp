#include <windhawk_api.h>
#include <atomic>
#include <cstdio>

HCURSOR WINAPI ObservedSetCursor(HCURSOR cursor);

#include "../Mod/cursor-scheme-extension.wh.cpp"

namespace {
int failures;
HCURSOR expectedInObserver;
HCURSOR nestedInput;
HCURSOR nestedPrevious;
bool observing;
bool insideObserver;
unsigned nativeCreates;

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
    BeginCursorSession();
    g_setCursorOriginal = ObservedSetCursor;
    g_getCursorOriginal = GetCursor;
    g_enabledTypes = UINT32_MAX;
    g_enabledTargets = UINT32_MAX;
    g_isOffice = true;
    g_smoothingTypes = TypeBit(CursorType::SelectionArrows);
    g_needsRefresh.fill(true);
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

void NonCooperative() {
    // STATIC with its default procedure never handles the private restore message.
    HWND window = AppWindow();
    HCURSOR original = Fixture(0xff456789);
    SetCursorHook(original);
    HCURSOR displayed = GetCursor();
    Check(window && displayed && displayed != original, "themed before unload");
    Finish();
    Check(GetCursor() == original, "unload restores without app SetCursor");
    Check(!Readable(displayed), "private handle reclaimed");
    SetCursor(nullptr);
    DestroyCursor(original);
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

struct SlowThread {
    HANDLE ready;
    HANDLE resume;
    HANDLE checked;
    HANDLE exit;
    HCURSOR original;
    HCURSOR displayed = nullptr;
    bool restored = false;
};

DWORD WINAPI DelayedOwner(void* argument) {
    auto& test = *static_cast<SlowThread*>(argument);
    HWND window = AppWindow();
    SetCursorHook(test.original);
    test.displayed = GetCursor();
    SetEvent(test.ready);
    WaitForSingleObject(test.resume, 5000);
    MSG msg{};
    const auto end = GetTickCount64() + 2000;
    while (GetTickCount64() < end) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            DispatchMessageW(&msg);
        }
        if (GetCursor() == test.original) {
            test.restored = true;
            break;
        }
        Sleep(5);
    }
    SetEvent(test.checked);
    WaitForSingleObject(test.exit, 5000);
    SetCursor(nullptr);
    DestroyWindow(window);
    return 0;
}

void SlowUnload() {
    HCURSOR original = Fixture(0xff456789);
    SlowThread test{CreateEventW(nullptr, TRUE, FALSE, nullptr),
                    CreateEventW(nullptr, TRUE, FALSE, nullptr),
                    CreateEventW(nullptr, TRUE, FALSE, nullptr),
                    CreateEventW(nullptr, TRUE, FALSE, nullptr), original};
    HANDLE owner = CreateThread(nullptr, 0, DelayedOwner, &test, 0, nullptr);
    Check(owner && WaitForSingleObject(test.ready, 5000) == WAIT_OBJECT_0, "slow owner ready");
    const auto before = GetTickCount64();
    Finish();
    Check(GetTickCount64() - before < 1500 && !g_restoreSessions.empty(),
          "unresponsive queue does not block unload");
    Check(Readable(test.displayed), "unresponsive queue keeps live handle");
    SetEvent(test.resume);
    Check(WaitForSingleObject(test.checked, 5000) == WAIT_OBJECT_0 && test.restored,
          "resumed queue executes deferred restoration");
    const auto deadline = GetTickCount64() + 1000;
    while (Readable(test.displayed) && GetTickCount64() < deadline) {
        Sleep(5);
    }
    Check(!Readable(test.displayed), "resumed queue releases former handle");
    SetEvent(test.exit);
    Check(WaitForSingleObject(owner, 5000) == WAIT_OBJECT_0, "slow owner exited");
    CloseHandle(owner);
    for (HANDLE event : {test.ready, test.resume, test.checked, test.exit}) {
        CloseHandle(event);
    }
    DestroyCursor(original);
}

void ReloadWithPendingQueue() {
    HCURSOR original = Fixture(0xff56789a);
    SlowThread test{CreateEventW(nullptr, TRUE, FALSE, nullptr),
                    CreateEventW(nullptr, TRUE, FALSE, nullptr),
                    CreateEventW(nullptr, TRUE, FALSE, nullptr),
                    CreateEventW(nullptr, TRUE, FALSE, nullptr), original};
    HANDLE owner = CreateThread(nullptr, 0, DelayedOwner, &test, 0, nullptr);
    Check(owner && WaitForSingleObject(test.ready, 5000) == WAIT_OBJECT_0, "reload owner ready");
    Finish();
    Check(!g_restoreSessions.empty(), "old session remains available");
    Start();
    HWND window = AppWindow();
    HCURSOR next = Fixture(0xff6789ab);
    SetCursorHook(next);
    HCURSOR displayed = GetCursor();
    Check(displayed && displayed != next && GetCursorHook() == next,
          "new session is enabled while old cleanup remains");
    SetEvent(test.resume);
    Check(WaitForSingleObject(test.checked, 5000) == WAIT_OBJECT_0 && test.restored,
          "old queue restores without replacing a new session pointer");
    SetEvent(test.exit);
    Check(WaitForSingleObject(owner, 5000) == WAIT_OBJECT_0, "old owner exited after reload");
    Sleep(50);
    Check(Readable(displayed) && GetCursorHook() == next,
          "old cleanup preserves the new session and handle");
    SetCursorHook(nullptr);
    Finish();
    CloseHandle(owner);
    for (HANDLE event : {test.ready, test.resume, test.checked, test.exit}) {
        CloseHandle(event);
    }
    DestroyCursor(original);
    DestroyCursor(next);
    DestroyWindow(window);
}

DWORD WINAPI QuietOwner(void* argument) {
    auto& test = *static_cast<SlowThread*>(argument);
    HWND window = AppWindow();
    SetEvent(test.ready);
    WaitForSingleObject(test.resume, 5000);
    MSG msg{};
    const auto end = GetTickCount64() + 2000;
    while (GetTickCount64() < end) {
        while (PeekMessageW(&msg, nullptr, 0, 0, PM_REMOVE)) {
            DispatchMessageW(&msg);
        }
        if (GetCursor() != test.displayed) {
            test.restored = true;
            break;
        }
        Sleep(5);
    }
    SetEvent(test.checked);
    WaitForSingleObject(test.exit, 5000);
    SetCursor(nullptr);
    DestroyWindow(window);
    return 0;
}

void InheritedQueue() {
    HWND window = AppWindow();
    HCURSOR original = Fixture(0xff789abc);
    SlowThread test{CreateEventW(nullptr, TRUE, FALSE, nullptr),
                    CreateEventW(nullptr, TRUE, FALSE, nullptr),
                    CreateEventW(nullptr, TRUE, FALSE, nullptr),
                    CreateEventW(nullptr, TRUE, FALSE, nullptr), original};
    DWORD thread = 0;
    HANDLE owner = CreateThread(nullptr, 0, QuietOwner, &test, 0, &thread);
    Check(owner && WaitForSingleObject(test.ready, 5000) == WAIT_OBJECT_0, "quiet owner ready");
    Check(AttachThreadInput(GetCurrentThreadId(), thread, TRUE), "quiet queue attached");
    SetCursorHook(original);
    const HCURSOR displayed = GetCursor();
    test.displayed = displayed;
    Check(displayed != original, "quiet queue receives a published alias");
    Check(AttachThreadInput(GetCurrentThreadId(), thread, FALSE), "quiet queue detached");
    SetCursorHook(nullptr);
    Finish();
    SetEvent(test.resume);
    Check(WaitForSingleObject(test.checked, 5000) == WAIT_OBJECT_0 && test.restored,
          "untracked input owner restores without calling either hook");
    SetEvent(test.exit);
    Check(WaitForSingleObject(owner, 5000) == WAIT_OBJECT_0, "quiet owner exited");
    CloseHandle(owner);
    for (HANDLE event : {test.ready, test.resume, test.checked, test.exit}) {
        CloseHandle(event);
    }
    DestroyCursor(original);
    DestroyWindow(window);
}
}  // namespace

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
    if (test == "reentrant") Reentrant();
    else if (test == "unload") NonCooperative();
    else if (test == "shared") SharedQueues();
    else if (test == "cache") Alternating();
    else if (test == "slow") SlowUnload();
    else if (test == "reload") ReloadWithPendingQueue();
    else if (test == "inherited") InheritedQueue();
    else return 2;
    std::printf("%s: %s\n", argv[1], failures ? "FAIL" : "PASS");
    return failures ? 1 : 0;
}

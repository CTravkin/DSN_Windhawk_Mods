#include <windows.h>
#include <cstdio>

namespace {
using Select = HCURSOR (*)(HCURSOR);
int failures;

void Check(bool value, const char* name) {
    if (!value) {
        ++failures;
        std::printf("FAIL %s\n", name);
    }
}

bool Readable(HCURSOR cursor) {
    ICONINFO info{};
    const bool valid = cursor && GetIconInfo(cursor, &info);
    if (info.hbmMask) DeleteObject(info.hbmMask);
    if (info.hbmColor) DeleteObject(info.hbmColor);
    return valid;
}

struct Owner {
    HANDLE work = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    HANDLE done = CreateEventW(nullptr, FALSE, FALSE, nullptr);
    unsigned command = 0;
    Select select = nullptr;
    HCURSOR original = nullptr;
    HCURSOR displayed = nullptr;
    bool survived = false;
    bool restored = false;
};

DWORD WINAPI GuiOwner(void* argument) {
    auto& owner = *static_cast<Owner*>(argument);
    HWND window = CreateWindowExW(0, L"STATIC", L"CSE unload test", WS_POPUP,
                                   0, 0, 40, 40, nullptr, nullptr, nullptr, nullptr);
    SetEvent(owner.done);
    while (WaitForSingleObject(owner.work, 5000) == WAIT_OBJECT_0) {
        if (owner.command == 1) {
            owner.displayed = owner.select(owner.original);
        } else if (owner.command == 2) {
            // This function belongs to the EXE and runs after the fixture DLL
            // has been freed. No function pointer into that DLL is invoked.
            owner.survived = Readable(owner.displayed) && GetCursor() == owner.displayed;
            SetCursor(owner.original);
            owner.restored = GetCursor() == owner.original;
            SetCursor(nullptr);
        } else {
            break;
        }
        SetEvent(owner.done);
    }
    if (window) DestroyWindow(window);
    return 0;
}

bool Run(Owner& owner, unsigned command) {
    owner.command = command;
    SetEvent(owner.work);
    return WaitForSingleObject(owner.done, 3000) == WAIT_OBJECT_0;
}
}

int wmain(int argc, wchar_t** argv) {
    if (argc != 2) return 2;
    HWND window = CreateWindowExW(0, L"STATIC", L"CSE unload controller", WS_POPUP,
                                   0, 0, 40, 40, nullptr, nullptr, nullptr, nullptr);
    Owner owner;
    HANDLE thread = CreateThread(nullptr, 0, GuiOwner, &owner, 0, nullptr);
    if (!thread || WaitForSingleObject(owner.done, 3000) != WAIT_OBJECT_0) return 2;
    for (unsigned cycle = 0; cycle < 3; ++cycle) {
        HMODULE module = LoadLibraryW(argv[1]);
        if (!module) return 2;
        const auto start = reinterpret_cast<void (*)()>(GetProcAddress(module, "FixtureStart"));
        const auto original = reinterpret_cast<HCURSOR (*)()>(GetProcAddress(module, "FixtureOriginal"));
        const auto stop = reinterpret_cast<void (*)()>(GetProcAddress(module, "FixtureStop"));
        owner.select = reinterpret_cast<Select>(GetProcAddress(module, "FixtureSelect"));
        if (!start || !original || !stop || !owner.select) return 2;
        start();
        owner.original = original();
        Check(Run(owner, 1), "owner selects cursor");
        Check(owner.displayed && owner.displayed != owner.original, "native DLL replaces cursor");
        stop();
        Check(FreeLibrary(module) != FALSE, "FreeLibrary succeeds");
        Check(GetModuleHandleW(argv[1]) == nullptr, "DLL is unmapped while GUI owner is alive");
        owner.select = nullptr;
        Check(Run(owner, 2) && owner.survived && owner.restored,
              "native cursor survives unloaded DLL and restores without a callback");
        DestroyCursor(owner.original);
        DestroyCursor(owner.displayed);
    }
    owner.command = 0;
    SetEvent(owner.work);
    Check(WaitForSingleObject(thread, 3000) == WAIT_OBJECT_0, "GUI owner exits after unload cycles");
    CloseHandle(thread);
    CloseHandle(owner.work);
    CloseHandle(owner.done);
    if (window) DestroyWindow(window);
    std::printf("native-dll-unload: %s (3 cycles)\n", failures ? "FAIL" : "PASS");
    return failures ? 1 : 0;
}

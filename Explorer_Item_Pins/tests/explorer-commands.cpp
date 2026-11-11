#include "../mods/explorer-item-pins.wh.cpp"

#include <wrl/client.h>
#include <propkey.h>

#include <array>
#include <atomic>
#include <fstream>
#include <iostream>
#include <type_traits>

namespace {
using explorer_item_pins::ApplyPinAction;
using explorer_item_pins::CaptureSingleSelection;
using explorer_item_pins::GetPinAction;
using explorer_item_pins::PinAction;
using explorer_item_pins::PinStore;
using explorer_item_pins::CompareViewItems;
using explorer_item_pins::FindNativeOrdering;
using explorer_item_pins::ReadPinNames;
using explorer_item_pins::SortViewBatch;
using explorer_item_pins::SortWithPins;
using explorer_item_pins::ViewSortBatch;
using explorer_item_pins::NativeGrouping;
using explorer_item_pins::GroupList;
using Microsoft::WRL::ComPtr;
namespace fs = std::filesystem;

void Require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

void Check(HRESULT result, const char* message) {
    if (FAILED(result)) {
        std::cerr << message << " HRESULT=0x" << std::hex << static_cast<unsigned long>(result) << '\n';
        throw std::runtime_error(message);
    }
}

void Pump() {
    MSG message{};
    while (PeekMessageW(&message, nullptr, 0, 0, PM_REMOVE)) {
        TranslateMessage(&message);
        DispatchMessageW(&message);
    }
    Sleep(15);
}

template <typename Predicate>
void WaitUntil(Predicate ready, const char* message) {
    const auto deadline = GetTickCount64() + 4000;
    while (GetTickCount64() < deadline) {
        if (ready()) return;
        Pump();
    }
    throw std::runtime_error(message);
}

class ExplorerView {
public:
    ExplorerView(const fs::path& folder, int expectedItems)
        : window_(CreateWindowExW(WS_EX_TOOLWINDOW, L"STATIC", L"Explorer Item Pins test",
                                  WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, 800, 500,
                                  nullptr, nullptr, GetModuleHandleW(nullptr), nullptr),
                  DestroyWindow) {
        Require(static_cast<bool>(window_), "create the isolated Explorer host");
        Check(CoCreateInstance(CLSID_ExplorerBrowser, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&browser_)),
              "create IExplorerBrowser");
        RECT bounds{};
        GetClientRect(window_.get(), &bounds);
        FOLDERSETTINGS settings{FVM_DETAILS, FWF_NOCLIENTEDGE};
        Check(browser_->Initialize(window_.get(), &bounds, &settings), "initialize IExplorerBrowser");
        Check(browser_->SetOptions(EBO_NOBORDER | EBO_NOPERSISTVIEWSTATE), "set isolated browser options");

        PIDLIST_ABSOLUTE raw = nullptr;
        const auto parsed = SHParseDisplayName(folder.c_str(), nullptr, &raw, 0, nullptr);
        const std::unique_ptr<std::remove_pointer_t<PIDLIST_ABSOLUTE>, decltype(&CoTaskMemFree)>
            pidl(raw, CoTaskMemFree);
        Check(parsed, "parse fixture folder");
        Check(browser_->BrowseToIDList(pidl.get(), SBSP_ABSOLUTE), "open fixture folder");
        WaitUntil([&] {
            int count = 0;
            return SUCCEEDED(browser_->GetCurrentView(IID_PPV_ARGS(view_.ReleaseAndGetAddressOf()))) &&
                   view_ && SUCCEEDED(view_->ItemCount(SVGIO_ALLVIEW, &count)) && count == expectedItems;
        }, "wait for the fixture folder view");
    }

    ~ExplorerView() {
        view_.Reset();
        browser_->Destroy();
    }

    IFolderView2& View() { return *view_.Get(); }

    std::vector<std::wstring> Items() {
        int count = 0;
        Check(view_->ItemCount(SVGIO_ALLVIEW, &count), "get Explorer row count");
        ComPtr<IShellFolder> folder;
        Check(view_->GetFolder(IID_PPV_ARGS(&folder)), "get the current shell folder");
        std::vector<std::wstring> result;
        for (int i = 0; i < count; ++i) {
            PITEMID_CHILD raw = nullptr;
            const auto itemResult = view_->Item(i, &raw);
            const std::unique_ptr<std::remove_pointer_t<PITEMID_CHILD>, decltype(&CoTaskMemFree)>
                item(raw, CoTaskMemFree);
            if (FAILED(itemResult) || !item) return {};
            STRRET display{};
            Check(folder->GetDisplayNameOf(item.get(), SHGDN_INFOLDER | SHGDN_FORPARSING, &display),
                  "get the Explorer item's parsing name");
            wchar_t name[MAX_PATH]{};
            Check(StrRetToBufW(&display, item.get(), name, ARRAYSIZE(name)), "read the Explorer item name");
            result.push_back(fs::path(name).filename().native());
        }
        return result;
    }

    void Select(std::wstring name) {
        ComPtr<IShellFolder> folder;
        Check(view_->GetFolder(IID_PPV_ARGS(&folder)), "get the current shell folder");
        PIDLIST_RELATIVE raw = nullptr;
        const auto parsed = folder->ParseDisplayName(nullptr, nullptr, name.data(), nullptr, &raw, nullptr);
        const std::unique_ptr<std::remove_pointer_t<PIDLIST_RELATIVE>, decltype(&CoTaskMemFree)>
            pidl(raw, CoTaskMemFree);
        Check(parsed, "parse fixture item");
        PCUITEMID_CHILD selected[] = {reinterpret_cast<PCUITEMID_CHILD>(pidl.get())};
        Check(view_->SelectAndPositionItems(1, selected, nullptr, SVSI_SELECT | SVSI_DESELECTOTHERS | SVSI_FOCUSED),
              "select a fixture item in the native view");
        WaitUntil([&] {
            ComPtr<IShellItemArray> selection;
            DWORD count = 0;
            return view_->GetSelection(FALSE, &selection) == S_OK &&
                   SUCCEEDED(selection->GetCount(&count)) && count == 1;
        }, "wait for the native selection");
    }

private:
    std::unique_ptr<std::remove_pointer_t<HWND>, decltype(&DestroyWindow)> window_;
    ComPtr<IExplorerBrowser> browser_;
    ComPtr<IFolderView2> view_;
};

auto Selected(ExplorerView& view, const fs::path& expected) {
    view.Select(expected.filename().native());
    auto selection = CaptureSingleSelection(view.View());
    Require(selection.has_value(), "capture a real filesystem selection from IFolderView2");
    Require(selection->folder == expected.parent_path() && selection->item == expected,
            "selection should identify the actual parent folder and item");
    return *selection;
}

void VerifyCommands(const fs::path& root) {
    const auto source = root / L"source";
    const auto target = root / L"target";
    const auto report = source / L"Отчет.txt";
    const auto renamed = source / L"План.txt";
    const auto photos = source / L"Фото";
    fs::create_directories(photos);
    fs::create_directories(target);
    std::ofstream(report) << "report";
    PinStore store(root / L"data" / L"pins.json");

    {
        ExplorerView view(source, 2);
        Require(!CaptureSingleSelection(view.View()), "an empty selection should not target the open folder");
        const auto file = Selected(view, report);
        Require(GetPinAction(store, file) == PinAction::Pin, "an unpinned selected file should offer Pin");
        Require(ApplyPinAction(store, file, PinAction::Pin), "the Pin command should persist the selected file");
        Require(!ApplyPinAction(store, file, PinAction::Pin), "a stale Pin command should remain idempotent");
        Require(GetPinAction(store, file) == PinAction::Unpin, "a pinned selected file should offer Unpin");
        const auto directory = Selected(view, photos);
        Require(ApplyPinAction(store, directory, PinAction::Pin), "the Pin command should persist a selected subfolder");
        Require(store.ReadFolder(source).size() == 2, "both selected items should have persisted pins");
    }

    fs::rename(report, renamed);
    Require(ReadPinNames(store, source) == explorer_item_pins::PinNames{renamed.filename().native(), photos.filename().native()},
            "the view pin snapshot should follow a same-folder rename");
    {
        ExplorerView view(source, 2);
        const auto file = Selected(view, renamed);
        Require(GetPinAction(store, file) == PinAction::Unpin, "a renamed selected file should still offer Unpin");
        Require(ApplyPinAction(store, file, PinAction::Unpin), "Unpin should remove the old record after a rename");
        Require(!ApplyPinAction(store, file, PinAction::Unpin), "a stale Unpin command should remain idempotent");
        Require(store.ReadFolder(source).size() == 1, "unpinning the file should retain the subfolder's pin");
        Require(ApplyPinAction(store, file, PinAction::Pin), "pin the renamed file again");
    }

    const auto replacement = source / L"replacement.txt";
    std::ofstream(replacement) << "new contents";
    Require(MoveFileExW(replacement.c_str(), renamed.c_str(), MOVEFILE_REPLACE_EXISTING) != FALSE,
            "replace the fixture file with a different filesystem object");
    fs::copy_file(renamed, target / renamed.filename());
    fs::rename(photos, target / photos.filename());
    Require(ReadPinNames(store, source) == explorer_item_pins::PinNames{renamed.filename().native()} &&
            ReadPinNames(store, target).empty(), "the view snapshot should retain replacement and exclude a copy and move");
    {
        ExplorerView view(source, 1);
        Require(GetPinAction(store, Selected(view, renamed)) == PinAction::Unpin,
                "the replacement should retain the pin for that name");
    }
    {
        ExplorerView view(target, 2);
        Require(GetPinAction(store, Selected(view, target / renamed.filename())) == PinAction::Pin,
                "the copy should be unpinned in the destination view");
        Require(GetPinAction(store, Selected(view, target / photos.filename())) == PinAction::Pin,
                "the moved subfolder should be unpinned in the destination view");
    }
}

std::optional<PinStore> orderingStore;
SortViewBatch originalSort = nullptr;
CompareViewItems nativeCompare = nullptr;

BOOL TestSortHook(ViewSortBatch* batch) {
    return SortWithPins(*orderingStore, originalSort, nativeCompare, batch);
}

template <typename Function>
Function InstallTestHook(Function target, Function callback, std::span<const unsigned char> expected,
                         size_t ripLoad = std::dynamic_extent) {
    // This process is isolated from the Explorer session. Install before
    // opening the test view and let process teardown discard the code pages.
    auto* entry = reinterpret_cast<unsigned char*>(target);
    Require(expected.size() >= 14 && std::memcmp(entry, expected.data(), expected.size()) == 0,
            "verify the test trampoline's instruction boundaries");
    const auto jump = [](unsigned char* location, const void* targetAddress) {
        unsigned char instruction[14]{0xff, 0x25};
        std::memcpy(instruction + 6, &targetAddress, sizeof(targetAddress));
        std::memcpy(location, instruction, sizeof(instruction));
    };
    const size_t trampolineSize = expected.size() + 14 + (ripLoad == std::dynamic_extent ? 0 : 6);
    auto* trampoline = static_cast<unsigned char*>(VirtualAlloc(nullptr, trampolineSize,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    Require(trampoline != nullptr, "allocate the isolated test trampoline");
    if (ripLoad == std::dynamic_extent) {
        std::memcpy(trampoline, entry, expected.size());
    } else {
        constexpr unsigned char opcode[]{0x48, 0x8b, 0x05};
        Require(ripLoad + 7 == expected.size() && std::memcmp(entry + ripLoad, opcode, sizeof(opcode)) == 0,
                "verify the RIP-relative load at the end of the test prologue");
        std::int32_t displacement{};
        std::memcpy(&displacement, entry + ripLoad + sizeof(opcode), sizeof(displacement));
        const auto address = reinterpret_cast<std::uintptr_t>(entry + expected.size()) + displacement;
        std::memcpy(trampoline, entry, ripLoad);
        unsigned char load[]{0x48, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0x48, 0x8b, 0x00};
        std::memcpy(load + 2, &address, sizeof(address));
        std::memcpy(trampoline + ripLoad, load, sizeof(load));
    }
    jump(trampoline + trampolineSize - 14, entry + expected.size());
    DWORD protection = 0;
    Require(VirtualProtect(trampoline, trampolineSize, PAGE_EXECUTE_READ, &protection) != FALSE &&
            FlushInstructionCache(GetCurrentProcess(), trampoline, trampolineSize) != FALSE,
            "prepare the isolated test trampoline");
    Require(VirtualProtect(entry, expected.size(), PAGE_EXECUTE_READWRITE, &protection) != FALSE,
            "make the sort entry writable in the test process");
    jump(entry, reinterpret_cast<const void*>(callback));
    DWORD ignored = 0;
    Require(VirtualProtect(entry, expected.size(), protection, &ignored) != FALSE &&
            FlushInstructionCache(GetCurrentProcess(), entry, expected.size()) != FALSE,
            "publish the sort hook in the test process");
    return reinterpret_cast<Function>(trampoline);
}

constexpr unsigned char sortPrologue[]{0x48, 0x8b, 0xc4, 0x48, 0x89, 0x58, 0x08, 0x48,
                                       0x89, 0x68, 0x10, 0x48, 0x89, 0x70, 0x18};

struct Layout { FOLDERVIEWMODE mode; int size; };
constexpr std::array layouts{Layout{FVM_DETAILS, 16}, Layout{FVM_LIST, 16}, Layout{FVM_TILE, 48},
                             Layout{FVM_CONTENT, 32}, Layout{FVM_SMALLICON, 16}, Layout{FVM_ICON, 48},
                             Layout{FVM_ICON, 96}, Layout{FVM_ICON, 256}};

void SetLayout(ExplorerView& view, const Layout& layout) {
    Check(view.View().SetViewModeAndIconSize(layout.mode, layout.size), "switch the native view layout");
    FOLDERVIEWMODE actualMode{};
    int actualSize = 0;
    Check(view.View().GetViewModeAndIconSize(&actualMode, &actualSize), "read the native view layout");
    Require(actualMode == layout.mode && actualSize == layout.size, "the requested view layout should be active");
}

void VerifyOrdering(const fs::path& root) {
    const auto folder = root / L"ordering";
    constexpr int itemCount = 6;
    const std::array<std::wstring_view, itemCount> names{L"10 folder", L"20.txt", L"30.txt",
                                                       L"60.txt", L"80 pinned folder", L"90 pinned.txt"};
    fs::create_directories(folder / names.front());
    fs::create_directories(folder / names[4]);
    const auto now = fs::file_time_type::clock::now();
    for (size_t i = 0; i < names.size(); ++i) {
        const auto path = folder / names[i];
        if (!fs::is_directory(path)) std::ofstream(path) << i;
        fs::last_write_time(path, now - std::chrono::hours(1 + 2 * i));
    }

    const auto storage = LoadLibraryExW(L"Windows.Storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    Check(storage ? S_OK : HRESULT_FROM_WIN32(GetLastError()), "load Windows.Storage.dll in the isolated test process");
    const auto native = FindNativeOrdering(storage);
    if (!native) {
        std::cout << "SKIP native sort hooks on this Windows.Storage.dll build\n";
        return;
    }
    const auto& store = orderingStore.emplace(root / L"ordering-data" / L"pins.json");
    nativeCompare = native->compare;
    originalSort = InstallTestHook(native->sort, TestSortHook, sortPrologue);
    ExplorerView ascending(folder, itemCount);
    ExplorerView descending(folder, itemCount);

    const auto setSort = [](ExplorerView& view, const PROPERTYKEY& key, SORTDIRECTION direction) {
        const SORTCOLUMN column{key, direction};
        Check(view.View().SetSortColumns(&column, 1), "set the native view's sort direction");
    };
    const auto settle = [] {
        const auto deadline = GetTickCount64() + 160;
        while (GetTickCount64() < deadline) Pump();
    };
    const auto isPinned = [&](std::wstring_view name) { return name == names[4] || name == names[5]; };
    int verified = 0;
    for (const auto& layout : layouts) {
        SetLayout(ascending, layout);
        SetLayout(descending, layout);
        for (const auto& property : {PKEY_ItemNameDisplay, PKEY_DateModified}) {
            setSort(ascending, property, SORT_ASCENDING);
            setSort(descending, property, SORT_DESCENDING);
            settle();
            auto expectedAscending = ascending.Items();
            auto expectedDescending = descending.Items();
            Require(expectedAscending.size() == itemCount && expectedDescending.size() == itemCount &&
                    expectedAscending != expectedDescending, "record distinct stock ascending and descending orders");
            std::ranges::stable_partition(expectedAscending, isPinned);
            std::ranges::stable_partition(expectedDescending, isPinned);

            for (size_t pin : {4u, 5u}) {
                Require(store.SetPinned(folder, {std::wstring(names[pin]), {}}, true), "save an ordering fixture pin");
            }
            const auto refresh = [](ExplorerView& view) {
                ComPtr<IShellView> shellView;
                Check(view.View().QueryInterface(IID_PPV_ARGS(&shellView)), "get the native shell view");
                Check(shellView->Refresh(), "refresh the native sort after changing pins");
            };
            refresh(ascending);
            refresh(descending);
            try {
                WaitUntil([&] { return ascending.Items() == expectedAscending && descending.Items() == expectedDescending; },
                          "the two Explorer views should pin first and keep their own stock order");
            } catch (...) {
                const auto print = [](std::wstring_view label, const auto& rows) {
                    std::wcerr << label;
                    for (const auto& row : rows) std::wcerr << L" [" << row << L']';
                    std::wcerr << L'\n';
                };
                print(L"Expected ascending", expectedAscending);
                print(L"Observed ascending", ascending.Items());
                print(L"Expected descending", expectedDescending);
                print(L"Observed descending", descending.Items());
                throw;
            }
            verified += 2;
            for (size_t pin : {4u, 5u}) {
                Require(store.SetPinned(folder, {std::wstring(names[pin]), {}}, false), "clear the ordering fixture pin");
            }
            refresh(ascending);
            refresh(descending);
            settle();
        }
    }
    std::cout << "PASS pin priority with stock order in " << verified << " native view scenarios\n";
}

NativeGrouping groupingApi{};
std::atomic<unsigned> pinnedHeaders{};
std::atomic<unsigned> topPinnedGroups{};

HRESULT TestGroupBatch(const unsigned char* batch, void* result) {
    return explorer_item_pins::GroupWithPins(*orderingStore, groupingApi.batch, batch, result);
}

HRESULT TestAssignGroup(IUnknown* category, IUnknown* row, void* item, void* context, DWORD flags) {
    return explorer_item_pins::AssignWithPins(groupingApi, category, row, item, context, flags);
}

HRESULT TestGroupInfo(explorer_item_pins::GroupInfo original, IUnknown* category, DWORD id, IUnknown** result) {
    const auto hr = explorer_item_pins::GroupInfoWithPins(groupingApi.make, original, category, id, result);
    if (SUCCEEDED(hr) && *result && id == explorer_item_pins::pinnedGroupId) ++pinnedHeaders;
    return hr;
}

HRESULT TestRangeInfo(IUnknown* category, DWORD id, IUnknown** result) {
    return TestGroupInfo(groupingApi.rangeInfo, category, id, result);
}

HRESULT TestTextInfo(IUnknown* category, DWORD id, IUnknown** result) {
    return TestGroupInfo(groupingApi.textInfo, category, id, result);
}

HRESULT ObserveAppliedGroups(const unsigned char* view, GroupList* list, UINT* added) {
    UINT count = 0;
    bool pinnedFirst = false;
    if (list && SUCCEEDED(list->GetCount(&count)) && count) {
        const auto settings = *reinterpret_cast<IUnknown* const*>(view + 0x268);
        if (settings) {
            const auto first = groupingApi.ascending(settings) ? 0 : count - 1;
            pinnedFirst = explorer_item_pins::NativeGroupId(*list, first) == explorer_item_pins::pinnedGroupId;
        }
    }
    const auto hr = groupingApi.apply(view, list, added);
    if (SUCCEEDED(hr) && pinnedFirst) ++topPinnedGroups;
    return hr;
}

HRESULT TestApplyGroups(const unsigned char* view, GroupList* list, UINT* added) {
    auto observed = groupingApi;
    observed.apply = ObserveAppliedGroups;
    return explorer_item_pins::ApplyWithPins(observed, view, list, added);
}

HRESULT TestSetGrouping(IFolderView2* view, REFPROPERTYKEY key, BOOL ascending) {
    return explorer_item_pins::SetGroupingWithPins(*orderingStore, groupingApi.setGrouping, view, key, ascending);
}

void InstallTestGrouping(const NativeGrouping& native) {
    constexpr unsigned char batch[]{0x48, 0x89, 0x5c, 0x24, 0x18, 0x48, 0x89, 0x74, 0x24, 0x20, 0x55, 0x57, 0x41, 0x54};
    constexpr unsigned char assign[]{0x40, 0x53, 0x55, 0x56, 0x57, 0x48, 0x83, 0xec, 0x58, 0x48, 0x8b, 0x05, 0xc0, 0x73, 0x6d, 0x00};
    constexpr unsigned char range[]{0x48, 0x89, 0x5c, 0x24, 0x20, 0x55, 0x56, 0x57, 0x48, 0x83, 0xec, 0x50, 0x48, 0x8b, 0x05, 0x1d, 0xb9, 0x60, 0x00};
    constexpr unsigned char strings[]{0x48, 0x89, 0x5c, 0x24, 0x08, 0x48, 0x89, 0x74, 0x24, 0x10, 0x57, 0x48, 0x83, 0xec, 0x40};
    constexpr unsigned char apply[]{0x40, 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8b, 0xec};
    constexpr unsigned char set[]{0x48, 0x8b, 0x89, 0x60, 0x05, 0x00, 0x00, 0x48, 0x8b, 0x01, 0x48, 0x8b, 0x40, 0x20};
    groupingApi = native;
    groupingApi.batch = InstallTestHook(native.batch, TestGroupBatch, batch);
    groupingApi.assign = InstallTestHook(native.assign, TestAssignGroup, assign, 9);
    groupingApi.rangeInfo = InstallTestHook(native.rangeInfo, TestRangeInfo, range, 12);
    groupingApi.textInfo = InstallTestHook(native.textInfo, TestTextInfo, strings);
    groupingApi.apply = InstallTestHook(native.apply, TestApplyGroups, apply);
    groupingApi.setGrouping = InstallTestHook(native.setGrouping, TestSetGrouping, set);
}

void VerifyGrouping(const fs::path& root) {
    const auto storage = LoadLibraryExW(L"Windows.Storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    Check(storage ? S_OK : HRESULT_FROM_WIN32(GetLastError()), "load Windows.Storage.dll in the isolated test process");
    const auto shell = GetModuleHandleW(L"Shell32.dll");
    const auto grouping = explorer_item_pins::FindNativeGrouping(storage, shell);
    const auto ordering = FindNativeOrdering(storage);
    if (!grouping || !ordering) {
        std::cout << "SKIP native grouping hooks on these Windows shell images\n";
        return;
    }
    const auto& store = orderingStore.emplace(root / L"grouping-data" / L"pins.json");
    nativeCompare = ordering->compare;
    originalSort = InstallTestHook(ordering->sort, TestSortHook, sortPrologue);
    InstallTestGrouping(*grouping);

    const std::array<std::wstring_view, 5> names{L"Alpha folder", L"Beta.txt", L"Gamma.zip",
                                                 L"Omega pinned folder", L"Zeta pinned.txt"};
    const auto stock = root / L"stock";
    const auto pinned = root / L"pinned";
    const auto now = fs::file_time_type::clock::now();
    for (const auto& folder : {stock, pinned}) {
        fs::create_directories(folder / names[0]);
        fs::create_directories(folder / names[3]);
        for (size_t i = 0; i < names.size(); ++i) {
            const auto path = folder / names[i];
            if (!fs::is_directory(path)) {
                std::ofstream(path) << i;
                fs::resize_file(path, 1024ULL << (2 * i));
            }
            fs::last_write_time(path, now - std::chrono::days(i));
        }
    }
    for (size_t i : {3u, 4u}) {
        Require(store.SetPinned(pinned, {std::wstring(names[i]), {}}, true), "persist a grouping fixture pin");
    }
    ExplorerView reference(stock, names.size());
    ExplorerView modified(pinned, names.size());

    const auto isPinned = [&](std::wstring_view name) { return name == names[3] || name == names[4]; };
    int passed = 0;
    for (const auto& layout : layouts) {
        SetLayout(reference, layout);
        SetLayout(modified, layout);
        for (const auto& key : {PKEY_ItemNameDisplay, PKEY_DateModified, PKEY_ItemTypeText, PKEY_Size,
                                PKEY_DateCreated, PKEY_FileExtension}) {
            for (BOOL ascending : {TRUE, FALSE, TRUE}) {
                const auto before = pinnedHeaders.load();
                const auto previouslyApplied = topPinnedGroups.load();
                Check(reference.View().SetGroupBy(key, ascending), "group the stock view");
                Check(modified.View().SetGroupBy(key, ascending), "group the pinned view");
                WaitUntil([&] {
                    if (pinnedHeaders.load() == before || topPinnedGroups.load() == previouslyApplied) return false;
                    auto actual = modified.Items();
                    auto expected = reference.Items();
                    if (actual.size() != names.size() || expected.size() != names.size() ||
                        !isPinned(actual[0]) || !isPinned(actual[1])) return false;
                    std::erase_if(actual, isPinned);
                    std::erase_if(expected, isPinned);
                    return actual == expected;
                }, "place the pinned group above the unmodified native groups in either direction");
                ++passed;
            }
        }
    }
    std::cout << "PASS pinned group and stock order in " << passed << " native view scenarios\n";
}
}

int wmain(int argc, wchar_t** argv) {
    if (argc != 3) return 2;
    if (FAILED(OleInitialize(nullptr))) return 3;
    int result = 0;
    try {
        const std::wstring_view mode(argv[2]);
        if (mode == L"commands") {
            VerifyCommands(argv[1]);
            std::cout << "PASS native Explorer selection, pin and unpin, rename, replacement, copy and move\n";
        } else if (mode == L"ordering") {
            VerifyOrdering(argv[1]);
        } else if (mode == L"grouping") {
            VerifyGrouping(argv[1]);
        } else {
            result = 2;
        }
    } catch (const std::exception& error) {
        std::cerr << "FAIL " << error.what() << '\n';
        result = 1;
    }
    OleUninitialize();
    return result;
}

#include "../Mod/explorer-item-pins.wh.cpp"

#include <wrl/client.h>
#include <propkey.h>

#include <array>
#include <atomic>
#include <chrono>
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
using explorer_item_pins::ReadItem;
using explorer_item_pins::NativeGrouping;
using explorer_item_pins::GroupList;
using Microsoft::WRL::ComPtr;
namespace fs = std::filesystem;

using explorer_item_pins::SetGrouping;
using explorer_item_pins::GroupBatch;
using explorer_item_pins::AssignGroup;
using explorer_item_pins::GroupInfo;
using explorer_item_pins::ApplyGroups;
using explorer_item_pins::RegisterGroup;
using explorer_item_pins::MakeGroup;
using explorer_item_pins::DrawItemImage;
using explorer_item_pins::PaintFolderImage;

// The isolated x64 hook tests verify the image identity before using fixture addresses.
constexpr GUID storagePdb{0x6ebed4d4, 0x5734, 0x9fc4, {0x9e, 0x3e, 0x8d, 0xe5, 0x0b, 0x33, 0x0f, 0x0f}};
constexpr GUID shellPdb{0x617696ae, 0x77ca, 0x0846, {0x74, 0x3e, 0x0c, 0x34, 0x54, 0xe1, 0x7c, 0x05}};
constexpr GUID storagePdb9974{0x63a99e08, 0xa6bc, 0x71de, {0x16, 0x8a, 0xd4, 0xf5, 0x4b, 0x00, 0x12, 0x59}};
constexpr GUID shellPdb9974{0x64708141, 0x4aa5, 0xc991, {0xb8, 0xa9, 0x4f, 0x15, 0xc5, 0xba, 0x60, 0x02}};

unsigned char* CheckedImage(HMODULE module, const GUID& expectedPdb, DWORD lastRva) {
    if (!module) return nullptr;
    auto* image = reinterpret_cast<unsigned char*>(module);
    const auto* dos = reinterpret_cast<const IMAGE_DOS_HEADER*>(image);
    const auto* pe = reinterpret_cast<const IMAGE_NT_HEADERS64*>(image + dos->e_lfanew);
    if (pe->FileHeader.Machine != IMAGE_FILE_MACHINE_AMD64) return nullptr;

    const auto& debug = pe->OptionalHeader.DataDirectory[IMAGE_DIRECTORY_ENTRY_DEBUG];
    const auto size = pe->OptionalHeader.SizeOfImage;
    if (!debug.VirtualAddress || debug.VirtualAddress >= size || debug.Size > size - debug.VirtualAddress ||
        lastRva >= size) return nullptr;

    const auto* entries = reinterpret_cast<const IMAGE_DEBUG_DIRECTORY*>(image + debug.VirtualAddress);
    for (DWORD i = 0; i < debug.Size / sizeof(*entries); ++i) {
        const auto& entry = entries[i];
        constexpr DWORD codeViewHeaderSize = 24;
        if (entry.Type != IMAGE_DEBUG_TYPE_CODEVIEW || !entry.AddressOfRawData ||
            entry.AddressOfRawData >= size || entry.SizeOfData < codeViewHeaderSize ||
            entry.SizeOfData > size - entry.AddressOfRawData) continue;
        const auto* data = image + entry.AddressOfRawData;
        DWORD age = 0;
        std::memcpy(&age, data + 20, sizeof(age));
        if (age == 1 && std::memcmp(data, "RSDS", 4) == 0 &&
            std::memcmp(data + 4, &expectedPdb, sizeof(expectedPdb)) == 0) return image;
    }
    return nullptr;
}

struct OrderingFixture {
    explorer_item_pins::RunViewSort sort;
    CompareViewItems compare;
    explorer_item_pins::GetComparedItem getItem;
};

std::optional<OrderingFixture> FindNativeOrdering(HMODULE storage) {
    auto image = CheckedImage(storage, storagePdb9974, 0x1a6da0);
    if (!image) return std::nullopt;
    return OrderingFixture{reinterpret_cast<explorer_item_pins::RunViewSort>(image + 0x1537e8),
                           reinterpret_cast<CompareViewItems>(image + 0x155190),
                           reinterpret_cast<explorer_item_pins::GetComparedItem>(image + 0x1a6da0)};
}

[[maybe_unused]] SetGrouping FindKnownSetGrouping(HMODULE shell) {
    constexpr DWORD oldRva = 0x2e8ae0;
    constexpr DWORD updatedRva = 0x2e8e50;
    if (const auto image = CheckedImage(shell, shellPdb, oldRva)) {
        return reinterpret_cast<SetGrouping>(image + oldRva);
    }
    if (const auto image = CheckedImage(shell, shellPdb9974, updatedRva)) {
        return reinterpret_cast<SetGrouping>(image + updatedRva);
    }
    return nullptr;
}

[[maybe_unused]] std::optional<NativeGrouping> FindNativeGrouping(HMODULE storage, SetGrouping setGrouping) {
    const auto updated = CheckedImage(storage, storagePdb9974, 0x5bcf00);
    const auto image = updated;
    if (!image || !setGrouping) return std::nullopt;
    return NativeGrouping{
        reinterpret_cast<GroupBatch>(image + 0x153e50), reinterpret_cast<AssignGroup>(image + 0x1641f0),
        reinterpret_cast<GroupInfo>(image + (updated ? 0x22fcf0 : 0x22fc90)), reinterpret_cast<GroupInfo>(image + 0x16f0c0),
        reinterpret_cast<ApplyGroups>(image + 0x16eab8), setGrouping,
        reinterpret_cast<RegisterGroup>(image + 0x1642dc), reinterpret_cast<MakeGroup>(image + (updated ? 0x22fdac : 0x22fd4c)),
        image + (updated ? 0x248790 : 0x248730),
        image + (updated ? 0x5bcf00 : 0x5bcb30),
        reinterpret_cast<explorer_item_pins::ReadGroupValue>(image + 0x154390),
        reinterpret_cast<explorer_item_pins::WriteGroupValue>(image + 0x294510),
        reinterpret_cast<explorer_item_pins::GetGroupAt>(image + 0x2cec10),
    };
}

constexpr GUID framePdb{0x468742f3, 0xd41c, 0x3441, {0xa5, 0x02, 0x2f, 0xbf, 0xfe, 0xf9, 0xa6, 0x56}};
constexpr GUID framePdb9974{0xf8886b4f, 0x52b8, 0x73ab, {0x4a, 0x57, 0x43, 0xf3, 0x89, 0x20, 0x9a, 0xce}};

[[maybe_unused]] DrawItemImage FindKnownImageRenderer(HMODULE storage) {
    constexpr DWORD draw = 0x16d730;
    auto image = CheckedImage(storage, storagePdb, draw);
    if (!image) image = CheckedImage(storage, storagePdb9974, draw);
    return image ? reinterpret_cast<DrawItemImage>(image + draw) : nullptr;
}

[[maybe_unused]] PaintFolderImage FindKnownFolderImagePaint(HMODULE frame) {
    constexpr DWORD paint = 0xbde30;
    auto image = CheckedImage(frame, framePdb, paint);
    if (!image) image = CheckedImage(frame, framePdb9974, paint);
    return image ? reinterpret_cast<PaintFolderImage>(image + paint) : nullptr;
}

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
void WaitUntil(Predicate ready, const char* message, ULONGLONG timeoutMs = 4000) {
    const auto deadline = GetTickCount64() + timeoutMs;
    while (GetTickCount64() < deadline) {
        if (ready()) return;
        Pump();
    }
    throw std::runtime_error(message);
}

explorer_item_pins::PinNames ReadPinNames(std::span<const explorer_item_pins::Item> pins, const fs::path& folder) {
    const auto exactNames = explorer_item_pins::ReadExactPinNames(pins, folder);
    if (exactNames.size() == pins.size()) return exactNames;
    return explorer_item_pins::ResolvePinAliases(pins, folder, exactNames);
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
    HWND Window() const { return window_.get(); }

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
    Require(ReadPinNames(store.ReadFolder(source), source) == explorer_item_pins::PinNames{renamed.filename().native(), photos.filename().native()},
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
    Require(ReadPinNames(store.ReadFolder(source), source) == explorer_item_pins::PinNames{renamed.filename().native()} &&
            ReadPinNames(store.ReadFolder(target), target).empty(), "the view snapshot should retain replacement and exclude a copy and move");
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
explorer_item_pins::IndicatorPins orderingPins;
explorer_item_pins::RunViewSort originalSort = nullptr;
CompareViewItems originalCompare = nullptr;
explorer_item_pins::GetComparedItem originalGetItem = nullptr;

explorer_item_pins::PinNamesSnapshot TestFolderPins(const fs::path& folder) {
    return orderingPins.Snapshot(*orderingStore, folder);
}

void WaitForFolderPins(const fs::path& folder, const explorer_item_pins::PinNames& expected) {
    orderingPins.Invalidate(folder);
    WaitUntil([&] {
        const auto names = TestFolderPins(folder);
        return names && *names == expected && orderingPins.Cached(folder / L"probe").has_value();
    }, "resolve the expected folder pins off the calling thread");
}

void VerifyFolderSnapshots(const fs::path& root) {
    const auto folder = root / L"snapshot-folder";
    fs::create_directories(folder);
    const auto& store = orderingStore.emplace(root / L"snapshot-data" / L"pins.json");
    const auto missing = folder / L"Deleted.txt";
    const auto current = folder / L"Before rename.txt";
    const auto renamed = folder / L"After rename.txt";
    std::ofstream(missing) << "deleted pin";
    std::ofstream(current) << "renamed pin";
    for (const auto& path : {missing, current}) {
        Require(store.SetPinned(folder, ReadItem(path), true), "persist a folder snapshot fixture pin");
    }
    fs::remove(missing);

    Require(!TestFolderPins(folder), "the first folder lookup should schedule work and return immediately");
    WaitForFolderPins(folder, {current.filename().native()});
    const auto snapshot = TestFolderPins(folder);
    WaitForFolderPins(folder, {current.filename().native()});
    Require(TestFolderPins(folder) == snapshot, "reuse a negative identity lookup for an unchanged folder");

    const auto unchangedTime = fs::last_write_time(folder);
    std::ofstream(missing) << "replacement at the pinned name";
    fs::last_write_time(folder, unchangedTime);
    Require(fs::last_write_time(folder) == unchangedTime, "restore the original directory timestamp");
    WaitUntil([&] {
        const auto names = TestFolderPins(folder);
        return names && names->contains(missing.filename().native());
    }, "restore a pin when its exact name returns without a directory timestamp change");
    fs::remove(missing);

    fs::rename(current, renamed);
    fs::last_write_time(folder, fs::file_time_type::clock::now() + std::chrono::seconds(2));
    WaitForFolderPins(folder, {renamed.filename().native()});

    const auto sameTime = fs::last_write_time(folder);
    const auto renamedAgain = folder / L"Renamed without timestamp change.txt";
    fs::rename(renamed, renamedAgain);
    fs::last_write_time(folder, sameTime);
    Require(fs::last_write_time(folder) == sameTime, "preserve the directory timestamp after renaming an alias");
    WaitForFolderPins(folder, {});
    WaitUntil([&] {
        const auto names = TestFolderPins(folder);
        return names && *names == explorer_item_pins::PinNames{renamedAgain.filename().native()};
    }, "retry the identity lookup even when the directory timestamp stays the same", 8000);
    fs::rename(renamedAgain, renamed);
    fs::last_write_time(folder, sameTime + std::chrono::seconds(2));
    orderingPins.Invalidate(folder);
    TestFolderPins(folder);
    std::optional<bool> pinnedAfterChange;
    WaitUntil([&] {
        pinnedAfterChange = orderingPins.Cached(renamed);
        return pinnedAfterChange.has_value();
    }, "complete the first background refresh after the directory timestamp changes");
    Require(*pinnedAfterChange, "the first refresh after a timestamp change must resolve the renamed pin");
    std::cout << "PASS asynchronous snapshots, same-name restoration and identity retries for changed and unchanged timestamps\n";

    const auto busyFolder = root / L"busy-folder";
    const auto heldFolder = root / L"held-files";
    fs::create_directories(busyFolder);
    fs::create_directories(heldFolder);
    const auto originalPin = busyFolder / L"Pinned.txt";
    const auto heldPin = heldFolder / originalPin.filename();
    const auto returnedPin = busyFolder / L"Returned under a new name.txt";
    std::ofstream(originalPin) << "pin that may return";
    Require(store.SetPinned(busyFolder, ReadItem(originalPin), true), "persist a pin for the busy folder");
    fs::rename(originalPin, heldPin);
    WaitForFolderPins(busyFolder, {});
    const auto busyTime = fs::last_write_time(busyFolder) + std::chrono::seconds(2);
    fs::last_write_time(busyFolder, busyTime);
    WaitForFolderPins(busyFolder, {});

    fs::rename(heldPin, returnedPin);
    fs::last_write_time(busyFolder, busyTime + std::chrono::seconds(2));
    orderingPins.Invalidate(busyFolder);
    TestFolderPins(busyFolder);
    WaitUntil([&] { return orderingPins.Cached(returnedPin).has_value(); }, "refresh the busy folder snapshot");
    Require(orderingPins.Cached(returnedPin) == false,
            "avoid an immediate third full scan after repeated misses even if the folder timestamp changes");
    WaitUntil([&] { TestFolderPins(busyFolder); return orderingPins.Cached(returnedPin) == true; },
              "retry the deferred scan and restore the pin by ID", 15000);
    fs::rename(returnedPin, originalPin);
    fs::last_write_time(busyFolder, busyTime + std::chrono::seconds(2));
    WaitForFolderPins(busyFolder, {originalPin.filename().native()});
    std::cout << "PASS repeated misses defer full scans and preserve pin restoration\n";

    const auto exactFolder = root / L"cold-view";
    fs::create_directories(exactFolder);
    const auto exact = exactFolder / L"Pinned.txt";
    std::ofstream(exact) << "pinned";
    Require(store.SetPinned(exactFolder, ReadItem(exact), true), "persist a first-open fixture pin");
    store.PrimeCache();
    explorer_item_pins::IndicatorPins firstOpen;
    const auto initial = firstOpen.Snapshot(store, exactFolder);
    Require(initial && initial->contains(exact.filename().native()),
            "use the preloaded name on the first snapshot request");
    WaitUntil([&] { return firstOpen.Cached(exact) == true; }, "validate the first-open pin in the background");

    const auto identityFolder = root / L"directory-identities";
    fs::create_directories(identityFolder);
    const auto original = identityFolder / L"Original.txt";
    const auto alias = identityFolder / L"Renamed.txt";
    const auto hardLink = identityFolder / L"Hard link.txt";
    std::ofstream(original) << "identity";
    const std::array records{ReadItem(original)};
    fs::rename(original, alias);
    for (int i = 0; i < 600; ++i) {
        std::ofstream(identityFolder / (L"Ordinary " + std::to_wstring(i) + L".txt")) << i;
    }
    std::vector<explorer_item_pins::Item> entries;
    Require(explorer_item_pins::ReadDirectoryIdentities(identityFolder, records, entries) && entries.size() == 601,
            "read a directory spanning multiple batches");
    const auto candidate = std::ranges::find(entries, alias.filename().native(), &explorer_item_pins::Item::name);
    Require(candidate != entries.end() && candidate->identity == records.front().identity && !candidate->identity.empty(),
            "the batched directory IDs must find the same rename as the handle lookup");
    Require(ReadPinNames(records, identityFolder) == explorer_item_pins::PinNames{alias.filename().native()},
            "resolve a unique rename using batched IDs");
    fs::create_hard_link(alias, hardLink);
    Require(ReadPinNames(records, identityFolder).empty(), "multiple aliases remain ambiguous");
    std::ofstream(original) << "replacement";
    Require(ReadPinNames(records, identityFolder) == explorer_item_pins::PinNames{original.filename().native()},
            "a replacement at the original name wins over aliases");
    std::cout << "PASS first-open pin snapshot and multi-batch rename, hard-link and replacement semantics\n";

    const auto recent = root / L"cache-a";
    const auto oldest = root / L"cache-z";
    WaitForFolderPins(oldest, {});
    for (int i = 0; i < 60; ++i) {
        WaitForFolderPins(root / (L"cache-m" + std::to_wstring(i)), {});
    }
    WaitForFolderPins(recent, {});
    WaitForFolderPins(folder, {renamed.filename().native()});
    WaitForFolderPins(busyFolder, {originalPin.filename().native()});
    const auto beforeEviction = TestFolderPins(recent);
    WaitForFolderPins(root / L"cache-new", {});
    Require(TestFolderPins(recent) == beforeEviction && !TestFolderPins(oldest),
            "retain recently used snapshots and evict the oldest idle snapshot");
    std::cout << "PASS folder snapshot eviction follows recency\n";

    std::atomic_bool completed = false;
    const bool submitted = explorer_item_pins::QueueModuleWork([&] { completed = true; });
    explorer_item_pins::moduleWork.Stop();
    Require(submitted && completed && !explorer_item_pins::QueueModuleWork([] {}),
            "drain module work and reject new submissions during unload");
    std::cout << "PASS background work drained on unload\n";
}

BOOL TestSortHook(void* request) {
    return explorer_item_pins::RunSortWithPins(TestFolderPins, originalSort, request);
}

int TestCompare(const void* left, const void* right, void* context) {
    return explorer_item_pins::CompareWithPins(originalCompare, left, right, context);
}

HRESULT TestGetItem(IUnknown* store, const void* key, IShellFolder* folder, void* viewState, REFIID iid, void** output) {
    return explorer_item_pins::GetComparedItemWithPins(originalGetItem, store, key, folder, viewState, iid, output);
}

template <typename Function>
Function InstallTestHook(Function target, Function callback, std::span<const unsigned char> expected,
                         size_t relativeOperand = std::dynamic_extent) {
    // This process is isolated from the Explorer session. Install before
    // opening the test view and let process teardown discard the code pages.
    auto* entry = reinterpret_cast<unsigned char*>(target);
    const bool relative = relativeOperand != std::dynamic_extent;
    const bool relativeCall = relative && expected[relativeOperand] == 0xe8;
    Require(expected.size() >= 14 && (!relative || relativeOperand + (relativeCall ? 5 : 7) == expected.size()),
            "verify the test trampoline's prologue length");
    // The last four bytes of a RIP-relative load are a displacement that varies by image.
    const auto fixedBytes = expected.size() - (relative ? sizeof(std::int32_t) : 0);
    Require(std::memcmp(entry, expected.data(), fixedBytes) == 0,
            "verify the test trampoline's instruction boundaries");
    const auto jump = [](unsigned char* location, const void* targetAddress) {
        unsigned char instruction[14]{0xff, 0x25};
        std::memcpy(instruction + 6, &targetAddress, sizeof(targetAddress));
        std::memcpy(location, instruction, sizeof(instruction));
    };
    const size_t trampolineSize = expected.size() + 14 + (relativeCall ? 7 : relative ? 6 : 0);
    auto* trampoline = static_cast<unsigned char*>(VirtualAlloc(nullptr, trampolineSize,
        MEM_COMMIT | MEM_RESERVE, PAGE_READWRITE));
    Require(trampoline != nullptr, "allocate the isolated test trampoline");
    if (!relative) {
        std::memcpy(trampoline, entry, expected.size());
    } else if (relativeCall) {
        std::int32_t displacement{};
        std::memcpy(&displacement, entry + relativeOperand + 1, sizeof(displacement));
        const auto address = reinterpret_cast<std::uintptr_t>(entry + expected.size()) + displacement;
        std::memcpy(trampoline, entry, relativeOperand);
        unsigned char call[]{0x48, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0xff, 0xd0};
        std::memcpy(call + 2, &address, sizeof(address));
        std::memcpy(trampoline + relativeOperand, call, sizeof(call));
    } else {
        const auto opcode = entry + relativeOperand;
        Require(opcode[0] == 0x48 && (opcode[1] == 0x8b || opcode[1] == 0x8d) && opcode[2] == 0x05,
                "verify the RIP-relative operand at the end of the test prologue");
        std::int32_t displacement{};
        std::memcpy(&displacement, opcode + 3, sizeof(displacement));
        const auto address = reinterpret_cast<std::uintptr_t>(entry + expected.size()) + displacement;
        std::memcpy(trampoline, entry, relativeOperand);
        unsigned char load[]{0x48, 0xb8, 0, 0, 0, 0, 0, 0, 0, 0, 0x48, 0x8b, 0x00};
        if (opcode[1] == 0x8d) std::fill(load + 10, load + 13, 0x90);
        std::memcpy(load + 2, &address, sizeof(address));
        std::memcpy(trampoline + relativeOperand, load, sizeof(load));
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

void InstallTestOrdering(const OrderingFixture& native) {
    constexpr unsigned char compare[]{0x48, 0x89, 0x5c, 0x24, 0x20, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56};
    constexpr unsigned char getItem[]{0x40, 0x53, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56, 0x41, 0x57, 0x48, 0x83, 0xec, 0x58};
    originalGetItem = InstallTestHook(native.getItem, TestGetItem, getItem);
    originalCompare = InstallTestHook(native.compare, TestCompare, compare);
    originalSort = InstallTestHook(native.sort, TestSortHook, sortPrologue);
}

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
    Require(native.has_value(), "native sort requires a verified Windows.Storage.dll image");
    const auto& store = orderingStore.emplace(root / L"ordering-data" / L"pins.json");
    InstallTestOrdering(*native);
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
            WaitForFolderPins(folder, {std::wstring(names[4]), std::wstring(names[5])});
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
            WaitForFolderPins(folder, {});
            refresh(ascending);
            refresh(descending);
            settle();
        }
    }
    std::cout << "PASS pin priority with stock order in " << verified << " native view scenarios\n";
}

NativeGrouping groupingApi{};
explorer_item_pins::GetGroupAt originalGroupAt = nullptr;
explorer_item_pins::SetGrouping testSetGroupingTarget = nullptr;
std::atomic<unsigned> pinnedHeaders{};
std::atomic<unsigned> topPinnedGroups{};

HRESULT TestGroupBatch(const unsigned char* batch, void* result) {
    return explorer_item_pins::GroupWithPins(TestFolderPins, groupingApi.batch, batch, result);
}

HRESULT TestAssignGroup(IUnknown* category, IUnknown* row, REFPROPERTYKEY property, IShellFolder2* context, DWORD flags) {
    return explorer_item_pins::AssignWithPins(groupingApi, category, row, property, context, flags);
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

struct GroupObservation {
    GroupList* array;
    bool applying = false;
    bool firstRead = false;
    bool pinnedFirst = false;
};
thread_local GroupObservation* groupObservation = nullptr;

HRESULT TestGetGroupAt(GroupList* list, UINT index, IUnknown** group) {
    const auto result = originalGroupAt(list, explorer_item_pins::GroupOrder::Index(list, index), group);
    const auto observation = groupObservation;
    if (observation && observation->applying && list == observation->array && !observation->firstRead &&
        SUCCEEDED(result) && *group) {
        observation->firstRead = true;
        observation->pinnedFirst = explorer_item_pins::IsPinnedGroup(**group);
    }
    return result;
}

HRESULT ObserveAppliedGroups(const unsigned char* view, GroupList* list, UINT* added) {
    Require(groupObservation && list == groupObservation->array, "pass Explorer's original COM array to the native apply method");
    groupObservation->applying = true;
    const auto hr = groupingApi.apply(view, list, added);
    if (SUCCEEDED(hr) && groupObservation->pinnedFirst) ++topPinnedGroups;
    return hr;
}

HRESULT TestApplyGroups(const unsigned char* view, GroupList* list, UINT* added) {
    auto observed = groupingApi;
    observed.apply = ObserveAppliedGroups;
    GroupObservation observation{list};
    auto previous = std::exchange(groupObservation, &observation);
    const auto result = explorer_item_pins::ApplyWithPins(observed, view, list, added);
    groupObservation = previous;
    return result;
}

HRESULT TestSetGrouping(IFolderView2* view, REFPROPERTYKEY key, BOOL ascending) {
    return explorer_item_pins::SetGroupingWithPins(TestFolderPins, testSetGroupingTarget,
                                                   groupingApi.setGrouping, view, key, ascending);
}

void InstallTestGrouping(const NativeGrouping& native) {
    constexpr unsigned char batch[]{0x48, 0x89, 0x5c, 0x24, 0x18, 0x48, 0x89, 0x74, 0x24, 0x20, 0x55, 0x57, 0x41, 0x54};
    constexpr unsigned char assign[]{0x40, 0x53, 0x55, 0x56, 0x57, 0x48, 0x83, 0xec, 0x58, 0x48, 0x8b, 0x05, 0xc0, 0x73, 0x6d, 0x00};
    constexpr unsigned char range[]{0x48, 0x89, 0x5c, 0x24, 0x20, 0x55, 0x56, 0x57, 0x48, 0x83, 0xec, 0x50, 0x48, 0x8b, 0x05, 0x1d, 0xb9, 0x60, 0x00};
    constexpr unsigned char strings[]{0x48, 0x89, 0x5c, 0x24, 0x08, 0x48, 0x89, 0x74, 0x24, 0x10, 0x57, 0x48, 0x83, 0xec, 0x40};
    constexpr unsigned char apply[]{0x40, 0x55, 0x53, 0x56, 0x57, 0x41, 0x54, 0x41, 0x56, 0x41, 0x57, 0x48, 0x8b, 0xec};
    constexpr unsigned char getAt[]{0x48, 0x83, 0xec, 0x28, 0x48, 0x8b, 0x49, 0x08, 0x8b, 0xd2, 0xe8, 0, 0, 0, 0};
    constexpr unsigned char set[]{0x48, 0x8b, 0x89, 0x60, 0x05, 0x00, 0x00, 0x48, 0x8b, 0x01, 0x48, 0x8b, 0x40, 0x20};
    groupingApi = native;
    groupingApi.batch = InstallTestHook(native.batch, TestGroupBatch, batch);
    groupingApi.assign = InstallTestHook(native.assign, TestAssignGroup, assign, 9);
    groupingApi.rangeInfo = InstallTestHook(native.rangeInfo, TestRangeInfo, range, 12);
    groupingApi.textInfo = InstallTestHook(native.textInfo, TestTextInfo, strings);
    originalGroupAt = InstallTestHook(native.getGroupAt, TestGetGroupAt, getAt, 10);
    groupingApi.apply = InstallTestHook(native.apply, TestApplyGroups, apply);
    testSetGroupingTarget = native.setGrouping;
    groupingApi.setGrouping = InstallTestHook(native.setGrouping, TestSetGrouping, set);
}

void VerifyGrouping(const fs::path& root) {
    const auto storage = LoadLibraryExW(L"Windows.Storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    Check(storage ? S_OK : HRESULT_FROM_WIN32(GetLastError()), "load Windows.Storage.dll in the isolated test process");
    const auto shell = GetModuleHandleW(L"Shell32.dll");
    const auto grouping = FindNativeGrouping(storage, FindKnownSetGrouping(shell));
    const auto ordering = FindNativeOrdering(storage);
    Require(grouping && ordering, "native grouping requires verified Windows.Storage.dll and Shell32.dll images");
    const auto& store = orderingStore.emplace(root / L"grouping-data" / L"pins.json");
    InstallTestOrdering(*ordering);
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
    WaitForFolderPins(pinned, {std::wstring(names[3]), std::wstring(names[4])});
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

using explorer_item_pins::IndicatorPins;
DrawItemImage nativeDrawImage = nullptr;
explorer_item_pins::PaintFolderImage nativePaintFolderImage = nullptr;
explorer_item_pins::ConstructImageRenderer nativeConstructImage = nullptr;
explorer_item_pins::DestroyImageRenderer nativeDestroyImage = nullptr;
explorer_item_pins::ImageSources imageSources;
std::atomic_uint removedImageSources{};
explorer_item_pins::ImageMethods imageMethods{};
const void* previousPaintCaller = nullptr;
bool paintScopeMatchesCaller = true;
IndicatorPins indicatorFixture;
bool indicatorOn = false;
bool capturePaint = false;

struct IconPixels {
    std::array<COLORREF, 256> right{};
    std::array<COLORREF, 64> left{};
    bool operator==(const IconPixels&) const = default;
};

IconPixels SampleIcon(HDC dc, const RECT& rect) {
    IconPixels pixels;
    for (int y = 0; y < 16; ++y) {
        for (int x = 0; x < 16; ++x) {
            pixels.right[y * 16 + x] = GetPixel(dc, rect.right - 16 + x, rect.top + y);
        }
    }
    for (int y = 0; y < 8; ++y) {
        for (int x = 0; x < 8; ++x) {
            pixels.left[y * 8 + x] = GetPixel(dc, rect.left + x, rect.bottom - 8 + y);
        }
    }
    return pixels;
}

IconPixels stockIcon;
std::map<std::wstring, bool> paintedIcons;
bool shortcutCornerPreserved = true;

struct IndicatorRetryFixture {
    explorer_item_pins::NavigationPinCache cache;
    bool ready = false;
    bool attempted = false;
    bool painted = false;

    static LRESULT CALLBACK Paint(HWND window, UINT message, WPARAM wParam, LPARAM lParam) {
        const auto self = reinterpret_cast<IndicatorRetryFixture*>(GetWindowLongPtrW(window, GWLP_USERDATA));
        if (message != WM_PAINT || !self) return DefWindowProcW(window, message, wParam, lParam);
        PAINTSTRUCT paint{};
        const auto dc = BeginPaint(window, &paint);
        self->painted = explorer_item_pins::DrawPinnedIndicator(dc, {0, 0, 16, 16}, {0, 8, 8, 16}, self->cache,
            [&](HDC target) { return self->ready ? explorer_item_pins::LoadNavigationPin(target) : explorer_item_pins::NavigationPin{}; });
        self->attempted = true;
        EndPaint(window, &paint);
        return 0;
    }
};

void* TestConstructImage(void* renderer, IUnknown* services, IUnknown* item) {
    const auto result = nativeConstructImage(renderer, services, item);
    imageSources.Set(renderer, {services, item});
    return result;
}

void TestDestroyImage(void* renderer) {
    if (imageSources.Erase(renderer)) ++removedImageSources;
    nativeDestroyImage(renderer);
}

HRESULT TestItemImage(const unsigned char* image, HDC dc, const RECT* rect) {
    const bool folderView = explorer_item_pins::FolderImagePaintScope::Active();
    paintScopeMatchesCaller &= folderView == (__builtin_return_address(0) == previousPaintCaller);
    if (!capturePaint || !folderView || !rect) return nativeDrawImage(image, dc, rect);
    const auto result = nativeDrawImage(image, dc, rect);
    const auto source = imageSources.Find(image);
    if (FAILED(result) || !source) return result;
    const auto path = explorer_item_pins::ItemImagePath(imageMethods, *source);
    if (!path) return result;
    stockIcon = SampleIcon(dc, *rect);
    if (indicatorOn) explorer_item_pins::DrawItemPin(*orderingStore, indicatorFixture, imageMethods, *source, dc, *rect);
    const auto output = SampleIcon(dc, *rect);
    const auto name = path->filename().native();
    if (name == L"Pinned shortcut.lnk") shortcutCornerPreserved &= stockIcon.left == output.left;
    paintedIcons[name] |= stockIcon.right != output.right;
    return result;
}

void TestPaintFolderImage(void* image, HDC dc, const RECT* bounds, const RECT* clip, RECT* content, RECT* paint) {
    const explorer_item_pins::FolderImagePaintScope scope;
    nativePaintFolderImage(image, dc, bounds, clip, content, paint);
}

void WriteThumbnail(const fs::path& path) {
    BITMAPFILEHEADER file{};
    BITMAPINFOHEADER info{};
    constexpr LONG size = 64;
    const std::array<DWORD, size * size> pixels = [] {
        std::array<DWORD, size * size> values{};
        values.fill(0x00405080);
        return values;
    }();
    file.bfType = 0x4d42;
    file.bfOffBits = sizeof(file) + sizeof(info);
    file.bfSize = file.bfOffBits + sizeof(pixels);
    info.biSize = sizeof(info);
    info.biWidth = size;
    info.biHeight = size;
    info.biPlanes = 1;
    info.biBitCount = 32;
    std::ofstream stream(path, std::ios::binary);
    stream.write(reinterpret_cast<const char*>(&file), sizeof(file));
    stream.write(reinterpret_cast<const char*>(&info), sizeof(info));
    stream.write(reinterpret_cast<const char*>(pixels.data()), sizeof(pixels));
    Require(stream.good(), "write the thumbnail fixture");
}

void VerifyIndicator(const fs::path& root) {
    const auto folder = root / L"paint";
    fs::create_directories(folder / L"Pinned folder");
    const auto bitmap = folder / L"Pinned thumbnail.bmp";
    WriteThumbnail(bitmap);
    std::ofstream(folder / L"Ordinary.txt") << "ordinary";
    ComPtr<IShellLinkW> link;
    Check(CoCreateInstance(CLSID_ShellLink, nullptr, CLSCTX_INPROC_SERVER, IID_PPV_ARGS(&link)), "create a fixture shortcut");
    Check(link->SetPath(bitmap.c_str()), "set the fixture shortcut target");
    ComPtr<IPersistFile> save;
    Check(link.As(&save), "get the fixture shortcut's persistence interface");
    Check(save->Save((folder / L"Pinned shortcut.lnk").c_str(), TRUE), "save the fixture shortcut");

    const auto storage = LoadLibraryExW(L"Windows.Storage.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    const auto frame = LoadLibraryExW(L"ExplorerFrame.dll", nullptr, LOAD_LIBRARY_SEARCH_SYSTEM32);
    const auto renderer = FindKnownImageRenderer(storage);
    const auto paint = FindKnownFolderImagePaint(frame);
    Require(renderer && paint, "native indicator requires verified Windows.Storage.dll and ExplorerFrame.dll images");
    const auto& store = orderingStore.emplace(root / L"paint-data" / L"pins.json");
    for (const auto* name : {L"Pinned folder", L"Pinned thumbnail.bmp", L"Pinned shortcut.lnk"}) {
        Require(store.SetPinned(folder, explorer_item_pins::ReadItem(folder / name), true), "persist an indicator fixture pin");
    }
    Require(!indicatorFixture.Contains(store, bitmap), "the first paint lookup should not wait for the pin snapshot");
    WaitUntil([&] { return indicatorFixture.Contains(store, bitmap); }, "load the indicator snapshot off the paint thread");
    Require(indicatorFixture.Cached(bitmap) == true && indicatorFixture.Cached(folder / L"Ordinary.txt") == false,
            "use one snapshot for actions on both pinned and ordinary items");
    {
        // This offscreen frame receives the same invalidation as an Explorer window.
        IndicatorRetryFixture retry;
        WNDCLASSW windowClass{};
        windowClass.hInstance = GetModuleHandleW(nullptr);
        windowClass.lpszClassName = L"CabinetWClass";
        windowClass.lpfnWndProc = IndicatorRetryFixture::Paint;
        Require(RegisterClassW(&windowClass) != 0, "register the isolated retry frame");
        const std::unique_ptr<std::remove_pointer_t<HWND>, decltype(&DestroyWindow)> retryFrame(
            CreateWindowExW(WS_EX_TOOLWINDOW | WS_EX_NOACTIVATE, windowClass.lpszClassName, L"Indicator retry test",
                            WS_OVERLAPPEDWINDOW, -10000, -10000, 80, 80, nullptr, nullptr, windowClass.hInstance, nullptr),
            DestroyWindow);
        Require(retryFrame != nullptr, "create the isolated retry frame");
        SetWindowLongPtrW(retryFrame.get(), GWLP_USERDATA, reinterpret_cast<LONG_PTR>(&retry));
        ShowWindow(retryFrame.get(), SW_SHOWNOACTIVATE);
        SendMessageW(retryFrame.get(), WM_PAINT, 0, 0);
        Require(retry.attempted, "deliver the first paint to the isolated retry frame");
        Require(!retry.painted, "keep the stock icon while the navigation pin is unavailable");
        retry.ready = true;
        WaitUntil([&] { return retry.painted; }, "repaint after the native pin resource becomes available");
    }
    constexpr unsigned char draw[]{0x48, 0x89, 0x5c, 0x24, 0x20, 0x55, 0x56, 0x57, 0x41, 0x54, 0x41, 0x55, 0x41, 0x56};
    constexpr unsigned char paintPrologue[]{0x40, 0x55, 0x53, 0x41, 0x57, 0x48, 0x8d, 0x6c, 0x24,
                                           0xc9, 0x48, 0x81, 0xec, 0xa0, 0x00, 0x00, 0x00};
    previousPaintCaller = reinterpret_cast<const unsigned char*>(frame) + 0xbe1ac;
    constexpr unsigned char construct[]{0x48, 0x89, 0x5c, 0x24, 0x08, 0x48, 0x89, 0x74, 0x24, 0x10, 0x57, 0x48, 0x83, 0xec, 0x20};
    constexpr unsigned char destroy[]{0x40, 0x53, 0x48, 0x83, 0xec, 0x20, 0x48, 0x8b, 0xd9, 0x48, 0x8d, 0x05, 0, 0, 0, 0};
    const auto code = CheckedImage(storage, storagePdb9974, 0x215470);
    Require(code != nullptr, "verify the current image renderer fixture");
    imageMethods = {
        reinterpret_cast<explorer_item_pins::ReadImageValue>(code + 0x31db0),
        reinterpret_cast<explorer_item_pins::ReadImageValue>(code + 0x1add40),
        reinterpret_cast<explorer_item_pins::DrawCachedIcon>(code + 0x215470),
        reinterpret_cast<explorer_item_pins::FillCachedOverlay>(code + 0x1295c0),
    };
    nativeConstructImage = InstallTestHook(reinterpret_cast<explorer_item_pins::ConstructImageRenderer>(code + 0x32a88),
                                           TestConstructImage, construct);
    nativeDestroyImage = InstallTestHook(reinterpret_cast<explorer_item_pins::DestroyImageRenderer>(code + 0x32b68),
                                         TestDestroyImage, destroy, 9);
    nativePaintFolderImage = InstallTestHook(paint, TestPaintFolderImage, paintPrologue);
    nativeDrawImage = InstallTestHook(renderer, TestItemImage, draw);

    ExplorerView view(folder, 4);
    const auto window = view.Window();
    SetWindowPos(window, nullptr, -10000, -10000, 0, 0, SWP_NOACTIVATE | SWP_NOSIZE | SWP_NOZORDER);
    ShowWindow(window, SW_SHOWNOACTIVATE);
    RECT bounds{};
    GetWindowRect(window, &bounds);
    const auto screen = GetDC(window);
    const auto dc = CreateCompatibleDC(screen);
    const auto surface = CreateCompatibleBitmap(screen, bounds.right - bounds.left, bounds.bottom - bounds.top);
    ReleaseDC(window, screen);
    Require(dc && surface, "create a private render target for the native view");
    const auto previous = SelectObject(dc, surface);

    explorer_item_pins::NavigationPin uniformColor{{0xff97a2a8u, 0}, 2, 1};
    std::array<DWORD, 3> blended{};
    Require(explorer_item_pins::ScaleNavigationPin(uniformColor, 3, 1, blended.data()), "resample a translucent edge");
    const auto edge = blended[1];
    const auto coverage = static_cast<BYTE>(edge >> 24);
    Require(coverage > 32 && coverage < 224 && std::abs(static_cast<int>(static_cast<BYTE>(edge >> 16)) - 151) <= 3,
            "keep the pin color at fractional alpha for light and dark backgrounds");

    const auto nativePin = explorer_item_pins::LoadNavigationPin(dc);
    Require(nativePin.width > 100 && nativePin.height > 100, "load the original high-resolution navigation pin from Windows");
    const auto solid = std::ranges::find_if(nativePin.pixels, [](DWORD pixel) { return pixel >> 24 == 0xff; });
    Require(solid != nativePin.pixels.end(), "read the navigation pin's opaque color");
    const auto solidGreen = static_cast<BYTE>(*solid >> 8);

    for (const auto& [size, arrow] : {std::pair{16, 8}, {32, 13}, {96, 19}, {256, 40}}) {
        const RECT icon{8, 8, 8 + size, 8 + size};
        const RECT shortcut{0, size - arrow, arrow, size};
        PatBlt(dc, 0, 0, size + 16, size + 16, WHITENESS);
        Require(explorer_item_pins::DrawPinnedIndicator(dc, icon, shortcut), "draw the navigation pin resource");
        const auto width = size + 16;
        const auto ink = explorer_item_pins::VisibleBounds(width, width, [&](size_t i) {
            return GetPixel(dc, i % width, i / width) != RGB(255, 255, 255);
        });
        Require(ink.top == icon.top && ink.right == icon.right && ink.right - ink.left == arrow &&
                ink.bottom - ink.top == arrow, "mirror the shortcut bounds with the visible pin pixels");
        const auto smoothEdge = explorer_item_pins::VisibleBounds(width, width, [&](size_t i) {
            const auto green = GetGValue(GetPixel(dc, i % width, i / width));
            return green > solidGreen && green < 255;
        });
        Require(!IsRectEmpty(&smoothEdge), "preserve antialiased coverage while shrinking the native resource");
    }

    int verified = 0;
    for (const auto& layout : layouts) {
        SetLayout(view, layout);
        for (bool enabled : {false, true}) {
            indicatorOn = enabled;
            WaitUntil([&] {
                paintedIcons.clear();
                shortcutCornerPreserved = true;
                capturePaint = true;
                const auto rendered = PrintWindow(window, dc, 2);
                capturePaint = false;
                return rendered && paintedIcons.size() == 4 && (!enabled ||
                    (paintedIcons[L"Pinned folder"] && paintedIcons[L"Pinned thumbnail.bmp"] &&
                     paintedIcons[L"Pinned shortcut.lnk"]));
            }, "paint each fixture image after the native overlay cache is ready");
            if (paintedIcons[L"Ordinary.txt"] || paintedIcons[L"Pinned folder"] != enabled ||
                paintedIcons[L"Pinned thumbnail.bmp"] != enabled || paintedIcons[L"Pinned shortcut.lnk"] != enabled ||
                !shortcutCornerPreserved) {
                std::cerr << "Layout " << layout.mode << " size " << layout.size << " enabled " << enabled
                          << " ordinary " << paintedIcons[L"Ordinary.txt"] << " folder " << paintedIcons[L"Pinned folder"]
                          << " thumbnail " << paintedIcons[L"Pinned thumbnail.bmp"] << " shortcut "
                          << paintedIcons[L"Pinned shortcut.lnk"] << " corner " << shortcutCornerPreserved << '\n';
                throw std::runtime_error("draw only pinned top corners and preserve the shortcut overlay corner");
            }
            ++verified;
        }
    }
    SelectObject(dc, previous);
    Require(paintScopeMatchesCaller, "match the previous indicator call site for every renderer call");
    DeleteObject(surface);
    DeleteDC(dc);
    Require(store.SetPinned(folder, explorer_item_pins::ReadItem(bitmap), false), "remove the thumbnail's fixture pin");
    indicatorFixture.Invalidate(folder);
    Require(!indicatorFixture.Cached(bitmap), "invalidate the cached action after a pin change");
    WaitUntil([&] { return !indicatorFixture.Contains(store, bitmap); }, "apply a changed indicator snapshot");
    Require(indicatorFixture.Contains(store, folder / L"Pinned folder"), "preserve the other indicators after a change");
    Require(removedImageSources.load() > 0, "the native renderer destructor should remove tracked sources");
    std::cout << "PASS indicator pixels and shortcut corner in " << verified
              << " native view scenarios, with native renderer cleanup\n";
}
}

int wmain(int argc, wchar_t** argv) {
    if (argc != 3) return 2;
    if (FAILED(OleInitialize(nullptr))) return 3;
    int result = 0;
    try {
        Require(explorer_item_pins::moduleWork.Start(), "start the module's background work queue");
        const std::wstring_view mode(argv[2]);
        if (mode == L"commands") {
            VerifyCommands(argv[1]);
            std::cout << "PASS native Explorer selection, pin and unpin, rename, replacement, copy and move\n";
            VerifyFolderSnapshots(argv[1]);
        } else if (mode == L"ordering") {
            VerifyOrdering(argv[1]);
        } else if (mode == L"grouping") {
            VerifyGrouping(argv[1]);
        } else if (mode == L"indicator") {
            VerifyIndicator(argv[1]);
        } else {
            result = 2;
        }
    } catch (const std::exception& error) {
        std::cerr << "FAIL " << error.what() << '\n';
        result = 1;
    }
    explorer_item_pins::moduleWork.Stop();
    OleUninitialize();
    return result;
}

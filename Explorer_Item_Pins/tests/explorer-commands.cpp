#include "../mods/explorer-item-pins.wh.cpp"

#include <wrl/client.h>

#include <fstream>
#include <iostream>
#include <type_traits>

namespace {
using explorer_item_pins::ApplyPinAction;
using explorer_item_pins::CaptureSingleSelection;
using explorer_item_pins::GetPinAction;
using explorer_item_pins::PinAction;
using explorer_item_pins::PinStore;
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
}

int wmain(int argc, wchar_t** argv) {
    if (argc != 2) return 2;
    if (FAILED(OleInitialize(nullptr))) return 3;
    int result = 0;
    try {
        VerifyCommands(argv[1]);
        std::cout << "PASS native Explorer selection, pin and unpin, rename, replacement, copy and move\n";
    } catch (const std::exception& error) {
        std::cerr << "FAIL " << error.what() << '\n';
        result = 1;
    }
    OleUninitialize();
    return result;
}

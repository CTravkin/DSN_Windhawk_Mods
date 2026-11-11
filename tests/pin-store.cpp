#include "../mods/explorer-item-pins.wh.cpp"

#include <fstream>
#include <iostream>

namespace {
using explorer_item_pins::Item;
using explorer_item_pins::PinStore;
using explorer_item_pins::ReadItem;
using explorer_item_pins::ResolvePins;
namespace fs = std::filesystem;

constexpr wchar_t folderA[] = LR"(C:\PinStore fixtures\A)";
constexpr wchar_t folderB[] = LR"(C:\PinStore fixtures\B)";
const Item report{L"Отчет.docx", L"1:report"};
const Item photos{L"照片", L"1:photos"};

void Require(bool condition, const char* message) {
    if (!condition) throw std::runtime_error(message);
}

std::string ReadBytes(const fs::path& file) {
    std::ifstream input(file, std::ios::binary);
    return {std::istreambuf_iterator<char>(input), std::istreambuf_iterator<char>()};
}

void Persistence(const fs::path& root) {
    const auto file = root / L"persistence" / L"pins.json";
    PinStore store(file);
    Require(store.ReadFolder(folderA).empty(), "a missing store should return an empty folder");
    Require(!fs::exists(file.parent_path()), "reading a missing store should not create directories");
    Require(store.SetPinned(folderA, report, true), "pinning a new file should write the record");
    Require(store.SetPinned(folderA, photos, true), "pinning a new folder should write the record");
    Require(!store.SetPinned(folderA, photos, true), "pinning the same object twice should be idempotent");

    PinStore reopened(file);
    Require(reopened.ReadFolder(LR"(c:\pinstore fixtures\a\)") == std::vector{report, photos},
            "records should survive reopen, Unicode and Windows path case");
    Require(reopened.ReadFolder(folderB).empty(), "a different parent folder should have its own pins");
    Require(reopened.SetPinned(folderA, report, false), "unpinning should remove the named record");
    Require(PinStore(file).ReadFolder(folderA) == std::vector{photos}, "unpinning should persist");
    Require(!fs::exists(file.parent_path() / L"pins.json.lock"), "the lock file should be removed on close");
}

void FolderSemantics(const fs::path& root) {
    PinStore store(root / L"semantics" / L"pins.json");
    store.SetPinned(folderA, report, true);
    const auto stored = store.ReadFolder(folderA);
    const Item replacement{report.name, L"1:replacement"};
    const Item renamed{L"План.docx", report.identity};

    Require(ResolvePins(stored, std::vector{replacement, renamed}) ==
                std::vector<std::optional<size_t>>{0},
            "a replacement at the pinned name should win over the old identity at another name");
    Require(ResolvePins(stored, std::vector{renamed}) == std::vector<std::optional<size_t>>{0},
            "an unambiguous rename within the same folder should follow the identity");
    Require(ResolvePins(stored, std::vector{renamed, Item{L"Link.docx", report.identity}}) ==
                std::vector<std::optional<size_t>>{std::nullopt},
            "two names for the same identity should not guess which item was renamed");
    Require(ResolvePins(store.ReadFolder(folderB), std::vector{report, replacement}).empty(),
            "the destination should not inherit a pin from another folder");

    const std::vector<Item> nameOnly{{L"Archive.zip", L""}};
    Require(ResolvePins(nameOnly, std::vector{Item{L"archive.zip", L""}}) ==
                std::vector<std::optional<size_t>>{0},
            "ordinary Windows paths should still match when only case changed and no identity is available");
}

void ProtectInvalidData(const fs::path& root) {
    const auto file = root / L"invalid.json";
    const std::string invalidDocuments[] = {
        R"({"version":2,"folders":[]})",
        R"({"version":1,"folders":[{"path":"C:\\Example","pins":[{"name":"..","identity":""}]}]})",
        "{",
    };
    for (const auto& invalid : invalidDocuments) {
        std::ofstream(file, std::ios::binary | std::ios::trunc) << invalid;
        bool rejected = false;
        try {
            PinStore(file).SetPinned(folderA, report, true);
        } catch (const std::exception&) {
            rejected = true;
        }
        Require(rejected && ReadBytes(file) == invalid, "an invalid store should be preserved verbatim");
    }
}

void NativeFileSemantics(const fs::path& root) {
    const auto source = root / L"native" / L"source";
    const auto target = root / L"native" / L"target";
    fs::create_directories(source / L"Фото");
    fs::create_directories(target);
    PinStore store(root / L"native" / L"pins.json");
    const auto original = source / L"Отчет.txt";
    const auto temporary = source / L"new.txt";
    const auto renamed = source / L"План.txt";
    std::ofstream(original) << "original";
    std::ofstream(temporary) << "replacement";
    const auto pin = ReadItem(original);
    Require(!pin.identity.empty(), "a local test file should expose a Windows identity");
    Require(!ReadItem(source / L"Фото").identity.empty(), "a local folder should expose a Windows identity");
    Require(ReadItem(source / L"missing.txt").identity.empty(), "an unavailable identity should leave a name-only pin");

    store.SetPinned(source, pin, true);
    fs::rename(original, renamed);
    const auto renamedItem = ReadItem(renamed);
    Require(renamedItem.identity == pin.identity, "rename should preserve identity on the test volume");
    Require(ResolvePins(store.ReadFolder(source), std::vector{renamedItem}) ==
                std::vector<std::optional<size_t>>{0},
            "a renamed file should still match its pin");
    fs::rename(temporary, original);
    const auto replacement = ReadItem(original);
    Require(replacement.identity != pin.identity, "the replacement fixture must be a different file");
    Require(ResolvePins(store.ReadFolder(source), std::vector{replacement, renamedItem}) ==
                std::vector<std::optional<size_t>>{0},
            "the same name should keep the pin when the file is replaced");

    fs::copy_file(original, target / original.filename());
    fs::rename(renamed, target / renamed.filename());
    Require(ResolvePins(store.ReadFolder(target),
                        std::vector{ReadItem(target / original.filename()), ReadItem(target / renamed.filename())}).empty(),
            "copying and moving actual files should not create destination pins");

    const auto defaultPath = PinStore::DefaultPath();
    Require(defaultPath.is_absolute() && defaultPath.filename() == L"pins.json" &&
                defaultPath.parent_path().filename() == L"Explorer Item Pins",
            "the default path should use the agreed user data directory and file name");
}

Item ConcurrentItem(unsigned writer, unsigned index) {
    return {L"writer-" + std::to_wstring(writer) + L"-" + std::to_wstring(index), L""};
}

void WaitForStart(const wchar_t* name) {
    const HANDLE event = OpenEventW(SYNCHRONIZE, FALSE, name);
    Require(event != nullptr, "the start event should be open");
    const DWORD result = WaitForSingleObject(event, 10000);
    CloseHandle(event);
    Require(result == WAIT_OBJECT_0, "the test processes should start together");
}
}

int wmain(int argc, wchar_t** argv) {
    try {
        if (argc == 3 && std::wstring_view(argv[1]) == L"self-test") {
            Persistence(argv[2]);
            FolderSemantics(argv[2]);
            ProtectInvalidData(argv[2]);
            NativeFileSemantics(argv[2]);
            std::cout << "PASS persistence, folder semantics, invalid data, native identities\n";
        } else if (argc == 5 && std::wstring_view(argv[1]) == L"write") {
            PinStore store(argv[2]);
            const auto writer = static_cast<unsigned>(std::stoul(argv[3]));
            WaitForStart(argv[4]);
            for (unsigned i = 0; i < 12; ++i) store.SetPinned(folderA, ConcurrentItem(writer, i), true);
        } else if (argc == 4 && std::wstring_view(argv[1]) == L"read") {
            PinStore store(argv[2]);
            WaitForStart(argv[3]);
            size_t previous = 0;
            for (unsigned i = 0; i < 100; ++i) {
                const size_t count = store.ReadFolder(folderA).size();
                Require(count >= previous, "a reader should see complete snapshots without lost pins");
                previous = count;
                Sleep(5);
            }
        } else if (argc == 4 && std::wstring_view(argv[1]) == L"verify-writers") {
            const auto writers = static_cast<unsigned>(std::stoul(argv[3]));
            const auto pins = PinStore(argv[2]).ReadFolder(folderA);
            Require(pins.size() == writers * 12, "concurrent writers should preserve every pin");
            for (unsigned writer = 0; writer < writers; ++writer) {
                for (unsigned i = 0; i < 12; ++i) {
                    Require(std::find(pins.begin(), pins.end(), ConcurrentItem(writer, i)) != pins.end(),
                            "a concurrent pin was lost");
                }
            }
            std::cout << "PASS " << writers << " writers and one concurrent reader, "
                      << pins.size() << " persisted pins\n";
        } else {
            return 2;
        }
    } catch (const std::exception& error) {
        std::cerr << "FAIL " << error.what() << '\n';
        return 1;
    }
    return 0;
}

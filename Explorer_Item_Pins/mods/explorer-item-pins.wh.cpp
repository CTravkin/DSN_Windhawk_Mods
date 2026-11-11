// ==WindhawkMod==
// @id              explorer-item-pins
// @name            Explorer Item Pins
// @description     Pin files and folders to the top of their parent folder in File Explorer.
// @description:ru-RU Закрепляет файлы и подпапки вверху списка открытой папки Проводника.
// @description:es-ES Ancla archivos y carpetas en la parte superior de su carpeta en el Explorador de archivos.
// @description:de-DE Heftet Dateien und Ordner im Datei-Explorer oben in ihrem übergeordneten Ordner an.
// @description:pt-BR Fixa arquivos e pastas no topo da pasta que os contém no Explorador de Arquivos.
// @description:zh-CN 将文件和文件夹固定到文件资源管理器中所在文件夹的顶部。
// @version         0.4.0
// @author          Constantine Travkin
// @github          https://github.com/CTravkin
// @include         explorer.exe
// @architecture    amd64
// @compilerOptions -lruntimeobject -lole32 -loleaut32 -lshell32 -lshlwapi -luuid
// ==/WindhawkMod==
// Build: 4

// ==WindhawkModReadme==
/*
# Explorer Item Pins

Explorer Item Pins is a Windhawk mod in development for pinning files and
folders to the top of their parent folder in File Explorer.
*/
// ==/WindhawkModReadme==

// ==WindhawkModSettings==
/*
- contextMenu: true
  $name: Context menu command
  $name:ru-RU: Команда контекстного меню
  $name:es-ES: Comando del menú contextual
  $name:de-DE: Befehl im Kontextmenü
  $name:pt-BR: Comando do menu de contexto
  $name:zh-CN: 上下文菜单命令
- commandBar: true
  $name: Command bar button
  $name:ru-RU: Кнопка на панели команд
  $name:es-ES: Botón de la barra de comandos
  $name:de-DE: Schaltfläche in der Befehlsleiste
  $name:pt-BR: Botão na barra de comandos
  $name:zh-CN: 命令栏按钮
- pinIndicator: true
  $name: Pinned item indicator
  $name:ru-RU: Индикатор закрепленного объекта
  $name:es-ES: Indicador de elemento anclado
  $name:de-DE: Symbol für angeheftete Elemente
  $name:pt-BR: Indicador de item fixado
  $name:zh-CN: 固定项标记
*/
// ==/WindhawkModSettings==

#include <windows.h>
#include <roapi.h>
#include <shlobj.h>
#include <shlwapi.h>
#include <propkey.h>
#include <winrt/Windows.Foundation.h>
#include <winrt/Windows.Foundation.Collections.h>
#include <winrt/Windows.Data.Json.h>
#include <wrl/client.h>

#include <algorithm>
#include <cstring>
#include <filesystem>
#include <map>
#include <memory>
#include <optional>
#include <set>
#include <span>
#include <stdexcept>
#include <string>
#include <string_view>
#include <system_error>
#include <type_traits>
#include <vector>

namespace explorer_item_pins {

namespace fs = std::filesystem;
using winrt::Windows::Data::Json::IJsonValue;
using winrt::Windows::Data::Json::IJsonObjectStatics;
using winrt::Windows::Data::Json::IJsonValueStatics;
using winrt::Windows::Data::Json::JsonArray;
using winrt::Windows::Data::Json::JsonObject;
using winrt::Windows::Data::Json::JsonValue;

struct Item {
    std::wstring name;
    std::wstring identity;
    bool operator==(const Item&) const = default;
};

namespace {

constexpr unsigned formatVersion = 1;
constexpr size_t maxStoreBytes = 16 * 1024 * 1024;

int CompareNames(std::wstring_view left, std::wstring_view right) {
    return CompareStringOrdinal(left.data(), static_cast<int>(left.size()),
                                right.data(), static_cast<int>(right.size()), TRUE);
}

struct FolderLess {
    bool operator()(const std::wstring& left, const std::wstring& right) const {
        return CompareNames(left, right) == CSTR_LESS_THAN;
    }
};

using Folders = std::map<std::wstring, std::vector<Item>, FolderLess>;
using FileHandle = std::unique_ptr<void, decltype(&CloseHandle)>;

class JsonApartment {
public:
    JsonApartment() : result_(RoInitialize(RO_INIT_MULTITHREADED)) {
        if (result_ != RPC_E_CHANGED_MODE) winrt::check_hresult(result_);
    }
    ~JsonApartment() {
        if (SUCCEEDED(result_)) RoUninitialize();
    }
    JsonApartment(const JsonApartment&) = delete;
    JsonApartment& operator=(const JsonApartment&) = delete;

private:
    HRESULT result_;
};

template <typename Class, typename Interface = winrt::Windows::Foundation::IActivationFactory>
auto JsonFactory() {
    // Keep activation factories within the lifetime of the calling apartment.
    auto factory = winrt::try_get_activation_factory<Class, Interface>();
    if (!factory) throw std::runtime_error("Cannot activate Windows JSON");
    return factory;
}

[[noreturn]] void FileError(DWORD code, const char* operation) {
    throw std::system_error(static_cast<int>(code), std::system_category(), operation);
}

std::wstring FolderKey(fs::path path) {
    if (!path.is_absolute() || path.native().find(L'\0') != std::wstring::npos) {
        throw std::invalid_argument("An absolute Windows folder path is required");
    }
    path = path.lexically_normal().make_preferred();
    if (path.has_relative_path() && !path.has_filename()) path = path.parent_path();
    return path.native();
}

void CheckItem(const Item& item) {
    if (item.name.empty() || item.name == L"." || item.name == L".." ||
        item.name.find_first_of(L"\\/:*?\"<>|") != std::wstring::npos ||
        item.name.find(L'\0') != std::wstring::npos ||
        item.identity.find(L'\0') != std::wstring::npos) {
        throw std::invalid_argument("A pin must have a valid relative item name");
    }
}

std::optional<std::string> ReadFileBytes(const fs::path& path) {
    const HANDLE raw = CreateFileW(path.c_str(), GENERIC_READ,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                   nullptr, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, nullptr);
    if (raw == INVALID_HANDLE_VALUE) {
        const DWORD error = GetLastError();
        if (error == ERROR_FILE_NOT_FOUND || error == ERROR_PATH_NOT_FOUND) return std::nullopt;
        FileError(error, "Open pin store");
    }
    const FileHandle file(raw, CloseHandle);
    LARGE_INTEGER size{};
    if (!GetFileSizeEx(file.get(), &size)) FileError(GetLastError(), "Size pin store");
    if (size.QuadPart <= 0 || size.QuadPart > static_cast<LONGLONG>(maxStoreBytes)) {
        throw std::runtime_error("Invalid pin store size");
    }
    std::string bytes(static_cast<size_t>(size.QuadPart), '\0');
    DWORD count = 0;
    if (!ReadFile(file.get(), bytes.data(), static_cast<DWORD>(bytes.size()), &count, nullptr)) {
        FileError(GetLastError(), "Read pin store");
    }
    if (count != bytes.size()) throw std::runtime_error("Incomplete pin store read");
    return bytes;
}

Folders ReadFolders(const fs::path& path) {
    const auto bytes = ReadFileBytes(path);
    if (!bytes) return {};
    try {
        const JsonApartment apartment;
        const auto root = JsonFactory<JsonObject, IJsonObjectStatics>().Parse(winrt::to_hstring(*bytes));
        if (root.GetNamedNumber(L"version") != formatVersion) {
            throw std::runtime_error("Unsupported pin store version");
        }
        Folders folders;
        for (const auto& value : root.GetNamedArray(L"folders")) {
            const auto folder = value.GetObject();
            const auto key = FolderKey(std::wstring(folder.GetNamedString(L"path")));
            const auto [position, inserted] = folders.try_emplace(key);
            if (!inserted) throw std::runtime_error("Duplicate folder in pin store");
            auto& pins = position->second;
            for (const auto& pinValue : folder.GetNamedArray(L"pins")) {
                const auto pin = pinValue.GetObject();
                Item item{std::wstring(pin.GetNamedString(L"name")),
                          std::wstring(pin.GetNamedString(L"identity"))};
                CheckItem(item);
                if (std::ranges::find(pins, item.name, &Item::name) != pins.end()) {
                    throw std::runtime_error("Duplicate item name in pin store");
                }
                pins.push_back(std::move(item));
            }
        }
        return folders;
    } catch (const winrt::hresult_error&) {
        throw std::runtime_error("Cannot parse pin store JSON");
    }
}

std::string WriteJson(const Folders& folders) {
    try {
        const JsonApartment apartment;
        const auto objectFactory = JsonFactory<JsonObject>();
        const auto arrayFactory = JsonFactory<JsonArray>();
        const auto valueFactory = JsonFactory<JsonValue, IJsonValueStatics>();
        auto folderArray = arrayFactory.ActivateInstance<JsonArray>();
        for (const auto& [path, items] : folders) {
            auto pins = arrayFactory.ActivateInstance<JsonArray>();
            for (const auto& item : items) {
                auto pin = objectFactory.ActivateInstance<JsonObject>();
                pin.SetNamedValue(L"name", valueFactory.CreateStringValue(item.name));
                pin.SetNamedValue(L"identity", valueFactory.CreateStringValue(item.identity));
                pins.Append(pin.as<IJsonValue>());
            }
            auto folder = objectFactory.ActivateInstance<JsonObject>();
            folder.SetNamedValue(L"path", valueFactory.CreateStringValue(path));
            folder.SetNamedValue(L"pins", pins.as<IJsonValue>());
            folderArray.Append(folder.as<IJsonValue>());
        }
        auto root = objectFactory.ActivateInstance<JsonObject>();
        root.SetNamedValue(L"version", valueFactory.CreateNumberValue(formatVersion));
        root.SetNamedValue(L"folders", folderArray.as<IJsonValue>());
        return winrt::to_string(root.Stringify());
    } catch (const winrt::hresult_error&) {
        throw std::runtime_error("Cannot serialize pin store JSON");
    }
}

FileHandle LockForWriting(const fs::path& path) {
    fs::path lockPath = path;
    lockPath += L".lock";
    const auto deadline = GetTickCount64() + 5000;
    for (;;) {
        const HANDLE raw = CreateFileW(lockPath.c_str(), GENERIC_READ | GENERIC_WRITE | DELETE, 0,
                                       nullptr, OPEN_ALWAYS,
                                       FILE_ATTRIBUTE_TEMPORARY | FILE_FLAG_DELETE_ON_CLOSE, nullptr);
        if (raw != INVALID_HANDLE_VALUE) return FileHandle(raw, CloseHandle);

        // A delete-on-close lock may be reported as access denied until its owner exits.
        const DWORD error = GetLastError();
        if ((error != ERROR_SHARING_VIOLATION && error != ERROR_ACCESS_DENIED &&
             error != ERROR_DELETE_PENDING) || GetTickCount64() >= deadline) {
            FileError(error, "Lock pin store");
        }
        Sleep(15);
    }
}

void ReplacePinFile(const fs::path& path, std::string_view bytes) {
    if (bytes.size() > maxStoreBytes) throw std::runtime_error("Pin store size limit exceeded");
    GUID token{};
    wchar_t suffix[39]{};
    if (FAILED(CoCreateGuid(&token)) || !StringFromGUID2(token, suffix, ARRAYSIZE(suffix))) {
        throw std::runtime_error("Cannot allocate a temporary pin store name");
    }
    fs::path temporary = path;
    temporary += std::wstring(suffix) + L".tmp";
    const HANDLE raw = CreateFileW(temporary.c_str(), GENERIC_WRITE, 0, nullptr, CREATE_NEW,
                                   FILE_ATTRIBUTE_NORMAL, nullptr);
    if (raw == INVALID_HANDLE_VALUE) FileError(GetLastError(), "Create temporary pin store");
    FileHandle file(raw, CloseHandle);
    try {
        DWORD written = 0;
        if (!WriteFile(file.get(), bytes.data(), static_cast<DWORD>(bytes.size()), &written, nullptr)) {
            FileError(GetLastError(), "Write temporary pin store");
        }
        if (written != bytes.size()) throw std::runtime_error("Incomplete pin store write");
        if (!FlushFileBuffers(file.get())) FileError(GetLastError(), "Flush temporary pin store");
        file.reset();
        if (!MoveFileExW(temporary.c_str(), path.c_str(), MOVEFILE_REPLACE_EXISTING | MOVEFILE_WRITE_THROUGH)) {
            FileError(GetLastError(), "Replace pin store");
        }
    } catch (...) {
        file.reset();
        DeleteFileW(temporary.c_str());
        throw;
    }
}

}  // namespace

Item ReadItem(const fs::path& path) {
    Item item{path.filename().native(), {}};
    const HANDLE raw = CreateFileW(path.c_str(), FILE_READ_ATTRIBUTES,
                                   FILE_SHARE_READ | FILE_SHARE_WRITE | FILE_SHARE_DELETE,
                                   nullptr, OPEN_EXISTING,
                                   FILE_FLAG_BACKUP_SEMANTICS | FILE_FLAG_OPEN_REPARSE_POINT, nullptr);
    if (raw == INVALID_HANDLE_VALUE) return item;
    const FileHandle file(raw, CloseHandle);
    FILE_ID_INFO info{};
    if (!GetFileInformationByHandleEx(file.get(), FileIdInfo, &info, sizeof(info)) ||
        std::ranges::all_of(info.FileId.Identifier, [](BYTE byte) { return byte == 0; })) {
        return item;
    }

    constexpr wchar_t digits[] = L"0123456789abcdef";
    const auto appendByte = [&](BYTE byte) {
        item.identity.push_back(digits[byte >> 4]);
        item.identity.push_back(digits[byte & 0xf]);
    };
    item.identity.reserve(16 + 1 + 32);
    for (int shift = 56; shift >= 0; shift -= 8) {
        appendByte(static_cast<BYTE>(info.VolumeSerialNumber >> shift));
    }
    item.identity.push_back(L':');
    for (const auto byte : info.FileId.Identifier) appendByte(byte);
    return item;
}

class PinStore {
public:
    explicit PinStore(fs::path path) : path_(std::move(path)) {}

    static fs::path DefaultPath() {
        PWSTR path = nullptr;
        const HRESULT result = SHGetKnownFolderPath(FOLDERID_LocalAppData, KF_FLAG_DONT_VERIFY, nullptr, &path);
        const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> folder(path, CoTaskMemFree);
        if (FAILED(result) || !folder) throw std::runtime_error("Cannot locate Local AppData");
        return fs::path(folder.get()) / L"Explorer Item Pins" / L"pins.json";
    }

    std::vector<Item> ReadFolder(const fs::path& folder) const {
        const auto key = FolderKey(folder);
        auto data = ReadFolders(path_);
        const auto found = data.find(key);
        return found != data.end() ? std::move(found->second) : std::vector<Item>{};
    }

    bool SetPinned(const fs::path& folder, const Item& item, bool pinned) const {
        const auto key = FolderKey(folder);
        CheckItem(item);
        fs::create_directories(path_.parent_path());
        const auto lock = LockForWriting(path_);
        auto data = ReadFolders(path_);
        auto& entries = data[key];
        const auto match = std::ranges::find(entries, item.name, &Item::name);
        if (pinned) {
            if (match == entries.end()) {
                entries.push_back(item);
            } else {
                if (*match == item) return false;
                *match = item;
            }
        } else {
            if (match == entries.end()) return false;
            entries.erase(match);
            if (entries.empty()) data.erase(key);
        }
        ReplacePinFile(path_, WriteJson(data));
        return true;
    }

private:
    fs::path path_;
};

// Resolves one visible item per stored pin. Reserve names for all pins before
// following identities so a replacement at an existing name keeps its pin.
std::vector<std::optional<size_t>> ResolvePins(std::span<const Item> pins, std::span<const Item> items) {
    std::vector<std::optional<size_t>> resolved(pins.size());
    std::vector<bool> used(items.size());
    const auto resolveUnique = [&](auto matches) {
        for (size_t pin = 0; pin < pins.size(); ++pin) {
            if (resolved[pin]) continue;
            std::optional<size_t> match;
            for (size_t item = 0; item < items.size(); ++item) {
                if (!matches(pins[pin], items[item])) continue;
                if (match) {
                    match.reset();
                    break;
                }
                match = item;
            }
            if (match && !used[*match]) {
                used[*match] = true;
                resolved[pin] = match;
            }
        }
    };
    resolveUnique([](const Item& pin, const Item& item) { return pin.name == item.name; });
    resolveUnique([](const Item& pin, const Item& item) {
        return CompareNames(pin.name, item.name) == CSTR_EQUAL;
    });
    resolveUnique([](const Item& pin, const Item& item) {
        return !pin.identity.empty() && pin.identity == item.identity;
    });
    return resolved;
}

struct ExplorerSelection {
    fs::path folder;
    fs::path item;
};

enum class PinAction { Pin, Unpin };

namespace {

std::optional<fs::path> FileSystemPath(PCIDLIST_ABSOLUTE id) {
    if (!id) return std::nullopt;
    PWSTR rawName = nullptr;
    const HRESULT nameResult = SHGetNameFromIDList(id, SIGDN_FILESYSPATH, &rawName);
    const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> name(rawName, CoTaskMemFree);
    if (FAILED(nameResult) || !name) return std::nullopt;

    fs::path path(name.get());
    if (!path.is_absolute()) return std::nullopt;
    return path;
}

std::optional<fs::path> FileSystemFolderPath(IPersistFolder2& folder) {
    PIDLIST_ABSOLUTE rawFolder = nullptr;
    const HRESULT result = folder.GetCurFolder(&rawFolder);
    const std::unique_ptr<std::remove_pointer_t<PIDLIST_ABSOLUTE>, decltype(&CoTaskMemFree)>
        folderId(rawFolder, CoTaskMemFree);
    return SUCCEEDED(result) ? FileSystemPath(folderId.get()) : std::nullopt;
}

}  // namespace

std::optional<ExplorerSelection> CaptureSingleSelection(IFolderView2& view) {
    using Microsoft::WRL::ComPtr;
    ComPtr<IShellItemArray> selected;
    if (view.GetSelection(FALSE, &selected) != S_OK || !selected) return std::nullopt;
    DWORD count = 0;
    if (FAILED(selected->GetCount(&count)) || count != 1) return std::nullopt;

    ComPtr<IShellItem> item;
    if (FAILED(selected->GetItemAt(0, &item)) || !item) return std::nullopt;
    PWSTR rawItem = nullptr;
    const HRESULT itemResult = item->GetDisplayName(SIGDN_FILESYSPATH, &rawItem);
    const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> itemName(rawItem, CoTaskMemFree);
    if (FAILED(itemResult) || !itemName) return std::nullopt;

    ComPtr<IPersistFolder2> folder;
    if (FAILED(view.GetFolder(IID_PPV_ARGS(&folder))) || !folder) return std::nullopt;
    auto folderPath = FileSystemFolderPath(*folder.Get());
    fs::path itemPath(itemName.get());
    if (!folderPath || !itemPath.is_absolute() || !itemPath.has_filename() ||
        CompareNames(FolderKey(itemPath.parent_path()), FolderKey(*folderPath)) != CSTR_EQUAL) {
        return std::nullopt;
    }
    return ExplorerSelection{std::move(*folderPath), std::move(itemPath)};
}

namespace {

struct SelectionPin {
    Item item;
    std::optional<Item> stored;
};

// Call off the Explorer UI thread. This reads the store and may scan the folder.
SelectionPin InspectSelection(const PinStore& store, const ExplorerSelection& selection) {
    const auto pins = store.ReadFolder(selection.folder);
    const auto name = selection.item.filename().native();
    const auto exact = std::ranges::find(pins, name, &Item::name);
    if (exact != pins.end()) return {{name, {}}, *exact};

    auto item = ReadItem(selection.item);
    const bool possibleRename = !item.identity.empty() &&
        std::ranges::find(pins, item.identity, &Item::identity) != pins.end();
    const bool possibleCaseMatch = std::ranges::any_of(pins, [&](const Item& pin) {
        return CompareNames(pin.name, item.name) == CSTR_EQUAL;
    });
    if (!possibleRename && !possibleCaseMatch) return {std::move(item), std::nullopt};

    // Only an alias candidate needs a directory scan. A surviving old name
    // or an ambiguous file identity must not bind the old record to this item.
    std::vector<Item> entries;
    std::optional<size_t> selectedIndex;
    for (const auto& entry : fs::directory_iterator(selection.folder)) {
        const auto entryName = entry.path().filename().native();
        if (entryName == item.name) {
            selectedIndex = entries.size();
            entries.push_back(item);
        } else if (possibleRename) {
            entries.push_back(ReadItem(entry.path()));
        } else {
            entries.push_back({entryName, {}});
        }
    }
    if (selectedIndex) {
        const auto matches = ResolvePins(pins, entries);
        const auto match = std::ranges::find(matches, selectedIndex);
        if (match != matches.end()) {
            return {std::move(item), pins[static_cast<size_t>(match - matches.begin())]};
        }
    }
    return {std::move(item), std::nullopt};
}

}  // namespace

PinAction GetPinAction(const PinStore& store, const ExplorerSelection& selection) {
    return InspectSelection(store, selection).stored ? PinAction::Unpin : PinAction::Pin;
}

bool ApplyPinAction(const PinStore& store, const ExplorerSelection& selection, PinAction action) {
    const auto current = InspectSelection(store, selection);
    if (action == PinAction::Unpin) {
        return current.stored && store.SetPinned(selection.folder, *current.stored, false);
    }
    return !current.stored && store.SetPinned(selection.folder, current.item, true);
}

using PinNames = std::set<std::wstring, std::less<>>;

PinNames ReadPinNames(const PinStore& store, const fs::path& folder) {
    const auto pins = store.ReadFolder(folder);
    PinNames names;
    for (const auto& pin : pins) {
        WIN32_FIND_DATAW found{};
        HANDLE search = FindFirstFileW((folder / pin.name).c_str(), &found);
        if (search == INVALID_HANDLE_VALUE) continue;
        FindClose(search);
        if (pin.name == found.cFileName) names.insert(pin.name);
    }
    if (names.size() == pins.size()) return names;

    // Only a missing exact name needs enumeration and possible identity lookup.
    const bool lookUpIdentities = std::ranges::any_of(pins, [&](const Item& pin) {
        return !pin.identity.empty() && !names.contains(pin.name);
    });
    std::vector<Item> entries;
    for (const auto& entry : fs::directory_iterator(folder)) {
        auto name = entry.path().filename().native();
        if (lookUpIdentities) {
            entries.push_back(ReadItem(entry.path()));
        } else {
            entries.push_back({std::move(name), {}});
        }
    }
    names.clear();
    for (const auto& index : ResolvePins(pins, entries)) {
        if (index) names.insert(std::move(entries[*index].name));
    }
    return names;
}

namespace {

using CompareViewItems = int (*)(const void*, const void*, void*);

// Layout of the synchronous merge-sort request in the supported shell binary.
struct ViewSortBatch {
    int count;
    void* items;
    int itemSize;
    CompareViewItems compare;
    void* compareContext;
};

static_assert(offsetof(ViewSortBatch, compare) == 0x18);
static_assert(offsetof(ViewSortBatch, compareContext) == 0x20);

using SortViewBatch = BOOL (*)(ViewSortBatch*);

struct NativeOrdering {
    SortViewBatch sort;
    CompareViewItems compare;
};

// The private ABI and RVAs below are tied to the verified CodeView identities.
constexpr GUID storagePdb{0x6ebed4d4, 0x5734, 0x9fc4, {0x9e, 0x3e, 0x8d, 0xe5, 0x0b, 0x33, 0x0f, 0x0f}};
constexpr GUID shellPdb{0x617696ae, 0x77ca, 0x0846, {0x74, 0x3e, 0x0c, 0x34, 0x54, 0xe1, 0x7c, 0x05}};

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

[[maybe_unused]] std::optional<NativeOrdering> FindNativeOrdering(HMODULE storage) {
    constexpr DWORD sortRva = 0x1537e8;
    constexpr DWORD compareRva = 0x155190;
    const auto image = CheckedImage(storage, storagePdb, compareRva);
    if (!image) return std::nullopt;
    return NativeOrdering{reinterpret_cast<SortViewBatch>(image + sortRva),
                          reinterpret_cast<CompareViewItems>(image + compareRva)};
}

std::wstring_view LeafName(std::wstring_view path) {
    const auto separator = path.find_last_of(L"\\/");
    return separator == std::wstring_view::npos ? path : path.substr(separator + 1);
}

// The native comparer gets the child PIDL through these shell interfaces.
struct ViewCompareContext {
    IShellFolder* folder;
    IUnknown* itemResolver;
};

bool IsPinnedViewItem(const ViewCompareContext& view, const void* item, const PinNames& names) {
    using Microsoft::WRL::ComPtr;
    constexpr GUID iidPidlAccessor{0xc412bf5b, 0x91ea, 0x4904, {0xb3, 0x4a, 0x85, 0x55, 0x04, 0xfb, 0xbf, 0x0b}};
    using Bind = HRESULT (*)(IUnknown*, const void*, IShellFolder*, IBindCtx*, REFIID, void**);
    using GetId = PCUITEMID_CHILD (*)(IUnknown*);
    const auto bind = reinterpret_cast<Bind>((*reinterpret_cast<void***>(view.itemResolver))[8]);
    ComPtr<IUnknown> accessor;
    if (FAILED(bind(view.itemResolver, item, view.folder, nullptr, iidPidlAccessor,
                    reinterpret_cast<void**>(accessor.GetAddressOf()))) || !accessor) return false;
    const auto getId = reinterpret_cast<GetId>((*reinterpret_cast<void***>(accessor.Get()))[3]);
    const auto id = getId(accessor.Get());
    if (!id) return false;

    STRRET display{};
    if (FAILED(view.folder->GetDisplayNameOf(id, SHGDN_INFOLDER | SHGDN_FORPARSING, &display))) return false;
    PWSTR rawName = nullptr;
    const HRESULT nameResult = StrRetToStrW(&display, id, &rawName);
    const std::unique_ptr<wchar_t, decltype(&CoTaskMemFree)> name(rawName, CoTaskMemFree);
    if (FAILED(nameResult) || !name) return false;

    return names.contains(LeafName(name.get()));
}

struct PinnedComparison {
    CompareViewItems original;
    ViewCompareContext* view;
    const PinNames& names;

    static int Compare(const void* left, const void* right, void* context) {
        const auto& state = *static_cast<const PinnedComparison*>(context);
        const bool pinnedLeft = IsPinnedViewItem(*state.view, left, state.names);
        const bool pinnedRight = IsPinnedViewItem(*state.view, right, state.names);
        if (pinnedLeft != pinnedRight) return pinnedLeft ? -1 : 1;
        return state.original(left, right, state.view);
    }
};

[[maybe_unused]] BOOL SortWithPins(const PinStore& store, SortViewBatch sort, CompareViewItems compare,
                                  ViewSortBatch* batch) {
    if (!batch || batch->count < 2 || batch->compare != compare || !batch->compareContext) return sort(batch);

    auto* view = static_cast<ViewCompareContext*>(batch->compareContext);
    if (!view->folder || !view->itemResolver) return sort(batch);
    PinNames names;
    try {
        Microsoft::WRL::ComPtr<IPersistFolder2> folder;
        if (SUCCEEDED(view->folder->QueryInterface(IID_PPV_ARGS(&folder))) && folder) {
            if (const auto path = FileSystemFolderPath(*folder.Get())) names = ReadPinNames(store, *path);
        }
    } catch (...) {
        // A malformed store or an inaccessible folder must leave sorting available.
    }
    if (names.empty()) return sort(batch);

    PinnedComparison pinned{compare, view, names};
    // The native request lives on the caller's stack. Its scratch pointer is
    // stored immediately after these fields and must stay in the same request.
    batch->compare = PinnedComparison::Compare;
    batch->compareContext = &pinned;
    const BOOL result = sort(batch);
    batch->compareContext = view;
    batch->compare = compare;
    return result;
}

constexpr DWORD pinnedGroupId = 0x56494e50;

struct GroupList : IUnknown {
    virtual HRESULT STDMETHODCALLTYPE GetCount(UINT* count) = 0;
    virtual HRESULT STDMETHODCALLTYPE GetAt(UINT index, IUnknown** group) = 0;
};

using GroupBatch = HRESULT (*)(const unsigned char*, void*);
using AssignGroup = HRESULT (*)(IUnknown*, IUnknown*, void*, void*, DWORD);
using GroupInfo = HRESULT (*)(IUnknown*, DWORD, IUnknown**);
using ApplyGroups = HRESULT (*)(const unsigned char*, GroupList*, UINT*);
using SetGrouping = HRESULT (*)(IFolderView2*, REFPROPERTYKEY, BOOL);
using RegisterGroup = HRESULT (*)(IUnknown*, DWORD);
using MakeGroup = HRESULT (*)(DWORD, DWORD, const PROPERTYKEY*, DWORD, IUnknown*, PCWSTR, IUnknown**);
using GroupDirection = BOOL (*)(IUnknown*);

struct NativeGrouping {
    GroupBatch batch;
    AssignGroup assign;
    GroupInfo rangeInfo;
    GroupInfo textInfo;
    ApplyGroups apply;
    SetGrouping setGrouping;
    RegisterGroup remember;
    MakeGroup make;
    GroupDirection ascending;
    const void* rangeCategory;
    const void* textCategory;
};

[[maybe_unused]] std::optional<NativeGrouping> FindNativeGrouping(HMODULE storage, HMODULE shell) {
    const auto image = CheckedImage(storage, storagePdb, 0x5bcb30);
    const auto frontend = CheckedImage(shell, shellPdb, 0x2e8ae0);
    if (!image || !frontend) return std::nullopt;
    return NativeGrouping{
        reinterpret_cast<GroupBatch>(image + 0x153e50), reinterpret_cast<AssignGroup>(image + 0x1641f0),
        reinterpret_cast<GroupInfo>(image + 0x22fc90), reinterpret_cast<GroupInfo>(image + 0x16f0c0),
        reinterpret_cast<ApplyGroups>(image + 0x16eab8), reinterpret_cast<SetGrouping>(frontend + 0x2e8ae0),
        reinterpret_cast<RegisterGroup>(image + 0x1642dc), reinterpret_cast<MakeGroup>(image + 0x22fd4c),
        reinterpret_cast<GroupDirection>(image + 0x16ec10), image + 0x248730, image + 0x5bcb30,
    };
}

PCWSTR PinnedGroupTitle(LANGID language) {
    switch (PRIMARYLANGID(language)) {
        case LANG_RUSSIAN: return L"Закрепленные";
        case LANG_SPANISH: return L"Anclados";
        case LANG_GERMAN: return L"Angeheftet";
        case LANG_PORTUGUESE:
            if (SUBLANGID(language) == SUBLANG_PORTUGUESE_BRAZILIAN) return L"Fixados";
            break;
        case LANG_CHINESE:
            if (SUBLANGID(language) == SUBLANG_CHINESE_SIMPLIFIED) return L"已固定";
            break;
    }
    return L"Pinned";
}

// A grouping worker runs synchronously inside its batch. Each view gets its
// own snapshot, including when Explorer groups the same folder in two tabs.
class GroupingPins {
public:
    explicit GroupingPins(const PinNames& names) : previous_(current_) { current_ = &names; }
    ~GroupingPins() { current_ = previous_; }
    GroupingPins(const GroupingPins&) = delete;
    GroupingPins& operator=(const GroupingPins&) = delete;

    static const PinNames* Current() { return current_; }

private:
    inline static thread_local const PinNames* current_ = nullptr;
    const PinNames* previous_;
};

[[maybe_unused]] HRESULT GroupWithPins(const PinStore& store, GroupBatch original,
                                       const unsigned char* batch, void* result) {
    PinNames names;
    try {
        // The grouping request holds the backing folder's PIDL provider here.
        auto* owner = *reinterpret_cast<IUnknown* const*>(batch + 0x20);
        Microsoft::WRL::ComPtr<IPersistIDList> source;
        if (owner && SUCCEEDED(owner->QueryInterface(IID_PPV_ARGS(&source))) && source) {
            PIDLIST_ABSOLUTE raw = nullptr;
            const auto idResult = source->GetIDList(&raw);
            const std::unique_ptr<std::remove_pointer_t<PIDLIST_ABSOLUTE>, decltype(&CoTaskMemFree)>
                id(raw, CoTaskMemFree);
            if (SUCCEEDED(idResult)) {
                if (const auto folder = FileSystemPath(id.get())) names = ReadPinNames(store, *folder);
            }
        }
    } catch (...) {
        // Keep the shell's groups if the folder or store cannot be read.
    }
    const GroupingPins scope(names);
    return original(batch, result);
}

bool IsPinnedGroupItem(IUnknown* row, const PinNames& names) {
    using ReadValue = HRESULT (*)(IUnknown*, REFPROPERTYKEY, PROPVARIANT*);
    const auto read = reinterpret_cast<ReadValue>((*reinterpret_cast<void***>(row))[3]);
    PROPVARIANT path{};
    const HRESULT result = read(row, PKEY_ParsingPath, &path);
    const bool pinned = SUCCEEDED(result) && path.vt == VT_LPWSTR && path.pwszVal &&
                        names.contains(LeafName(path.pwszVal));
    PropVariantClear(&path);
    return pinned;
}

[[maybe_unused]] HRESULT AssignWithPins(const NativeGrouping& api, IUnknown* category, IUnknown* row,
                                        void* item, void* context, DWORD flags) {
    const auto* names = GroupingPins::Current();
    const auto categoryMethod = (*reinterpret_cast<void***>(category))[6];
    if (!names || names->empty() || !row ||
        (categoryMethod != api.rangeCategory && categoryMethod != api.textCategory) ||
        !IsPinnedGroupItem(row, *names)) return api.assign(category, row, item, context, flags);

    using SetCategory = HRESULT (*)(IUnknown*, void*, const PROPVARIANT*, DWORD);
    const auto set = reinterpret_cast<SetCategory>((*reinterpret_cast<void***>(row))[5]);
    PROPVARIANT group{};
    group.vt = VT_UI4;
    group.ulVal = pinnedGroupId;
    const auto result = set(row, item, &group, flags);
    return SUCCEEDED(result) ? api.remember(category, pinnedGroupId) : result;
}

[[maybe_unused]] HRESULT GroupInfoWithPins(MakeGroup make, GroupInfo original, IUnknown* category,
                                           DWORD group, IUnknown** output) {
    if (group != pinnedGroupId) return original(category, group, output);
    const auto* property = reinterpret_cast<const PROPERTYKEY*>(reinterpret_cast<const unsigned char*>(category) + 0x18);
    return make(group, 0, property, 0, nullptr, PinnedGroupTitle(GetUserDefaultUILanguage()), output);
}

class ReorderedGroupList final : public GroupList {
public:
    ReorderedGroupList(GroupList& original, UINT pinned, UINT target)
        : original_(original), pinned_(pinned), target_(target) {}

    HRESULT STDMETHODCALLTYPE QueryInterface(REFIID iid, void** output) override {
        *output = nullptr;
        if (iid != IID_IUnknown) return E_NOINTERFACE;
        *output = static_cast<GroupList*>(this);
        return S_OK;
    }
    ULONG STDMETHODCALLTYPE AddRef() override { return 1; }
    ULONG STDMETHODCALLTYPE Release() override { return 1; }
    HRESULT STDMETHODCALLTYPE GetCount(UINT* count) override { return original_.GetCount(count); }
    HRESULT STDMETHODCALLTYPE GetAt(UINT index, IUnknown** group) override {
        if (index == target_) return original_.GetAt(pinned_, group);
        const UINT withoutPinned = index - (index > target_ ? 1 : 0);
        return original_.GetAt(withoutPinned + (withoutPinned >= pinned_ ? 1 : 0), group);
    }

private:
    GroupList& original_;
    UINT pinned_;
    UINT target_;
};

std::optional<DWORD> NativeGroupId(GroupList& groups, UINT index) {
    Microsoft::WRL::ComPtr<IUnknown> group;
    if (FAILED(groups.GetAt(index, group.GetAddressOf())) || !group) return std::nullopt;
    using GetId = HRESULT (*)(IUnknown*, DWORD*);
    const auto read = reinterpret_cast<GetId>((*reinterpret_cast<void***>(group.Get()))[3]);
    DWORD id = 0;
    return SUCCEEDED(read(group.Get(), &id)) ? std::optional(id) : std::nullopt;
}

[[maybe_unused]] HRESULT ApplyWithPins(const NativeGrouping& api, const unsigned char* view, GroupList* groups,
                                       UINT* added) {
    UINT count = 0;
    if (!groups || FAILED(groups->GetCount(&count))) return api.apply(view, groups, added);
    for (UINT i = 0; i < count; ++i) {
        if (NativeGroupId(*groups, i) != pinnedGroupId) continue;

        const auto settings = *reinterpret_cast<IUnknown* const*>(view + 0x268);
        if (!settings) break;
        // The native consumer reads this list in reverse when descending.
        const UINT target = api.ascending(settings) ? 0 : count - 1;
        if (target == i) break;
        ReorderedGroupList reordered(*groups, i, target);
        return api.apply(view, &reordered, added);
    }
    return api.apply(view, groups, added);
}

[[maybe_unused]] HRESULT SetGroupingWithPins(const PinStore& store, SetGrouping original, IFolderView2* view,
                                             REFPROPERTYKEY key, BOOL ascending) {
    PROPERTYKEY previous{};
    BOOL previousAscending = FALSE;
    const bool flips = view->GetGroupBy(&previous, &previousAscending) == S_OK &&
                       IsEqualPropertyKey(key, previous) && (ascending != FALSE) != (previousAscending != FALSE);
    Microsoft::WRL::ComPtr<IShellView> refresh;
    if (flips) {
        try {
            Microsoft::WRL::ComPtr<IPersistFolder2> folder;
            if (SUCCEEDED(view->GetFolder(IID_PPV_ARGS(&folder))) && folder) {
                if (const auto path = FileSystemFolderPath(*folder.Get()); path && !store.ReadFolder(*path).empty()) {
                    view->QueryInterface(IID_PPV_ARGS(&refresh));
                }
            }
        } catch (...) {
            // The direction change remains available if the store is unreadable.
        }
    }
    const auto result = original(view, key, ascending);
    // A direction-only change normally reverses cached groups. Rebuild the
    // pinned folder's presentation with the same grouping key and new order.
    if (SUCCEEDED(result) && refresh) refresh->Refresh();
    return result;
}

#ifdef WH_MOD
std::optional<PinStore> nativePinStore;
SortViewBatch nativeSortOriginal = nullptr;
CompareViewItems nativeCompare = nullptr;
NativeGrouping nativeGrouping{};

BOOL NativeSortHook(ViewSortBatch* batch) {
    return SortWithPins(*nativePinStore, nativeSortOriginal, nativeCompare, batch);
}

HRESULT NativeGroupBatchHook(const unsigned char* batch, void* result) {
    return GroupWithPins(*nativePinStore, nativeGrouping.batch, batch, result);
}

HRESULT NativeAssignGroupHook(IUnknown* category, IUnknown* row, void* item, void* context, DWORD flags) {
    return AssignWithPins(nativeGrouping, category, row, item, context, flags);
}

HRESULT NativeRangeInfoHook(IUnknown* category, DWORD group, IUnknown** output) {
    return GroupInfoWithPins(nativeGrouping.make, nativeGrouping.rangeInfo, category, group, output);
}

HRESULT NativeTextInfoHook(IUnknown* category, DWORD group, IUnknown** output) {
    return GroupInfoWithPins(nativeGrouping.make, nativeGrouping.textInfo, category, group, output);
}

HRESULT NativeApplyGroupsHook(const unsigned char* view, GroupList* groups, UINT* added) {
    return ApplyWithPins(nativeGrouping, view, groups, added);
}

HRESULT NativeSetGroupingHook(IFolderView2* view, REFPROPERTYKEY key, BOOL ascending) {
    return SetGroupingWithPins(*nativePinStore, nativeGrouping.setGrouping, view, key, ascending);
}

template <typename Function>
bool HookFunction(Function target, Function callback, Function& original) {
    return Wh_SetFunctionHook(reinterpret_cast<void*>(target), reinterpret_cast<void*>(callback),
                              reinterpret_cast<void**>(&original));
}
#endif

}  // namespace

}  // namespace explorer_item_pins

BOOL Wh_ModInit() {
#ifdef WH_MOD
    using namespace explorer_item_pins;
    const auto storage = GetModuleHandleW(L"Windows.Storage.dll");
    const auto native = FindNativeOrdering(storage);
    const auto grouping = FindNativeGrouping(storage, GetModuleHandleW(L"Shell32.dll"));
    if (!native || !grouping) return FALSE;

    try {
        nativePinStore.emplace(PinStore::DefaultPath());
    } catch (...) {
        return FALSE;
    }
    nativeCompare = native->compare;
    nativeGrouping = *grouping;
    return HookFunction(native->sort, NativeSortHook, nativeSortOriginal) &&
           HookFunction(grouping->batch, NativeGroupBatchHook, nativeGrouping.batch) &&
           HookFunction(grouping->assign, NativeAssignGroupHook, nativeGrouping.assign) &&
           HookFunction(grouping->rangeInfo, NativeRangeInfoHook, nativeGrouping.rangeInfo) &&
           HookFunction(grouping->textInfo, NativeTextInfoHook, nativeGrouping.textInfo) &&
           HookFunction(grouping->apply, NativeApplyGroupsHook, nativeGrouping.apply) &&
           HookFunction(grouping->setGrouping, NativeSetGroupingHook, nativeGrouping.setGrouping);
#else
    return FALSE;
#endif
}

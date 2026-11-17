#include <windhawk_api.h>
#include <gdiplus.h>
#include <cstdio>
#include <filesystem>
#include <stdexcept>
#include "../Mod/cursor-scheme-extension.wh.cpp"

namespace {
constexpr uint32_t background = 0x000d1117;
std::filesystem::path output;
CLSID pngEncoder;
int count;

void Save(HCURSOR cursor, const wchar_t* name) {
    auto image = ReadCursor(cursor, false);
    if (!image) throw std::runtime_error("ReadCursor failed");
    const int width = image->width, height = image->height;
    BITMAPINFO info{};
    info.bmiHeader = {sizeof(BITMAPINFOHEADER), width, -height, 1, 32, BI_RGB};
    void* pixels = nullptr;
    HBITMAP bitmap = CreateDIBSection(nullptr, &info, DIB_RGB_COLORS, &pixels, nullptr, 0);
    HDC dc = CreateCompatibleDC(nullptr);
    HGDIOBJ previous = SelectObject(dc, bitmap);
    std::fill_n(static_cast<uint32_t*>(pixels), size_t(width) * height, background);
    if (!DrawIconEx(dc, 0, 0, cursor, width, height, 0, nullptr, DI_NORMAL))
        throw std::runtime_error("DrawIconEx failed");
    GdiFlush();
    auto* data = static_cast<uint32_t*>(pixels);
    int left = width, top = height, right = -1, bottom = -1;
    for (int y = 0; y < height; ++y) {
        for (int x = 0; x < width; ++x) {
            auto& pixel = data[y * width + x];
            if ((pixel & 0xffffff) != background) {
                left = std::min(left, x); top = std::min(top, y);
                right = std::max(right, x); bottom = std::max(bottom, y);
            }
            pixel |= 0xff000000;
        }
    }
    if (right < 0) throw std::runtime_error("Empty cursor");
    Gdiplus::Bitmap full(width, height, width * 4, PixelFormat32bppARGB,
                         reinterpret_cast<BYTE*>(pixels));
    std::unique_ptr<Gdiplus::Bitmap> cropped(full.Clone(left, top, right-left+1,
                                                       bottom-top+1, PixelFormat32bppARGB));
    const auto path = output / (std::wstring(name) + L".png");
    if (!cropped || cropped->Save(path.c_str(), &pngEncoder, nullptr) != Gdiplus::Ok)
        throw std::runtime_error("PNG save failed");
    std::wprintf(L"%ls: canvas %dx%d, ink %dx%d\n", name, width, height,
                 right-left+1, bottom-top+1);
    SelectObject(dc, previous); DeleteDC(dc); DeleteObject(bitmap);
    ++count;
}

HCURSOR LoadFile(const std::filesystem::path& path) {
    const auto cursor = static_cast<HCURSOR>(LoadImageW(nullptr, path.c_str(), IMAGE_CURSOR,
                                                       32, 32, LR_LOADFROMFILE));
    if (!cursor) {
        std::fwprintf(stderr, L"Cannot load %ls: %lu\n", path.c_str(), GetLastError());
        throw std::runtime_error("LoadImage failed");
    }
    return cursor;
}

void After(const std::filesystem::path& path, const wchar_t* name,
           CursorMapping mapping, bool smooth) {
    HCURSOR original = LoadFile(path);
    auto image = ReadCursor(original, smooth);
    if (!image) throw std::runtime_error("Read theme cursor failed");
    HCURSOR transformed = MakeCursor(TransformCursor(*image, mapping, smooth));
    if (!transformed) throw std::runtime_error("MakeCursor failed");
    Save(transformed, name);
    DestroyCursor(transformed); DestroyCursor(original);
}

bool officeHand, officeResize, officeCell;
BOOL CALLBACK OfficeResource(HMODULE module, PCWSTR, PWSTR name, LONG_PTR) {
    const auto image = LoadResourceImage(module, name);
    if (!image) return TRUE;
    const auto shape = MakeShape(*image);
    const auto slot = FindOfficeSlot(shape);
    const wchar_t* label = nullptr;
    if (!officeHand && slot == SystemSlot(kLinkId)) {
        officeHand = true; label = L"before-hand";
    } else if (!officeResize && slot == SystemSlot(kHorizontalResizeId)) {
        officeResize = true; label = L"before-resize";
    } else if (!officeCell && slot == SystemSlot(kCrossId)) {
        officeCell = true; label = L"before-cell";
    }
    if (label) {
        const auto cursor = static_cast<HCURSOR>(LoadImageW(module, name, IMAGE_CURSOR, 32, 32, 0));
        std::wprintf(L"Office resource %ls%u -> %ls\n", IS_INTRESOURCE(name) ? L"#" : name,
                     IS_INTRESOURCE(name) ? LOWORD(name) : 0, label);
        Save(cursor, label); DestroyCursor(cursor);
    }
    return TRUE;
}
}

int wmain(int argc, wchar_t** argv) {
    try {
        // output, Windows cursor directory, W11 directory, Vision directory, Office executable
        if (argc != 6) return 2;
        output = argv[1];
        std::filesystem::create_directories(output);
        ULONG_PTR gdiplus;
        Gdiplus::GdiplusStartupInput startup;
        if (Gdiplus::GdiplusStartup(&gdiplus, &startup, nullptr) != Gdiplus::Ok) return 3;
        CLSIDFromString(L"{557CF406-1A04-11D3-9A73-0000F81EF32E}", &pngEncoder);

        {
            HMODULE module = LoadLibraryExW(argv[5], nullptr,
                          LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
            if (!module) throw std::runtime_error("Cannot read Office resources");
            std::wprintf(L"Scanning Office resources\n");
            EnumResourceNamesW(module, RT_GROUP_CURSOR, OfficeResource, 0);
            FreeLibrary(module);
        }
        if (!officeHand || !officeResize || !officeCell)
            throw std::runtime_error("Required Office cursor not found");

        constexpr std::array ids = {32652, 32653, 32654, 32655, 32662};
        constexpr std::array names = {L"before-origin-vertical", L"before-origin-horizontal",
            L"before-origin-both", L"before-up", L"before-diagonal"};
        for (size_t i = 0; i < ids.size(); ++i) {
            HCURSOR cursor = LoadCursorW(nullptr, MAKEINTRESOURCEW(ids[i]));
            if (!cursor) throw std::runtime_error("Missing USER32 autoscroll cursor");
            Save(cursor, names[i]);
        }

        struct Theme {const wchar_t* name; const wchar_t* folder; const wchar_t* prefix;};
        const std::array themes = {
            Theme{L"aero", argv[2], L"aero_"},
            Theme{L"w11", argv[3], L""},
            Theme{L"vision", argv[4], L""}};
        for (size_t i = 0; i < themes.size(); ++i) {
            const auto& theme = themes[i];
            auto role = [&](const wchar_t* file) {
                return std::filesystem::path(theme.folder) / (std::wstring(theme.prefix) + file + L".cur");
            };
            auto label = [&](const wchar_t* row) {return std::wstring(L"after-") + theme.name + L"-" + row;};
            After(role(L"link"), label(L"hand").c_str(), {kLinkId, Role::Link, CursorType::Link}, false);
            After(role(i == 0 ? L"ns" : i == 1 ? L"ew" : L"move"), label(L"origin").c_str(),
                  kMappings[i], true);
            After(role(L"up"), label(L"up").c_str(), kMappings[3], true);
            After(role(L"up"), label(L"diagonal").c_str(), kMappings[10], true);
            After(role(L"ew"), label(L"resize").c_str(),
                  {kHorizontalResizeId, Role::Horizontal, CursorType::Resize}, false);
            if (i) {
                After(role(L"cross"), label(L"cell").c_str(), {kCrossId, Role::Cross, CursorType::Cell}, false);
            } else {
                HMODULE native = LoadLibraryExW(L"user32.dll", nullptr,
                                  LOAD_LIBRARY_AS_DATAFILE | LOAD_LIBRARY_AS_IMAGE_RESOURCE);
                // Aero's empty Crosshair scheme entry uses USER32's native precision cursor.
                // Its file resource ID is 103; IDC_CROSS would load the active personal scheme.
                const auto cursor = static_cast<HCURSOR>(LoadImageW(native, MAKEINTRESOURCEW(103),
                                                        IMAGE_CURSOR, 32, 32, 0));
                if (!cursor) throw std::runtime_error("Missing native precision select resource");
                Save(cursor, label(L"cell").c_str());
                DestroyCursor(cursor);
                FreeLibrary(native);
            }
        }
        Gdiplus::GdiplusShutdown(gdiplus);
        std::printf("Exported %d sprites\n", count);
        return count == 26 ? 0 : 4;
    } catch (const std::exception& error) {
        std::fprintf(stderr, "%s\n", error.what());
        return 1;
    }
}

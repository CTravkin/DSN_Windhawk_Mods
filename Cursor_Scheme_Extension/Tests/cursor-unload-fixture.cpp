#include <windhawk_api.h>
#include "../Mod/cursor-scheme-extension.wh.cpp"

extern "C" __declspec(dllexport) void FixtureStart() {
    BeginCursorSession();
    g_setCursorOriginal = SetCursor;
    g_getCursorOriginal = GetCursor;
    g_enabledTypes = UINT32_MAX;
    g_enabledTargets = UINT32_MAX;
    g_isOffice = true;
    g_smoothingTypes = TypeBit(CursorType::SelectionArrows);
}

extern "C" __declspec(dllexport) HCURSOR FixtureOriginal() {
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
                image.color[index] = 0xff456789;
            }
        }
    }
    return MakeCursor(image);
}

extern "C" __declspec(dllexport) HCURSOR FixtureSelect(HCURSOR original) {
    SetCursorHook(original);
    return GetCursor();
}

extern "C" __declspec(dllexport) void FixtureStop() {
    Wh_ModBeforeUninit();
    Wh_ModUninit();
}

#include <array>
#include <cassert>
#include <span>
#include <string>
#include <string_view>

#include "../Mod/Source/QuickKeys.h"

int main() {
    using enum QuickKeyMode;
    using enum QuickKeyAction;
    QuickKeyCycle cycle;
    constexpr auto none = QuickKeyDecision{};
    const auto expect = [&](std::span<QuickKeyTarget const> rows, QuickKeyMode mode,
                            wchar_t key, QuickKeyAction action, size_t index) {
        assert((ChooseQuickKey(rows, mode, key, cycle) == QuickKeyDecision{action, index}));
    };

    // A root item and two commands belonging to different submenus.
    std::array rows{QuickKeyTarget{L'П', true}, QuickKeyTarget{L'П', false, true},
                    QuickKeyTarget{L'П', false, true}, QuickKeyTarget{L'М', false, true}};
    assert(ChooseQuickKey(rows, Off, L'П', cycle) == none);
    expect(rows, Visible, L'П', Launch, 0);
    assert(ChooseQuickKey(rows, Visible, L'М', cycle) == none);
    expect(rows, All, L'П', Focus, 0);
    expect(rows, All, L'П', Focus, 1);
    rows[1].visible = true;
    expect(rows, All, L'П', Focus, 2);
    rows[1].visible = false;
    rows[2].visible = true;
    expect(rows, All, L'П', Focus, 0);
    cycle = {};
    expect(rows, Visible, L'П', Focus, 2);
    expect(rows, Visible, L'П', Focus, 0);
    expect(rows, All, L'М', Focus, 3);
    rows[3].visible = true;
    expect(rows, All, L'М', Launch, 3);

    // An opener leaves the matching set once expanded. The next press must
    // focus its child even when it is now the only eligible match.
    std::array power{QuickKeyTarget{L'З', true, false, true},
                     QuickKeyTarget{L'З', false, true}};
    expect(power, All, L'З', Expand, 0);
    power[0].excluded = true;
    power[1].visible = true;
    expect(power, All, L'З', Focus, 1);
    expect(power, All, L'З', Launch, 1);
    power[0].excluded = false;
    power[1].visible = false;
    expect(power, Visible, L'З', Expand, 0);
    cycle = {};
    expect(rows, All, L'П', Focus, 2);

    // Preserve the system position for identical titles and locate the key
    // again when the mod uses the resource title from a different surface.
    const auto fold = [](wchar_t ch) { return ch == L'м' ? L'М' : ch == L'е' ? L'Е' : ch; };
    assert((MarkQuickKey(L"Систе&ма", L"Система", L'М', fold) ==
            QuickKeyMark{L"Система", 5}));
    assert((MarkQuickKey(L"Систе&ма", L"О системе", L'М', fold) ==
            QuickKeyMark{L"О системе", 7}));
    assert((MarkQuickKey(L"&Электропитание", L"Питание", L'Э', fold) ==
            QuickKeyMark{L"Питание (Э)", 9}));
    assert((MarkQuickKey(L"设备管理器(&M)", L"设备管理器", L'M', fold) ==
            QuickKeyMark{L"设备管理器 (M)", 7}));
}

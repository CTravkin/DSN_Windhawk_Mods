enum class QuickKeyMode { Off, Visible, All };
enum class QuickKeyAction { Ignore, Launch, Focus, Expand };

struct QuickKeyTarget {
    wchar_t key{};
    bool visible{};
    bool nested{};
    bool opener{};
    bool excluded{};
};

struct QuickKeyDecision {
    QuickKeyAction action{QuickKeyAction::Ignore};
    size_t index{};
    bool operator==(QuickKeyDecision const&) const = default;
};

struct QuickKeyCycle {
    wchar_t key{};
    size_t index{};
    QuickKeyAction last{QuickKeyAction::Ignore};
};

QuickKeyDecision ChooseQuickKey(std::span<QuickKeyTarget const> rows, QuickKeyMode mode,
                               wchar_t key, QuickKeyCycle& cycle) {
    if (mode == QuickKeyMode::Off || !key) {
        cycle = {};
        return {};
    }
    const bool repeating = cycle.key == key;
    size_t count{};
    size_t first = rows.size();
    size_t firstMatch = rows.size();
    size_t next = rows.size();
    int firstPriority = -1;
    for (size_t index = 0; index < rows.size(); ++index) {
        const auto& row = rows[index];
        if (row.key != key || row.excluded || (mode == QuickKeyMode::Visible && !row.visible)) continue;
        if (!count++) firstMatch = index;
        const int priority = row.visible ? (row.nested ? 2 : 1) : 0;
        if (priority > firstPriority) {
            first = index;
            firstPriority = priority;
        }
        if (repeating && index > cycle.index && next == rows.size()) next = index;
    }
    if (!count) {
        cycle = {};
        return {};
    }

    // Wrap in menu order, independent of which submenu the cycle opened.
    const size_t selected = repeating && count > 1 ?
        (next == rows.size() ? firstMatch : next) : first;
    const auto& row = rows[selected];
    if (row.opener && row.visible) {
        cycle = {key, selected, QuickKeyAction::Expand};
        return {QuickKeyAction::Expand, selected};
    }
    if (count == 1 && row.visible &&
        (!repeating || (cycle.last == QuickKeyAction::Focus && cycle.index == selected))) {
        cycle = {};
        return {QuickKeyAction::Launch, selected};
    }
    cycle = {key, selected, QuickKeyAction::Focus};
    return {QuickKeyAction::Focus, selected};
}

struct QuickKeyMark {
    std::wstring caption;
    size_t index{};
    bool operator==(QuickKeyMark const&) const = default;
};

template<typename Fold>
QuickKeyMark MarkQuickKey(std::wstring_view resource, std::wstring_view caption, wchar_t key, Fold fold) {
    std::wstring plain;
    auto nativeIndex = std::wstring::npos;
    for (size_t index = 0; index < resource.size(); ++index) {
        const auto ch = resource[index];
        if (ch == L'&' && index + 1 < resource.size()) {
            const auto next = resource[++index];
            if (next != L'&' && fold(next) == fold(key) && nativeIndex == std::wstring::npos)
                nativeIndex = plain.size();
            plain += next;
        } else {
            plain += ch;
        }
    }
    if (plain == caption && nativeIndex != std::wstring::npos) return {std::move(plain), nativeIndex};
    for (size_t index = 0; index < caption.size(); ++index) {
        if (fold(caption[index]) == fold(key)) return {std::wstring(caption), index};
    }
    auto marked = std::wstring(caption) + L" (";
    const auto index = marked.size();
    marked += key;
    marked += L')';
    return {std::move(marked), index};
}

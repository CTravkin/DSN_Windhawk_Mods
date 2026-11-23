// The estimated-duration resources use unsigned integers in otherwise exact captions.
bool MatchesPowerCaption(std::wstring_view caption, std::wstring_view pattern) {
    if (pattern.empty()) return false;
    for (auto slot = pattern.find(L"%u"); slot != pattern.npos; slot = pattern.find(L"%u")) {
        if (!caption.starts_with(pattern.substr(0, slot))) return false;
        caption.remove_prefix(slot);
        const auto digits = caption.find_first_not_of(L"0123456789");
        if (digits == 0) return false;
        caption.remove_prefix(digits == caption.npos ? caption.size() : digits);
        pattern.remove_prefix(slot + 2);
    }
    return caption == pattern;
}

enum class PowerPlacement { Hidden, Main, Submenu };

constexpr PowerPlacement PlacePowerAction(bool withUpdate, bool primary, bool enabled) {
    if (primary && !withUpdate) return PowerPlacement::Main;
    return enabled ? PowerPlacement::Submenu : PowerPlacement::Hidden;
}

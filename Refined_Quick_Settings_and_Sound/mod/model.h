#pragma once
#include <algorithm>
#include <array>
#include <cmath>
#include <cwctype>
#include <map>
#include <string>
#include <string_view>
#include <vector>
#include <windows.h>
namespace sound {
inline PCWSTR Text(PCWSTR english, PCWSTR russian, PCWSTR spanish, PCWSTR german,
                   PCWSTR portuguese, PCWSTR chinese) {
  switch (PRIMARYLANGID(GetUserDefaultUILanguage())) {
  case LANG_RUSSIAN: return russian;
  case LANG_SPANISH: return spanish;
  case LANG_GERMAN: return german;
  case LANG_PORTUGUESE: return portuguese;
  case LANG_CHINESE: return chinese;
  default: return english;
  }
}

enum class QuickShortcut { None, Display, Night, Network, Bluetooth, Apps };
using QuickShortcutSlots = std::array<QuickShortcut, 5>;
inline QuickShortcut ParseQuickShortcut(std::wstring_view value) {
  if (value == L"display") return QuickShortcut::Display;
  if (value == L"night") return QuickShortcut::Night;
  if (value == L"network") return QuickShortcut::Network;
  if (value == L"bluetooth") return QuickShortcut::Bluetooth;
  if (value == L"apps") return QuickShortcut::Apps;
  return QuickShortcut::None;
}
inline std::vector<QuickShortcut> QuickShortcuts(QuickShortcutSlots const &slots,
                                                bool displayBelow, bool nightBelow) {
  std::vector<QuickShortcut> result;
  for (auto action : slots)
    if (action != QuickShortcut::None &&
        !(action == QuickShortcut::Display && displayBelow) &&
        !(action == QuickShortcut::Night && nightBelow) &&
        std::find(result.begin(), result.end(), action) == result.end())
      result.push_back(action);
  return result;
}

// Adapted from the hidden-applications parser of the Sound Output Direct and
// Taller Windhawk mod; the only code taken from it.
// Matching happens before a row is created, so hidden apps occupy no space.
inline std::wstring Lower(std::wstring value) {
  if (!value.empty())
    CharLowerBuffW(value.data(), static_cast<DWORD>(value.size()));
  return value;
}
inline std::vector<std::wstring> ParseHidden(std::wstring const &text) {
  std::vector<std::wstring> result;
  size_t start = 0;
  while (start < text.size()) {
    auto end = text.find_first_of(L";,\r\n", start);
    if (end == text.npos)
      end = text.size();
    auto part = text.substr(start, end - start);
    auto first = part.find_first_not_of(L" \t");
    if (first != part.npos)
      result.push_back(Lower(part.substr(first, part.find_last_not_of(L" \t") - first + 1)));
    start = end + 1;
  }
  return result;
}
inline bool Hidden(std::vector<std::wstring> const &patterns, std::wstring const &name,
                   std::wstring const &path) {
  auto text = Lower(name + L" " + path);
  return std::any_of(patterns.begin(), patterns.end(),
                     [&](auto const &p) { return text.find(p) != text.npos; });
}
inline float Clamp(float value) { return std::clamp(value, 0.f, 1.f); }
inline int VolumeIcon(float value, bool mute) {
  return mute || value <= 0 ? 0 : value <= .33f ? 1 : value <= .66f ? 2 : 3;
}
struct SavedVolume {
  float last = .5f;
  void Observe(float value, bool) {
    if (value > 0)
      last = Clamp(value);
  }
  float Toggle(float current, bool muted) {
    Observe(current, muted);
    return !muted && current > 0 ? 0 : last;
  }
};
struct Device {
  std::wstring id, name;
  bool current = false;
};
struct Snapshot {
  uint64_t masterNoticeAt = 0, masterRevision = 0;
  std::vector<Device> output, input;
  std::wstring outputId, inputId, error;
  float master = 0;
  bool muted = false, masterAvailable = false;
  unsigned long long revision = 0;
  unsigned long long applied = 0;
};
enum class Action { MasterVolume, MasterMute, Input, Refresh };
struct Command {
  Action action = Action::Refresh;
  std::wstring id;
  float value = 0;
  unsigned long long serial = 0;
};
} // namespace sound

#pragma once
#include "common.h"

namespace native {
inline ModSettings ReadPanelSettings(auto readInt, auto readString) {
  ModSettings settings;
  // Windhawk keeps old paths and adds the initial values of new paths on upgrade.
  // Until the legacy entries are moved in the editor, an unchanged new default
  // can take its effective value from the old path.
  auto stringSetting = [&](PCWSTR key, std::wstring_view initial, PCWSTR legacyKey) {
    auto value = readString(key);
    if (value.empty() || value == initial) {
      auto previous = readString(legacyKey);
      if (!previous.empty())
        return previous;
    }
    return value.empty() ? std::wstring(initial) : value;
  };

  auto brightness = readString(L"quickSettings.brightnessMode");
  auto oldBrightness = readString(L"brightnessMode");
  bool hasGroupedDefaults = !brightness.empty();
  // Both settings were present in every DSN schema with the old taskbar fields.
  bool hasLegacyTaskbar = !oldBrightness.empty() || !readString(L"quickSettings.shortcut1").empty();
  auto intSetting = [&](PCWSTR key, int initial, PCWSTR legacyKey) {
    auto value = hasGroupedDefaults ? readInt(key) : initial;
    return value == initial && hasLegacyTaskbar ? readInt(legacyKey) : value;
  };

  settings.quickSoundAccess = intSetting(L"taskbar.openSoundFromVolumeIcon", 1, L"quickSoundAccess") != 0;
  settings.trayMiddleClickMute = intSetting(L"taskbar.middleClickMute", 0, L"trayMiddleClickMute") != 0;
  settings.volumeWheelStep = sound::VolumeWheelStep(intSetting(L"sound.volumeWheelStep", 2, L"volumeWheelStep"));
  settings.brightnessEnabled = (brightness.empty() || brightness == L"auto" ? oldBrightness
                                                                            : brightness) != L"disabled";
  settings.pillQuickTiles = stringSetting(L"quickSettings.tileShape", L"default", L"appearance.quickTiles") == L"pills";
  settings.pillSoundPickers = stringSetting(L"sound.selectorShape", L"default", L"appearance.soundPickers") == L"pills";

  auto microphone = stringSetting(L"sound.microphoneVolumeMode", L"disabled", L"microphoneVolume");
  auto microphoneRight = readString(L"sound.microphoneVolumeRight");
  auto systemRight = readString(L"sound.systemVolumeRight");
  if ((systemRight.empty() || systemRight == L"soundSettings") &&
      readString(L"soundSettingsPosition") == L"header")
    systemRight = L"deviceSettings";
  auto spatial = stringSetting(L"sound.spatialSoundMenu", L"enabled", L"spatialSoundMenu");
  settings.sound = sound::ParseSoundOptions(microphone, microphoneRight, systemRight, spatial);

  auto oldRowKey = readString(L"appearance.quickActionRows").empty() ? L"quickActionRows"
                                                                   : L"appearance.quickActionRows";
  auto rows = stringSetting(L"quickSettings.tileRows", L"2", oldRowKey);
  if (rows.size() == 1 && rows.front() >= L'1' && rows.front() <= L'4')
    settings.quickActionRows = rows.front() - L'0';

  for (size_t i = 0; i < settings.quickShortcuts.size(); ++i) {
    auto slot = std::to_wstring(i + 1);
    auto value = stringSetting((L"quickSettings.additionalShortcuts.shortcut" + slot).c_str(),
                               i == 0 ? L"display" : L"none",
                               (L"quickSettings.shortcut" + slot).c_str());
    settings.quickShortcuts[i] = sound::ParseQuickShortcut(value);
  }
  if (!hasGroupedDefaults && !hasLegacyTaskbar)
    settings.quickShortcuts = {readInt(L"showDisplaySettings") ? sound::QuickShortcut::Display
                                                            : sound::QuickShortcut::None};

  auto readHidden = [&](PCWSTR path) {
    for (int i = 0; i < 32; ++i) {
      auto key = path + (L"[" + std::to_wstring(i) + L"]");
      sound::AddHiddenApp(settings.hidden, readString(key.c_str()));
    }
  };
  readHidden(L"sound.hiddenApplications");
  if (settings.hidden.empty())
    readHidden(L"hiddenApplications");
  ReadAlternativeIcons(settings, readInt);
  return settings;
}
} // namespace native

# Windows Border Switch

**English** · [Русский](README.ru.md) · [DSN Windhawk Mods](../README.md)

Windows Border Switch controls system-drawn borders in Windows 11.

## Features

- Separate settings for active windows, inactive windows, classic Win32 dialogs, the taskbar, Start, Search, the notification center, Quick Settings, and the Widgets board
- One Start setting covers the main menu and its Phone Link panel
- The taskbar setting also covers the virtual desktops bar in Task View
- The notifications setting covers the notification center, calendar, and desktop toasts
- The Quick Settings setting also covers the media controls
- The active-window setting can remove the title bar accent while keeping the frame
- Interface in English, Russian, Spanish, German, Brazilian Portuguese, and Simplified Chinese

## Platforms

Windows 11 x64 or ARM64 with Windhawk. Shell panel support depends on the XAML templates used by the installed Windows build and Windows Web Experience Pack. Applications with custom-drawn frames and title bars control their own appearance.

## Operations

In Windhawk, select **Create a New Mod**, paste the complete [source](Mod/windows-border-switch.wh.cpp) into the editor and select **Compile Mod**. By default, the mod removes the active title bar accent and hides inactive window, dialog, taskbar, and shell panel borders. These choices can be changed under **Settings**. The title bar option applies when Windows uses the accent color for title bars.

Returning a window setting to **Default** restores the system DWM color. Applications that set a custom DWM color can reapply it when the window reopens.

## References

- [Invisible Window Borders](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/invisible-borders.wh.cpp)
- [Windows 11 Accent Window Border](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/win11-accent-border.wh.cpp)
- [Windows 11 Start Menu Styler and its styling guide](https://github.com/ramensoftware/windows-11-start-menu-styling-guide)
- [Windows 11 Taskbar Styler and its styling guide](https://github.com/ramensoftware/windows-11-taskbar-styling-guide)
- [Windows 11 Notification Center Styler and its styling guide](https://github.com/ramensoftware/windows-11-notification-center-styling-guide)

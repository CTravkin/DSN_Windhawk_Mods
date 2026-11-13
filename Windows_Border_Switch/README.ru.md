# Windows Border Switch

[English](README.md) · **Русский** · [DSN Windhawk Mods](../README.ru.md)

Windows Border Switch управляет системными рамками в Windows 11.

## Функции

- Отдельные настройки для активного окна, неактивных окон, классических диалогов Win32, меню «Пуск» и панели задач
- Одна настройка «Пуска» действует на основное меню и панель «Телефон»
- Настройка активного окна позволяет убрать акцент заголовка, сохранив рамку
- Интерфейс на английском, русском, испанском, немецком, бразильском португальском и упрощенном китайском

## Платформы

Windows 11 x64 или ARM64 с Windhawk. Поддержка «Пуска» и панели задач зависит от XAML-шаблонов установленной сборки Windows. Приложения, которые рисуют рамки и заголовки самостоятельно, управляют их видом сами.

## Эксплуатация

В Windhawk выберите **Создать новый мод**, вставьте в редактор полный [исходный код](Mod/windows-border-switch.wh.cpp) и скомпилируйте мод. Рамки можно скрывать через вкладку **Настройки**. Начальное значение всех настроек — **По умолчанию**. Отключение акцента заголовка действует, если в Windows включен акцентный цвет заголовков.

Режим **По умолчанию** восстанавливает системный цвет DWM. Приложения с собственным цветом DWM могут восстановить его при повторном открытии окна.

## Ссылки

- [Invisible Window Borders](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/invisible-borders.wh.cpp)
- [Windows 11 Accent Window Border](https://github.com/ramensoftware/windhawk-mods/blob/main/mods/win11-accent-border.wh.cpp)
- [Windows 11 Start Menu Styler и руководство по настройке](https://github.com/ramensoftware/windows-11-start-menu-styling-guide)
- [Windows 11 Taskbar Styler и руководство по настройке](https://github.com/ramensoftware/windows-11-taskbar-styling-guide)

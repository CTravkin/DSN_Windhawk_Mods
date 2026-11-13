#include "../Mod/windows-border-switch.wh.cpp"

static_assert(WindowEffectFor({}, true, false) == WindowEffect::Default);
static_assert(WindowEffectFor({WindowEffect::HideBorder}, true, false) ==
              WindowEffect::HideBorder);
static_assert(WindowEffectFor({WindowEffect::HideBorder}, false, false) ==
              WindowEffect::Default);
static_assert(WindowEffectFor({WindowEffect::Default, true}, false, false) ==
              WindowEffect::HideBorder);
static_assert(WindowEffectFor({WindowEffect::Default, true}, true, false) ==
              WindowEffect::Default);
static_assert(WindowEffectFor({WindowEffect::RemoveCaptionAccent}, true, false) ==
              WindowEffect::RemoveCaptionAccent);
static_assert(WindowEffectFor({WindowEffect::RemoveCaptionAccent}, false, false) ==
              WindowEffect::Default);
static_assert(WindowEffectFor({WindowEffect::HideBorder, true}, true, true) ==
              WindowEffect::Default);
static_assert(WindowEffectFor({WindowEffect::RemoveCaptionAccent, false, true}, true, true) ==
              WindowEffect::HideBorder);

constexpr WindowSettings kHideInactiveWithCaption{WindowEffect::RemoveCaptionAccent, true, true};
static_assert(WindowFrameFor(kHideInactiveWithCaption, true, false, true, true) ==
              WindowFrame{false, CaptionEffect::Transparent});
static_assert(WindowFrameFor(kHideInactiveWithCaption, false, false, true, true) ==
              WindowFrame{true, CaptionEffect::Transparent});
static_assert(WindowFrameFor(kHideInactiveWithCaption, false, false, false, true) ==
              WindowFrame{true, CaptionEffect::Default});
static_assert(WindowFrameFor(kHideInactiveWithCaption, true, false, false, true) ==
              WindowFrame{false, CaptionEffect::Neutral});
static_assert(WindowFrameFor(kHideInactiveWithCaption, true, true, false, true) ==
              WindowFrame{true, CaptionEffect::Default});
static_assert(WindowFrameFor(kHideInactiveWithCaption, true, false, true, false) ==
              WindowFrame{false, CaptionEffect::Default});

static_assert(ShellBorderFor(ShellHost::Start, L"Windows.UI.Xaml.Controls.Border", L"AcrylicBorder") ==
              ShellBorder::Start);
static_assert(ShellBorderFor(ShellHost::Explorer, L"Windows.UI.Xaml.Shapes.Rectangle", L"BackgroundStroke") ==
              ShellBorder::Taskbar);
static_assert(ShellBorderFor(ShellHost::Explorer, L"Windows.UI.Xaml.Controls.Border", L"AcrylicBorder") ==
              ShellBorder::None);
static_assert(ShellBorderFor(ShellHost::Start, L"Windows.UI.Xaml.Shapes.Rectangle", L"BackgroundStroke") ==
              ShellBorder::None);
static_assert(ShellBorderFor(ShellHost::Explorer, L"Windows.UI.Xaml.Controls.Border", L"BackgroundStroke") ==
              ShellBorder::None);
static_assert(IsStartPane(L"MainMenu"));
static_assert(IsStartPane(L"CompanionRoot"));
static_assert(!IsStartPane(L"SearchRoot"));
static_assert(ShellBorderFor(ShellHost::Search, L"Windows.UI.Xaml.Controls.Border",
                            L"AppBorder") == ShellBorder::Search);
static_assert(ShellBorderFor(ShellHost::Search, L"Windows.UI.Xaml.Controls.Border",
                            L"AccentAppBorder") == ShellBorder::Search);
static_assert(ShellBorderFor(ShellHost::Flyouts, L"Windows.UI.Xaml.Controls.Grid",
                            L"CalendarCenterGrid") == ShellBorder::Notifications);
static_assert(ShellBorderFor(ShellHost::Flyouts, L"Windows.UI.Xaml.Controls.Grid",
                            L"NotificationCenterGrid") == ShellBorder::Notifications);
static_assert(ShellBorderFor(ShellHost::Flyouts, L"Windows.UI.Xaml.Controls.Grid",
                            L"ControlCenterRegion") == ShellBorder::QuickSettings);
static_assert(ShellBorderFor(ShellHost::Flyouts, L"Windows.UI.Xaml.Controls.Grid",
                            L"MediaTransportControlsRegion") == ShellBorder::QuickSettings);
static_assert(ShellBorderFor(ShellHost::Flyouts, L"Windows.UI.Xaml.Controls.Border",
                            L"ToastBackgroundBorder") == ShellBorder::Notifications);
static_assert(ShellBorderFor(ShellHost::Flyouts, L"Windows.UI.Xaml.Controls.Border",
                            L"ToastBackgroundBorder2") == ShellBorder::Notifications);
static_assert(ShellBorderFor(ShellHost::Flyouts, L"Windows.UI.Xaml.Controls.Border",
                            L"ItemOpaquePlating") == ShellBorder::None);
static_assert(ShellBorderFor(ShellHost::Search, L"Windows.UI.Xaml.Controls.Grid",
                            L"CalendarCenterGrid") == ShellBorder::None);
static_assert(ShellBorderFor(ShellHost::Widgets, L"Microsoft.UI.Xaml.Controls.Border",
                            L"NavigationViewBorder") == ShellBorder::Widgets);
static_assert(ShellBorderFor(ShellHost::Widgets, L"Microsoft.UI.Xaml.Controls.Border",
                            L"WidgetFrameBorder") == ShellBorder::None);
static_assert(ShellBorderFor(ShellHost::Widgets, L"Microsoft.UI.Xaml.Controls.Grid",
                            L"WidgetBoardHost") == ShellBorder::Widgets);
static_assert(ShellBorderFor(ShellHost::Widgets, L"Microsoft.UI.Xaml.Controls.Grid",
                            L"WidgetBoardContent") == ShellBorder::Widgets);
static_assert(ShellBorderFor(ShellHost::Widgets, L"WidgetBoard.WidgetHostContentControl",
                            L"WidgetBoardContent") == ShellBorder::Widgets);
static_assert(ShellBorderFor(ShellHost::Explorer, L"Windows.UI.Xaml.Controls.Border",
                            L"VirtualDesktopBarBackground") == ShellBorder::TaskView);
static_assert(ShellBorderFor(ShellHost::Explorer, L"Windows.UI.Xaml.Controls.Border",
                            L"VirtualDesktopSwitcherBackground") == ShellBorder::TaskView);
static_assert(ShellBorderFor(ShellHost::Explorer, L"Windows.UI.Xaml.Controls.Border",
                            L"BorderHighlight") == ShellBorder::None);

int main() {
    unsigned lastProbe = 0;
    auto probe = [&](unsigned index) {
        lastProbe = index;
        return index == 87 ? 0 : 1;
    };
    if (FindDiagnosticEndpoint(probe, [] { return false; }, 1, 2) != 0 ||
        lastProbe != 87) {
        return 1;
    }
    lastProbe = 0;
    if (FindDiagnosticEndpoint(probe, [&] { return lastProbe == 65; }, 1, 2) != 2 ||
        lastProbe != 65) {
        return 2;
    }
    unsigned probes = 0;
    if (FindDiagnosticEndpoint([&](unsigned) { ++probes; return 1; },
                               [] { return false; }, 1, 2) != 1 || probes != 10000) {
        return 3;
    }
}

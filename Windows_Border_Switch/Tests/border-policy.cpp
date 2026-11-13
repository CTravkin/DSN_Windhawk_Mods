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

static_assert(ShellBorderFor(true, L"Windows.UI.Xaml.Controls.Border", L"AcrylicBorder") ==
              ShellBorder::Start);
static_assert(ShellBorderFor(false, L"Windows.UI.Xaml.Shapes.Rectangle", L"BackgroundStroke") ==
              ShellBorder::Taskbar);
static_assert(ShellBorderFor(false, L"Windows.UI.Xaml.Controls.Border", L"AcrylicBorder") ==
              ShellBorder::None);
static_assert(ShellBorderFor(true, L"Windows.UI.Xaml.Shapes.Rectangle", L"BackgroundStroke") ==
              ShellBorder::None);
static_assert(ShellBorderFor(false, L"Windows.UI.Xaml.Controls.Border", L"BackgroundStroke") ==
              ShellBorder::None);
static_assert(IsStartPane(L"MainMenu"));
static_assert(IsStartPane(L"CompanionRoot"));
static_assert(!IsStartPane(L"SearchRoot"));

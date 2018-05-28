#ifndef FRAMEWORK_STYLE_HPP__
#define FRAMEWORK_STYLE_HPP__

#include <string>

namespace framework {

enum class Font {
  None = 0,
  Default  = 1,
  Bold = 1 << 1, // 1
  Faint = 1 << 2, // 2
  Roman = 1 << 3, // not Italic
  Italic = 1 << 4, // 3
  NotUnderline = 1 << 5,
  Underline = 1 << 6, // 4
  Visible = 1 << 7, 
  Hidden = 1 << 8, // 8
  Uncrossed = 1 << 9,
  Crossed = 1 << 10, // 9
  Inherit = 1 << 11
};

enum class FontColour {
  None = 0,
  Black = 1,
  Red = 2, 
  Green = 3, 
  Yellow = 4,
  Blue = 5, 
  Magenta = 6,
  Cyan = 7,
  White = 8, 
  Default = 9, 
  Transparent = 10, 
  Inherit = 11
};

enum class Colour {
  None = 0,
  Black = 1,
  Red = 2, 
  Green = 3, 
  Yellow = 4,
  Blue = 5, 
  Magenta = 6,
  Cyan = 7,
  White = 8, 
  Default = 9, 
  Transparent = 10, 
  Inherit = 11
};

template <class X>
constexpr auto composeMod(X x) { 
  return x;
}

template <class X, class... Xs>
constexpr auto composeMod(X x, Xs... xs) {
  auto const r = composeMod(xs...);
  auto const delim = (r == "") || (x == "") ? "" : ";";
  return x + delim + r;
}

template <class X, class... Xs>
constexpr auto computeMod(X x, Xs... xs) {
  auto const r = composeMod(x, xs...);
  return (r == "") ? "" : "\x1b[" + r + "m";
}

FontColour isStyle(FontColour x) { return x; }
Colour isStyle(Colour x) { return x; }
Font isStyle(Font x) { return x; }

FontColour toFontColour(FontColour c) { return c; }
Colour toColour(Colour c) { return c; }
Font toFont(Font f) { return f; }

FontColour toFontColour(...) { return FontColour::None; }
Colour toColour(...) { return Colour::None; }
Font toFont(...) { return Font::None; }

template <class...Xs>
constexpr Colour getColour(Xs... xs) {
  return (Colour)std::max<int>({0, (int)toColour(xs)...});
}

template <class...Xs>
constexpr FontColour getFontColour(Xs... xs) {
  return (FontColour)std::max<int>({0, (int)toFontColour(xs)...});
}

constexpr Font getFontStyle(Font f = Font::None) {
  return f;
}

template <class X, class... Xs>
constexpr Font getFontStyle(X x, Xs... xs) {
  return Font( ((int)getFontStyle(toFont(x))) | ((int)getFontStyle(toFont(xs)...)) );
}


bool has(Font x, Font y) {
  return (((int)x) & ((int)y));
}


struct Style {
  Colour bg;	
  FontColour fg;
  Font font;

  template <class... Styles>
  constexpr Style(Styles... styles) :
    bg{getColour(isStyle(styles)...)},
    fg{getFontColour(isStyle(styles)...)},
    font{getFontStyle(isStyle(styles)...)}
  {}
	
  constexpr static Style None() { return {}; }
  constexpr static Style Default() { return { Font::Default }; }

  std::string defaultMod() const {
    return has(font, Font::Default) ? "0" : "";
  }


  std::string boldMod() const {
    return has(font, Font::Bold) ? "1" : "";
  }

  std::string underlineMod() const {
    return has(font, Font::Underline) ? "4" : "";
  }

  std::string faintMod() const {
    return has(font, Font::Faint) ? "2" : "";
  }

  std::string italicMod() const {
    return has(font, Font::Italic) ? "3" : "";
  }

  std::string hiddenMod() const {
    return has(font, Font::Hidden) ? "8" : "";
  }

  std::string crossedMod() const {
    return has(font, Font::Crossed) ? "9" : "";
  }

  std::string bgMod() const {
    return ((int)bg<11) ? std::to_string(40 + (int)bg -1) : "";
  }

  std::string fgMod() const {
    return ((int)fg<11) ? std::to_string(30 + (int)fg -1) : "";
  }

  std::string toString() const {
    return computeMod(
      defaultMod(),
      boldMod(),
      underlineMod(),
      faintMod(),
      italicMod(),
      hiddenMod(),
      crossedMod(),
      bgMod(),
      fgMod()
    );
  }
};


Style diff(Style const& a, Style const& b = Style::None()) {
  bool keepBG = (a.bg == b.bg);
  bool keepFG = (a.fg == b.fg);
  bool keepFont = (a.font == b.font);

  int l = (int)a.font;
  int r = (int)b.font;
  bool reset = (l & ~r) ? 1 : 0;

  return Style {
    (keepBG && !reset) ? Colour::Inherit : b.bg,
    (keepFG && !reset) ? FontColour::Inherit : b.fg,
    (keepFont && !reset) ? Font::Inherit : (Font)((r&((l&r)^r))|(int)reset)
  };
}

}

#endif // FRAMEWORK_STYLE_HPP__
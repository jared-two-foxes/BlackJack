#ifndef FRAMEWORK_PIXEL_HPP__
#define FRAMEWORK_PIXEL_HPP__

#include <string>
#include <framework/terminal/style.hpp>

namespace framework {

struct Pixel {
  char c = '\0';
  Style style;

  std::string toString() const {
    return style.toString() + c;
  }	
};

}

#endif // FRAMEWORK_PIXEL_HPP__
#ifndef FRAMEWORK_TEXT_HPP__
#define FRAMEWORK_TEXT_HPP__

#include <framework/terminal/image.hpp>
#include <framework/terminal/util.hpp>
#include <framework/terminal/reflow.hpp>
#include <framework/terminal/style.hpp>

#include <algorithm>
#include <string>

namespace framework {

struct Text {
  Style const style;
  std::string const content;

  template <class T>
  Text(Style const& style, T const& content) :
  	style{style},
  	content{toString(content)}
  {}

  template <class T>
  Text(T const& content) :
    style{Style{}},
    content{toString(content)}
  {}

  Image render(unsigned const maxWidth) const {
  	auto const lines = split(reflow(maxWidth, content), "\n");
  	auto const height = lines.size();
  	auto const width = std::max_element(
  	  lines.begin(), 
  	  lines.end(),
  	  [](auto const& a, auto const& b) {
  	  	return a.size() < b.size();
  	  })->size();

  	auto image = Image::create(width, height, Pixel{'\0', style});
  	unsigned y = 0;
  	for (auto const& line : lines) {
  	  for (int x = 0; x < line.size(); ++x) {
  	  	image(x,y).c = line[x];
  	  }
  	  ++y;
  	}

  	return image;
  }
};

}

#endif // FRAMEWORK_TEXT_HPP__
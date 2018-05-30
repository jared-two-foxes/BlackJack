#ifndef FRAMEWORK_TERMINAL_UTIL_HPP__
#define FRAMEWORK_TERMINAL_UTIL_HPP__

#include <string>

#define ESC "\x1b"
#define CSI "\x1b["

namespace framework {

inline auto toString(std::string const& x) -> std::string {
	return x;
}

template <class T>
auto toString(T const& x) ->decltype(std::to_string(x)) {
	return std::to_string(x);
}

inline std::vector<std::string > split(std::string const& str, const std::string& delimiter = "\n") {
	std::vector<std::string > tokens;

	auto start = 0U;
	auto end = str.find(delimiter);
	while (end != std::string::npos) {
		tokens.push_back(str.substr(start, end - start));
		start = end + delimiter.size();
		end = str.find(delimiter, start);
	}

	if (start != str.size()) {
		tokens.push_back(str.substr(start, str.size() - start));
	}

	return tokens;
}

template <class T, class F>
auto map(T const& data, F const& f) {
  std::vector<decltype(f(data[0]))> result(data.size());
  std::transform(
  	data.begin(), data.end(), 
  	result.begin(), 
  	f);
  return result;
}

inline std::string repeat(unsigned n, std::string const& s) {
	std::string result = "";
	for (unsigned i = 0; i < n; ++i) {
		result += s;
	}
	return result;
}

inline std::string clearBeforeCursor() {
	return CSI"0K";
}

inline std::string clearAfterCursor() {
	return CSI"1K";
}

inline std::string clearLine() {
	return CSI"2K\r";
}

inline std::string moveUp(unsigned n = 1 ) {
	return CSI + std::to_string(n) + "A\r";
}	

inline std::string clearLines(unsigned n = 1) {
	return CSI"0m" + clearBeforeCursor() + ((n) ? repeat(n, clearLine() + moveUp()) + clearLine() : std::string(""));
}

}

#endif // FRAMEWORK_TERMINAL_UTIL_HPP__

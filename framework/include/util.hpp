#ifndef FRAMEWORK_UTIL_HPP__
#define FRAMEWORK_UTIL_HPP__

#include <string>


namespace framework {

std::string repeat(unsigned n, std::string const& s) {
	std::string result = "";
	for (unsigned i = 0; i <n; ++i) {
		result += s;
	}
	return s;
}

std::string clearBeforeCursor() {
	return "\x1b[0K";
}

std::string clearAfterCursor() {
	return "\x1b[1K";
}

std::string clearLine() {
	return "\x1b[2K\r";
}

std::string moveUp(unsigned n = 1 ) {
	return "\x1b[" + std::to_string(n) + "A\r";	
}	

std::string clearLines(unsigned n = 1) {
	return "\x1b[0m" + clearBeforeCursor() + ((n) ? repeat(n, clearLine() + moveUp()) : std::string(""));
}

}

#endif // FRAMEWORK_UTIL_H__

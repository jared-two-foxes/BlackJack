#ifndef FRAMEWORK_TERMINAL_HPP__
#define FRAMEWORK_TERMINAL_HPP__

#include <framework/util.hpp>

#include <algorithm>
#include <iostream>
#include <string>

namespace framework {

struct VirtualTerminal {
	std::string buffer;

	void reset() {
		if (buffer == "") return;
		unsigned const n = std::count(buffer.begin(), buffer.end(), '\n') + 1;
		std::cout << clearLines(n) << std::endl;
		buffer = "";
	}

	void append(std::string s) {
		buffer += s;
	}

	void flip(std::string s) {
		reset();
		std::cout << s << "\x1b[0m";
		std::flush(std::cout);
		buffer = s;
	}
};

}


#endif // FRAMEWORK_TERMINAL_HPP__
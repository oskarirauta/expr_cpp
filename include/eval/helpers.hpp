#pragma once

#include <string>

bool is_space(const char& ch);
bool is_digit(const char& ch);
bool is_alpha(const char& ch);
bool is_alnum(const char& ch);

bool is_space(const std::string& s);
bool is_digit(const std::string& s);
bool is_alpha(const std::string& s);
bool is_alnum(const std::string& s);

const std::string erase_prefix(std::string& s, size_t n);
const unsigned char erase_front(std::string& s);

const std::string toupper(const std::string& s);
const std::string tolower(const std::string& s);
const std::string double2str(const double& d);

template<typename... Ts>
std::string fmt(const std::string& fmt, Ts... vs) {

	size_t size = snprintf(nullptr, 0, fmt.c_str(), vs...);
	std::string buf;
	buf.reserve(size + 1);
	buf.resize(size);
	snprintf(&buf[0], size + 1, fmt.c_str(), vs...);
	return buf;
}

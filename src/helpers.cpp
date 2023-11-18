#include <sstream>
#include "eval/helpers.hpp"

bool is_space(const char& ch) {
	return ch == ' ' || ch == '\t';
}

bool is_digit(const char& ch) {
	return ch >= '0' && ch <= '9';
}

bool is_alpha(const char& ch) {
	return ( ch >= 'A' && ch <= 'Z' ) || ( ch >= 'a' && ch <= 'z' ) || ch == '_';
}

bool is_alnum(const char& ch) {
	return is_alpha(ch) || is_digit(ch);
}

bool is_space(const std::string& s) {
	return !s.empty() && is_space(s.front());
}

bool is_digit(const std::string& s) {
	return !s.empty() && is_digit(s.front());
}

bool is_alpha(const std::string& s) {
	return !s.empty() && is_alpha(s.front());
}

bool is_alnum(const std::string& s) {
	return !s.empty() && is_alnum(s.front());
}

const std::string erase_prefix(std::string& s, size_t n) {

	int l;
	std::string r;

	if ( l = n > s.size() ? s.size() : n; l == 0 )
		return "";

	if ( r = s.substr(0, l); !r.empty())
		s.erase(0, r.size());

	return r;
}

const unsigned char erase_front(std::string& s) {

	unsigned char r = 0;

	if ( !s.empty()) {
		r = s.front();
		s.erase(0, 1);
	}

	return r;
}

const std::string toupper(const std::string& s) {

	std::string _s(s);

	for ( char &c : _s )
		if ( std::islower(c))
			c &= ~32;

	return _s;
}

const std::string tolower(const std::string& s) {

	std::string _s(s);

	for ( char &c : _s )
		if ( std::isupper(c))
			c ^= 32;

	return _s;
}

const std::string double2str(const double& d) {
        std::stringstream ss;
        ss << d;
        return ss.str();
}

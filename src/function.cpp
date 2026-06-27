#include <ctime>
#include <iomanip>
#include <cstring>
#include <cmath>
#include <sstream>
#include <algorithm>

#include "common.hpp"
#include "logger.hpp"
#include "expr/function.hpp"

static double arg_to_double(const expr::VARIABLE& var) {

	double d = 0;
	if ( var == expr::V_NUMBER )
		d = var;
	else if ( var == expr::V_STRING ) {

		std::string s = var;

		try {
			d = std::stod(s);
		} catch ( const std::exception &e ) {
			d = 0;
			logger::warning["function"] << "failed to convert '" << s << "' to number, setting result to 0" << std::endl;
		}

	} else d = 0;

	return d;
}

expr::VARIABLE expr::functions::time_unixtime(const expr::FUNCTION_ARGS& args) {

	std::chrono::seconds s = std::chrono::duration_cast<std::chrono::seconds>
		(std::chrono::system_clock::now().time_since_epoch());

	double d = (double)s.count();
	return d;
}

expr::VARIABLE expr::functions::time_hour(const expr::FUNCTION_ARGS& args) {

	return (double)common::to_tm().tm_hour;
}

expr::VARIABLE expr::functions::time_min(const expr::FUNCTION_ARGS& args) {

	return (double)common::to_tm().tm_min;
}

expr::VARIABLE expr::functions::time_sec(const expr::FUNCTION_ARGS& args) {

	return (double)common::to_tm().tm_sec;
}

expr::VARIABLE expr::functions::date_day(const expr::FUNCTION_ARGS& args) {

	return (double)common::to_tm().tm_mday;
}

expr::VARIABLE expr::functions::date_month(const expr::FUNCTION_ARGS& args) {

	return (double)(common::to_tm().tm_mon + 1);
}

expr::VARIABLE expr::functions::date_year(const expr::FUNCTION_ARGS& args) {

	return (double)(common::to_tm().tm_year + 1900);
}

expr::VARIABLE expr::functions::date_weekday(const expr::FUNCTION_ARGS& args) {

	return (double)common::to_tm().tm_wday;
}

expr::VARIABLE expr::functions::date_day_name(const expr::FUNCTION_ARGS& args) {

	switch ( common::to_tm().tm_wday ) {
		case 0: return "Sun";
		case 1: return "Mon";
		case 2: return "Tue";
		case 3: return "Wed";
		case 4: return "Thu";
		case 5: return "Fri";
		case 6: return "Sat";
		default: return "Unk";
	}
}

expr::VARIABLE expr::functions::strftime(const expr::FUNCTION_ARGS& args) {

	if ( args.empty()) {
		logger::warning["function"] << "strftime needs 1 argument for format and optionally a timestamp as second argument, 0 arguments given" << std::endl;
		return "";
	} else if ( !args[0].string_convertible().empty()) {
		logger::warning["function"] << "strftime argument conversion error, reason: " << args[0].string_convertible() << std::endl;
		return "";
	} else if ( args[0].to_string().empty()) {
		return "";
	} else if ( args.size() > 1 && !args[1].number_convertible().empty()) {
		logger::warning["function"] << "second argument of strftime is not convertible to number, reason: " << args[1].string_convertible() << std::endl;
		return "";
	}

	std::chrono::seconds ds =
		args.size() > 1 && args[1].number_convertible().empty() ? common::mk_duration(args[1].to_double()) : common::mk_duration();

	return common::put_time(args[0].to_string(), ds);
}

expr::VARIABLE expr::functions::to_string(const expr::FUNCTION_ARGS& args) {

	if ( args.empty())
		return "";
	else if ( args[0] == expr::V_NUMBER ) {
		double d = (double)args[0].to_double();
		return common::to_string(d);
	} else if ( args[0] == expr::V_STRING ) {
		return args[0].to_string().empty() ? "" : (std::string)args[0];
	} else return "";
}

expr::VARIABLE expr::functions::to_double(const expr::FUNCTION_ARGS& args) {

	if ( args.empty())
		return (double)0;
	else return arg_to_double(args[0]);
}

expr::VARIABLE expr::functions::to_int(const expr::FUNCTION_ARGS& args) {

	int i = 0;
	if ( args.empty())
		i = 0;
	else i = (int)arg_to_double(args[0]);

	return i;
}

expr::VARIABLE expr::functions::to_bool(const expr::FUNCTION_ARGS& args) {

	bool b = false;

	if ( args.empty())
		b = false;
	else b = arg_to_double(args[0]) == 0 ? false : true;

	return b;
}

expr::VARIABLE expr::functions::is_odd(const expr::FUNCTION_ARGS& args) {

	int i = args.empty() ? 0 : (int)arg_to_double(args[0]);
	bool b = i % 2 == 0 ? false : true;
	return b;
}

expr::VARIABLE expr::functions::is_even(const expr::FUNCTION_ARGS& args) {

	int i = args.empty() ? 0 : (int)arg_to_double(args[0]);
	bool b = i % 2 == 0 ? true : false;
	return b;
}

expr::VARIABLE expr::functions::sqrt(const expr::FUNCTION_ARGS& args) {

	double d = args.empty() ? (double)0 : arg_to_double(args[0]);
	d = std::sqrt(d);
	return d;
}
expr::VARIABLE expr::functions::exp(const expr::FUNCTION_ARGS& args) {

	double d = args.empty() ? (double)0 : arg_to_double(args[0]);
	d = std::exp(d);
	return d;
}

expr::VARIABLE expr::functions::ln(const expr::FUNCTION_ARGS& args) {

	double d = args.empty() ? (double)0 : arg_to_double(args[0]);
	d = std::log(d);
	return d;
}

expr::VARIABLE expr::functions::log(const expr::FUNCTION_ARGS& args) {

	double d = args.empty() ? (double)0 : arg_to_double(args[0]);
	d = std::log10(d);
	return d;
}

expr::VARIABLE expr::functions::sin(const expr::FUNCTION_ARGS& args) {

	double d = args.empty() ? (double)0 : arg_to_double(args[0]);
	d = std::sin(d);
	return d;
}

expr::VARIABLE expr::functions::cos(const expr::FUNCTION_ARGS& args) {

	double d = args.empty() ? (double)0 : arg_to_double(args[0]);
	d = std::cos(d);
	return d;
}

expr::VARIABLE expr::functions::tan(const expr::FUNCTION_ARGS& args) {

	double d = args.empty() ? (double)0 : arg_to_double(args[0]);
	d = std::tan(d);
	return d;
}

expr::VARIABLE expr::functions::min(const expr::FUNCTION_ARGS& args) {

	double d1 = args.size() < 1 ? (double)0 : arg_to_double(args[0]);
	double d2 = args.size() < 2 ? (double)0 : arg_to_double(args[1]);
	d1 = std::min(d1, d2);
	return d1;
}

expr::VARIABLE expr::functions::max(const expr::FUNCTION_ARGS& args) {

	double d1 = args.size() < 1 ? (double)0 : arg_to_double(args[0]);
	double d2 = args.size() < 2 ? (double)0 : arg_to_double(args[1]);
	d1 = std::max(d1, d2);
	return d1;
}

expr::VARIABLE expr::functions::floor(const expr::FUNCTION_ARGS& args) {

	double d = args.empty() ? (double)0 : arg_to_double(args[0]);
	d = std::floor(d);
	return d;
}

expr::VARIABLE expr::functions::ceil(const expr::FUNCTION_ARGS& args) {

	double d = args.empty() ? (double)0 : arg_to_double(args[0]);
	d = std::ceil(d);
	return d;
}

expr::VARIABLE expr::functions::round(const expr::FUNCTION_ARGS& args) {

	double d = args.empty() ? (double)0 : arg_to_double(args[0]);
	return (double)std::lround(d);
}

expr::VARIABLE expr::functions::strlen(const expr::FUNCTION_ARGS& args) {

	if ( args.empty())
		return (double)0;
	else return (double)args[0].to_string().size();
}

expr::VARIABLE expr::functions::to_upper(const expr::FUNCTION_ARGS& args) {

	if ( args.empty())
		return "";
	else if ( args[0].to_string().empty())
		return "";
	else return common::to_upper(args[0].to_string());
}

expr::VARIABLE expr::functions::to_lower(const expr::FUNCTION_ARGS& args) {

	if ( args.empty())
		return "";
	else if ( args[0].to_string().empty())
		return "";
	else return common::to_lower(args[0].to_string());
}

expr::VARIABLE expr::functions::substr(const expr::FUNCTION_ARGS& args) {

	if ( args.empty() || args.size() < 2 ) {

		logger::error["function"] << "wrong number of arguments for substr function" << std::endl;
		return args.empty() ? "" : args[0].to_string();
	}

	if ( !args[0].string_convertible().empty()) {

		logger::error["function"] << "substr functions first argument is not a string" << std::endl;
		return "";
	} else if ( args[0].to_string().empty()) {

		logger::vverbose["function"] << "substr cannot be done for empty string" << std::endl;
		return "";
	} else if ( args.size() > 1 && !args[1].number_convertible().empty()) {

		logger::warning["function"] << "substr function's second argument must be a number" << std::endl;
		return args[0].to_string();
	} else if ( args.size() > 2 && !args[2].number_convertible().empty()) {

		logger::warning["function"] << "substr function's second and third arguments must be numbers" << std::endl;
		return args[0].to_string();
	} else if ( args.size() == 2 && args[1].number_convertible().empty()) {

		std::string s = args[0].to_string();
		size_t pos = (size_t)args[1].to_int();

		if ( pos == 0 )
			return s;
		else if ( pos + 1 > s.size()) {

			logger::vverbose["function"] << "substr pos(" << pos << ") outside of bounds(" << s.size() << ")" << std::endl;
			return "";
		}

		std::string r = s;

		try {
			r = s.substr(pos);
		} catch ( std::out_of_range& e ) {

			r = s;
			logger::vverbose["function"] << "substr('" << s << "', " << pos << ") failure, reason: " << e.what() << std::endl;
		}

		return r;

	} else if ( args.size() > 2 && args[1].number_convertible().empty() && args[2].number_convertible().empty()) {

		std::string s = args[0].to_string();
		size_t pos = (size_t)args[1].to_int();
		size_t len = (size_t)args[2].to_int();

		if ( pos == 0 && len >= s.size())
			return s;
		else if ( pos > s.size()) {

			logger::vverbose["function"] << "substr pos(" << pos << ") outside of bounds(" << s.size() << ")" << std::endl;
			return "";
		} else if ( s.size() - pos < len ) {

			logger::vverbose["function"] << "substr pos(" << pos << ") and size(" << len << ") outside of bounds(" <<
				s.size() << ")" << std::endl;

			if ( pos == 0 )
				return s;

			if ( pos + 1 >= s.size())
				return "";

			try {
				s.erase(0, pos);
			} catch ( std::out_of_range& e ) {

				logger::vverbose["function"] << "substr('" << s << ", " << pos << ", " << len << ") failure, reason: " << e.what() << std::endl;
			}

			return s;
		}

		std::string r;

		try {
			r = s.substr(pos, len);
		} catch ( std::out_of_range& e ) {

			r = s;
			logger::vverbose["function"] << "substr('" << s << "', " << pos << ", " << len << ")failure, reason: " << e.what() << std::endl;
		}

		return r;
	}

	std::string s = args.size() > 0 && args[0].string_convertible().empty() ? args[0].to_string() : "";
	size_t pos = args.size() > 1 && args[1].number_convertible().empty() ? (size_t)args[1].to_int() : (size_t)0;
	size_t len = args.size() > 2 && args[2].number_convertible().empty() ? (size_t)args[2].to_int() : (size_t)0;

	logger::error["function"] << "substr('" << s << "', " << pos << ", " << len << ") failure, reason: unknown error" << std::endl;
	return s;
}

// ── String helpers ────────────────────────────────────────────────────────────

static std::string arg_to_string(const expr::VARIABLE& var) {
	return var.to_string();
}

expr::VARIABLE expr::functions::trim(const expr::FUNCTION_ARGS& args) {

	if ( args.empty()) return "";
	return common::trim_ws(args[0].to_string());
}

expr::VARIABLE expr::functions::ltrim(const expr::FUNCTION_ARGS& args) {

	if ( args.empty()) return "";
	std::string s = args[0].to_string();
	s.erase(s.begin(), std::find_if(s.begin(), s.end(), [](unsigned char c){ return !std::isspace(c); }));
	return s;
}

expr::VARIABLE expr::functions::rtrim(const expr::FUNCTION_ARGS& args) {

	if ( args.empty()) return "";
	std::string s = args[0].to_string();
	s.erase(std::find_if(s.rbegin(), s.rend(), [](unsigned char c){ return !std::isspace(c); }).base(), s.end());
	return s;
}

expr::VARIABLE expr::functions::pad_left(const expr::FUNCTION_ARGS& args) {

	if ( args.size() < 2 ) {
		logger::warning["function"] << "pad_left needs at least 2 arguments: string, width [, fill_char]" << std::endl;
		return args.empty() ? "" : args[0].to_string();
	}

	std::string s    = args[0].to_string();
	int         w    = args[1].to_int();
	std::string fs   = args.size() > 2 ? arg_to_string(args[2]) : std::string(" ");
	char        fill = fs.empty() ? ' ' : fs.front();

	if ( w <= 0 || (int)s.size() >= w ) return s;
	return std::string(w - s.size(), fill) + s;
}

expr::VARIABLE expr::functions::pad_right(const expr::FUNCTION_ARGS& args) {

	if ( args.size() < 2 ) {
		logger::warning["function"] << "pad_right needs at least 2 arguments: string, width [, fill_char]" << std::endl;
		return args.empty() ? "" : args[0].to_string();
	}

	std::string s    = args[0].to_string();
	int         w    = args[1].to_int();
	std::string fs   = args.size() > 2 ? arg_to_string(args[2]) : std::string(" ");
	char        fill = fs.empty() ? ' ' : fs.front();

	if ( w <= 0 || (int)s.size() >= w ) return s;
	return s + std::string(w - s.size(), fill);
}

expr::VARIABLE expr::functions::str_repeat(const expr::FUNCTION_ARGS& args) {

	if ( args.size() < 2 ) {
		logger::warning["function"] << "str_repeat needs 2 arguments: string, count" << std::endl;
		return "";
	}

	std::string s = args[0].to_string();
	int         n = args[1].to_int();

	if ( n <= 0 || s.empty()) return "";

	std::string result;
	result.reserve(s.size() * n);
	for ( int i = 0; i < n; i++ ) result += s;
	return result;
}

expr::VARIABLE expr::functions::str_contains(const expr::FUNCTION_ARGS& args) {

	if ( args.size() < 2 ) {
		logger::warning["function"] << "str_contains needs 2 arguments: string, needle" << std::endl;
		return (double)0;
	}

	return (double)(args[0].to_string().find(args[1].to_string()) != std::string::npos ? 1 : 0);
}

expr::VARIABLE expr::functions::str_replace(const expr::FUNCTION_ARGS& args) {

	if ( args.size() < 3 ) {
		logger::warning["function"] << "str_replace needs 3 arguments: string, search, replace" << std::endl;
		return args.empty() ? "" : args[0].to_string();
	}

	std::string s      = args[0].to_string();
	std::string search = args[1].to_string();
	std::string repl   = args[2].to_string();

	if ( search.empty()) return s;

	std::string result;
	size_t pos = 0, found;
	while (( found = s.find(search, pos)) != std::string::npos ) {
		result += s.substr(pos, found - pos);
		result += repl;
		pos = found + search.size();
	}
	result += s.substr(pos);
	return result;
}

expr::VARIABLE expr::functions::str_find(const expr::FUNCTION_ARGS& args) {

	if ( args.size() < 2 ) {
		logger::warning["function"] << "str_find needs 2 arguments: string, needle [, start_pos]" << std::endl;
		return (double)-1;
	}

	std::string s      = args[0].to_string();
	std::string needle = args[1].to_string();
	size_t      start  = args.size() > 2 ? (size_t)args[2].to_int() : 0;

	size_t pos = s.find(needle, start);
	return pos == std::string::npos ? (double)-1 : (double)(int)pos;
}

// ── Numeric helpers ───────────────────────────────────────────────────────────

expr::VARIABLE expr::functions::abs_val(const expr::FUNCTION_ARGS& args) {

	return args.empty() ? (double)0 : std::abs(arg_to_double(args[0]));
}

expr::VARIABLE expr::functions::sign_val(const expr::FUNCTION_ARGS& args) {

	if ( args.empty()) return (double)0;
	double d = arg_to_double(args[0]);
	return d > 0 ? (double)1 : (d < 0 ? (double)-1 : (double)0);
}

expr::VARIABLE expr::functions::clamp_val(const expr::FUNCTION_ARGS& args) {

	if ( args.size() < 3 ) {
		logger::warning["function"] << "clamp needs 3 arguments: value, min, max" << std::endl;
		return args.empty() ? (double)0 : arg_to_double(args[0]);
	}

	double v  = arg_to_double(args[0]);
	double lo = arg_to_double(args[1]);
	double hi = arg_to_double(args[2]);
	return std::clamp(v, lo, hi);
}

expr::VARIABLE expr::functions::map_range(const expr::FUNCTION_ARGS& args) {

	if ( args.size() < 5 ) {
		logger::warning["function"] << "map_range needs 5 arguments: value, in_min, in_max, out_min, out_max" << std::endl;
		return (double)0;
	}

	double v      = arg_to_double(args[0]);
	double in_min = arg_to_double(args[1]);
	double in_max = arg_to_double(args[2]);
	double out_min = arg_to_double(args[3]);
	double out_max = arg_to_double(args[4]);

	if ( in_max == in_min ) {
		logger::warning["function"] << "map_range: in_min == in_max, division by zero" << std::endl;
		return out_min;
	}

	double result = out_min + (v - in_min) / (in_max - in_min) * (out_max - out_min);
	return std::clamp(result, std::min(out_min, out_max), std::max(out_min, out_max));
}

expr::VARIABLE expr::functions::frac(const expr::FUNCTION_ARGS& args) {

	if ( args.empty()) return (double)0;
	double d = arg_to_double(args[0]);
	return d - std::floor(d);
}

expr::VARIABLE expr::functions::trunc_val(const expr::FUNCTION_ARGS& args) {

	return args.empty() ? (double)0 : std::trunc(arg_to_double(args[0]));
}

expr::VARIABLE expr::functions::asin_fn(const expr::FUNCTION_ARGS& args) {

	return args.empty() ? (double)0 : std::asin(arg_to_double(args[0]));
}

expr::VARIABLE expr::functions::acos_fn(const expr::FUNCTION_ARGS& args) {

	return args.empty() ? (double)0 : std::acos(arg_to_double(args[0]));
}

expr::VARIABLE expr::functions::atan_fn(const expr::FUNCTION_ARGS& args) {

	return args.empty() ? (double)0 : std::atan(arg_to_double(args[0]));
}

expr::VARIABLE expr::functions::atan2_fn(const expr::FUNCTION_ARGS& args) {

	if ( args.size() < 2 ) {
		logger::warning["function"] << "atan2 needs 2 arguments: y, x" << std::endl;
		return (double)0;
	}
	return std::atan2(arg_to_double(args[0]), arg_to_double(args[1]));
}

expr::VARIABLE expr::functions::hypot_fn(const expr::FUNCTION_ARGS& args) {

	if ( args.size() < 2 ) {
		logger::warning["function"] << "hypot needs 2 arguments: x, y" << std::endl;
		return (double)0;
	}
	return std::hypot(arg_to_double(args[0]), arg_to_double(args[1]));
}

// ── Formatting ────────────────────────────────────────────────────────────────

expr::VARIABLE expr::functions::format_number(const expr::FUNCTION_ARGS& args) {

	if ( args.empty()) return "";

	double d         = arg_to_double(args[0]);
	int    precision = args.size() > 1 ? std::clamp(args[1].to_int(), 0, 15) : 2;

	std::ostringstream ss;
	ss << std::fixed << std::setprecision(precision) << d;
	return ss.str();
}

expr::VARIABLE expr::functions::to_hex(const expr::FUNCTION_ARGS& args) {

	if ( args.empty()) return "0";

	long long n = (long long)arg_to_double(args[0]);
	bool uppercase = args.size() > 1 && args[1].to_int() != 0;

	std::ostringstream ss;
	if ( uppercase ) ss << std::uppercase;
	ss << std::hex << n;
	return ss.str();
}

expr::VARIABLE expr::functions::to_oct(const expr::FUNCTION_ARGS& args) {

	if ( args.empty()) return "0";
	std::ostringstream ss;
	ss << std::oct << (long long)arg_to_double(args[0]);
	return ss.str();
}

expr::VARIABLE expr::functions::to_bin(const expr::FUNCTION_ARGS& args) {

	if ( args.empty()) return "0";

	long long n = (long long)arg_to_double(args[0]);
	if ( n == 0 ) return "0";

	std::string result;
	bool negative = n < 0;
	unsigned long long u = negative ? (unsigned long long)(-n) : (unsigned long long)n;

	while ( u > 0 ) {
		result = char('0' + (u & 1)) + result;
		u >>= 1;
	}

	return negative ? "-" + result : result;
}

// ── Conditional (function-style ternary, avoids parser space-termination) ────

expr::VARIABLE expr::functions::if_fn(const expr::FUNCTION_ARGS& args) {

	if ( args.size() < 3 ) {
		logger::warning["function"] << "if() needs 3 arguments: condition, true_value, false_value" << std::endl;
		return (double)0;
	}

	return arg_to_double(args[0]) != 0 ? args[1] : args[2];
}

// ── Built-in function registry ────────────────────────────────────────────────

expr::FUNCTIONMAP expr::functions::builtin_functions = {

	{ "time", expr::functions::time_unixtime },
	{ "time::timestamp", expr::functions::time_unixtime },
	{ "time::unixtime", expr::functions::time_unixtime },

	{ "time::hour", expr::functions::time_hour },
	{ "time::min", expr::functions::time_min },
	{ "time::sec", expr::functions::time_sec },

	{ "date::day", expr::functions::date_day },
	{ "date::month", expr::functions::date_month },
	{ "date::year", expr::functions::date_year },
	{ "date::weekday", expr::functions::date_weekday },
	{ "date::day::name", expr::functions::date_day_name },

	{ "strftime", expr::functions::strftime },
	{ "put_time", expr::functions::strftime },

	{ "to_string", expr::functions::to_string },
	{ "to_double", expr::functions::to_double },
	{ "to_int", expr::functions::to_int },
	{ "to_number", expr::functions::to_double },
	{ "to_bool", expr::functions::to_bool },

	{ "is_odd", expr::functions::is_odd },
	{ "is_even", expr::functions::is_even },

	{ "sqrt", expr::functions::sqrt },
	{ "exp", expr::functions::exp },
	{ "ln", expr::functions::ln },
	{ "log", expr::functions::log },
	{ "sin", expr::functions::sin },
	{ "cos", expr::functions::cos },
	{ "tan", expr::functions::tan },
	{ "min", expr::functions::min },
	{ "max", expr::functions::max },
	{ "floor", expr::functions::floor },
	{ "ceil", expr::functions::ceil },
	{ "round", expr::functions::round },

	{ "strlen", expr::functions::strlen },
	{ "length", expr::functions::strlen },
	{ "to_upper", expr::functions::to_upper },
	{ "strupper", expr::functions::to_upper },
	{ "to_lower", expr::functions::to_lower },
	{ "strlower", expr::functions::to_lower },
	{ "substr", expr::functions::substr },
	{ "trim", expr::functions::trim },
	{ "strip", expr::functions::trim },
	{ "ltrim", expr::functions::ltrim },
	{ "rtrim", expr::functions::rtrim },
	{ "pad_left", expr::functions::pad_left },
	{ "rpad", expr::functions::pad_left },
	{ "pad_right", expr::functions::pad_right },
	{ "lpad", expr::functions::pad_right },
	{ "str_repeat", expr::functions::str_repeat },
	{ "repeat", expr::functions::str_repeat },
	{ "str_contains", expr::functions::str_contains },
	{ "contains", expr::functions::str_contains },
	{ "str_replace", expr::functions::str_replace },
	{ "replace", expr::functions::str_replace },
	{ "str_find", expr::functions::str_find },
	{ "strpos", expr::functions::str_find },

	{ "abs", expr::functions::abs_val },
	{ "fabs", expr::functions::abs_val },
	{ "sign", expr::functions::sign_val },
	{ "sgn", expr::functions::sign_val },
	{ "clamp", expr::functions::clamp_val },
	{ "map_range", expr::functions::map_range },
	{ "map", expr::functions::map_range },
	{ "frac", expr::functions::frac },
	{ "trunc", expr::functions::trunc_val },
	{ "asin", expr::functions::asin_fn },
	{ "acos", expr::functions::acos_fn },
	{ "atan", expr::functions::atan_fn },
	{ "atan2", expr::functions::atan2_fn },
	{ "hypot", expr::functions::hypot_fn },

	{ "format", expr::functions::format_number },
	{ "number_format", expr::functions::format_number },
	{ "hex", expr::functions::to_hex },
	{ "to_hex", expr::functions::to_hex },
	{ "oct", expr::functions::to_oct },
	{ "to_oct", expr::functions::to_oct },
	{ "bin", expr::functions::to_bin },
	{ "to_bin", expr::functions::to_bin },

	{ "if", expr::functions::if_fn },
	{ "iif", expr::functions::if_fn },
	{ "ternary", expr::functions::if_fn },

};

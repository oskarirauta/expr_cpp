#include <ctime>
#include <iomanip>
#include <cstring>

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
	int i;

	if ( d > 0 )
		i = (int)(d + 0.5);
	else if ( d < 0 )
		i = (int)(d - 0.5);
	else i = 0;

	return i;
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
		pos = (size_t)args[1].to_int();

	logger::error["function"] << "substr('" << s << "', " << pos << ", " << len << ") failure, reason: unknown error" << std::endl;
	return s;
}

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

};

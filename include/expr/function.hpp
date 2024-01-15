#pragma once

#include <vector>
#include <functional>
#include "lowercase_map.hpp"
#include "expr/variable.hpp"

namespace expr {

	typedef std::vector<expr::VARIABLE> FUNCTION_ARGS;
	typedef std::function<expr::VARIABLE(const expr::FUNCTION_ARGS&)> FUNCTION;
	typedef common::lowercase_map<expr::FUNCTION> FUNCTIONMAP;

	namespace functions {

		expr::VARIABLE time_unixtime(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE time_hour(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE time_min(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE time_sec(const expr::FUNCTION_ARGS& args);

		expr::VARIABLE date_day(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE date_month(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE date_year(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE date_weekday(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE date_day_name(const expr::FUNCTION_ARGS& args);

		expr::VARIABLE strftime(const expr::FUNCTION_ARGS& args);

		expr::VARIABLE to_string(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE to_double(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE to_int(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE to_bool(const expr::FUNCTION_ARGS& args);

		expr::VARIABLE is_odd(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE is_even(const expr::FUNCTION_ARGS& args);

		expr::VARIABLE sqrt(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE exp(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE ln(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE log(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE sin(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE cos(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE tan(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE min(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE max(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE floor(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE ceil(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE round(const expr::FUNCTION_ARGS& args);

		expr::VARIABLE strlen(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE to_upper(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE to_lower(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE substr(const expr::FUNCTION_ARGS& args);

		extern expr::FUNCTIONMAP builtin_functions;
	}

}

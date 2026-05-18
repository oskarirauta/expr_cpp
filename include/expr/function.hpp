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
		expr::VARIABLE trim(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE ltrim(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE rtrim(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE pad_left(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE pad_right(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE str_repeat(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE str_contains(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE str_replace(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE str_find(const expr::FUNCTION_ARGS& args);

		expr::VARIABLE abs_val(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE sign_val(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE clamp_val(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE map_range(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE frac(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE trunc_val(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE asin_fn(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE acos_fn(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE atan_fn(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE atan2_fn(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE hypot_fn(const expr::FUNCTION_ARGS& args);

		expr::VARIABLE format_number(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE to_hex(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE to_oct(const expr::FUNCTION_ARGS& args);
		expr::VARIABLE to_bin(const expr::FUNCTION_ARGS& args);

		expr::VARIABLE if_fn(const expr::FUNCTION_ARGS& args);

		extern expr::FUNCTIONMAP builtin_functions;
	}

}

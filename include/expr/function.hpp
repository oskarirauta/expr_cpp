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

		expr::VARIABLE time_hour(expr::FUNCTION_ARGS args);
		expr::VARIABLE time_min(expr::FUNCTION_ARGS args);
		expr::VARIABLE time_sec(expr::FUNCTION_ARGS args);

		expr::VARIABLE date_day(expr::FUNCTION_ARGS args);
		expr::VARIABLE date_month(expr::FUNCTION_ARGS args);
		expr::VARIABLE date_year(expr::FUNCTION_ARGS args);
		expr::VARIABLE date_weekday(expr::FUNCTION_ARGS args);
		expr::VARIABLE date_day_name(expr::FUNCTION_ARGS args);

		extern expr::FUNCTIONMAP common;
	}

}

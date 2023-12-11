#pragma once

#include <vector>
#include <functional>
#include "lowercase_map.hpp"
#include "expr/variable.hpp"

namespace expr {

	typedef std::vector<expr::VARIABLE> FUNCTION_ARGS;
	typedef std::function<expr::VARIABLE(const expr::FUNCTION_ARGS&)> FUNCTION;
	typedef common::lowercase_map<expr::FUNCTION> FUNCTIONMAP;

}

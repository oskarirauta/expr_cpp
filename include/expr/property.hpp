#pragma once

#include <string>
#include <variant>

#include "lowercase_map.hpp"
#include "expr/function.hpp"
#include "expr/variable.hpp"
#include "expr/result.hpp"

namespace expr {

	class PROPERTY {

	private:
		common::lowercase_map<std::string> *_map;
		expr::FUNCTIONMAP no_funcs;
		expr::VARIABLEMAP no_vars;
		expr::FUNCTIONMAP *_funcs;
		expr::VARIABLEMAP *_vars;

	public:
		PROPERTY(common::lowercase_map<std::string> *m, expr::FUNCTIONMAP *f = nullptr, expr::VARIABLEMAP *v = nullptr);
		~PROPERTY();

		expr::RESULT operator [](const std::string& k, const std::variant<double, std::string, std::nullptr_t>& def = nullptr);

	};

} // end of namespace expr

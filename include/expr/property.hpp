#pragma once

#include <string>
#include <variant>
#include <utility>

#include "lowercase_map.hpp"
#include "expr/function.hpp"
#include "expr/variable.hpp"
#include "expr/result.hpp"

namespace expr {

	class PROPERTY {

	private:
		common::lowercase_map<std::string> *_map;
		expr::FUNCTIONMAP *_funcs;
		expr::VARIABLEMAP *_vars;

	public:
		PROPERTY(common::lowercase_map<std::string> *m, expr::FUNCTIONMAP *f, expr::VARIABLEMAP *v = nullptr);
		PROPERTY(common::lowercase_map<std::string> *m, expr::VARIABLEMAP *v);
		PROPERTY(common::lowercase_map<std::string> *m);
		~PROPERTY();

		expr::RESULT operator [](const std::string& k, const std::variant<double, std::string, std::nullptr_t>& def = nullptr);

		const std::string raw(const std::string& k);
		const std::string pretty(const std::string& k);
		const expr::expression expression(const std::string& k);

	};

	typedef common::lowercase_map<std::string> PROPERTYMAP;

} // end of namespace expr

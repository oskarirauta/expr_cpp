#include "common.hpp"
#include "logger.hpp"
#include "expr/property.hpp"
#include "expr/expression.hpp"

expr::PROPERTY::PROPERTY(common::lowercase_map<std::string> *m, expr::FUNCTIONMAP *f, expr::VARIABLEMAP *v) {

	this -> _map = m;
	this -> _funcs = f == nullptr ? &this -> no_funcs : f;
	this -> _vars = v == nullptr ? &this -> no_vars : v;

}

expr::PROPERTY::~PROPERTY() {
	this -> _map = nullptr;
	this -> _funcs = nullptr;
	this -> _vars = nullptr;
}

expr::RESULT expr::PROPERTY::operator [](const std::string& k, const std::variant<double, std::string, std::nullptr_t>& def) {

	if ( k.empty() || this -> _map == nullptr || this -> _map -> empty() ||
		!this -> _map -> contains(k) || (*this -> _map)[k].empty())
		return expr::RESULT(def);

	expr::expression e((*this -> _map)[k]);
	std::string pretty = e.operator std::string();

	try {
		expr::RESULT result = e.evaluate(this -> _funcs, this -> _vars);
		if ( !std::holds_alternative<std::nullptr_t>(def) && !std::holds_alternative<std::nullptr_t>(result))
			return expr::RESULT(def);
		else if ( result.is_string() && result.operator std::string().empty()) {

			if ( !std::holds_alternative<std::nullptr_t>(def))
				return expr::RESULT(def);
			else if ( !pretty.empty())
				return expr::RESULT(expr::VARIABLE(pretty));
		}

		return result;
	} catch ( std::runtime_error &err ) {
		logger::verbose["property"] << "expression evaluation failed: " << err.what() << std::endl;
	}

	return expr::RESULT(def);
}

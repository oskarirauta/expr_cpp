#include "common.hpp"
#include "logger.hpp"
#include "expr/property.hpp"
#include "expr/expression.hpp"

expr::PROPERTY::PROPERTY() {

	this -> _map = nullptr;
	this -> _funcs = nullptr;
	this -> _vars = nullptr;
}

expr::PROPERTY::PROPERTY(common::lowercase_map<std::string> *m, expr::FUNCTIONMAP *f, expr::VARIABLEMAP *v) {

	this -> _map = m;
	this -> _funcs = f;
	this -> _vars = v;
}

expr::PROPERTY::PROPERTY(common::lowercase_map<std::string> *m, expr::VARIABLEMAP *v) {

	this -> _map = m;
	this -> _funcs = nullptr;
	this -> _vars = v;
}

expr::PROPERTY::PROPERTY(common::lowercase_map<std::string> *m) {

	this -> _map = m;
	this -> _funcs = nullptr;
	this -> _vars = nullptr;
}

expr::PROPERTY::~PROPERTY() {

	this -> _map = nullptr;
	this -> _funcs = nullptr;
	this -> _vars = nullptr;
}

expr::RESULT expr::PROPERTY::get(const std::string& key, const std::variant<double, std::string, std::nullptr_t>& def) {

	if ( key.empty() || this -> _map == nullptr || this -> _map -> empty() ||
		!this -> _map -> contains(key) || (*this -> _map)[key].empty())
		return expr::RESULT(def);

	expr::expression e((*this -> _map)[key]);
	std::string pretty = e.operator std::string();

	try {
		expr::RESULT result = e.evaluate(this -> _funcs, this -> _vars);

		if (( result.is_string() && !(result.operator std::string()).empty()) || result.is_number()) {

			return result;

		} else if ( !std::holds_alternative<std::nullptr_t>(def) && std::holds_alternative<std::nullptr_t>(result)) {

			return expr::RESULT(def);

		} else if ( result.is_string() && result.operator std::string().empty()) {

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

expr::RESULT expr::PROPERTY::operator [](const std::string& key, const std::variant<double, std::string, std::nullptr_t>& def) {

	return this -> get(key, def);
}

expr::RESULT expr::PROPERTY::operator [](const std::string& key, const int def) {

	return this -> get(key, (double)def);
}

const std::string expr::PROPERTY::raw(const std::string& key) {

	if ( key.empty() || this -> _map == nullptr || this -> _map -> empty() ||
		!this -> _map -> contains(key) || (*this -> _map)[key].empty())
		return "nullptr";

	return (*this -> _map)[key];
}

const std::string expr::PROPERTY::pretty(const std::string& key) {

	if ( key.empty() || this -> _map == nullptr || this -> _map -> empty() ||
		!this -> _map -> contains(key) || (*this -> _map)[key].empty())
		return "nullptr";

	expr::expression e((*this -> _map)[key]);
	return e.operator std::string();
}

const expr::expression expr::PROPERTY::expression(const std::string& key) {

	if ( key.empty() || this -> _map == nullptr || this -> _map -> empty() ||
		!this -> _map -> contains(key) || (*this -> _map)[key].empty())
		return expr::expression();

	return expr::expression((*this -> _map)[key]);
}

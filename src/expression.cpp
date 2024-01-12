#include <sstream>
#include "expr/expression.hpp"

const std::string expr::expression::raw() const {
	return this -> _raw;
}

const std::vector<expr::TOKEN>& expr::expression::tokens() {
	return this -> _tokens;
}

const std::vector<expr::TOKEN> expr::expression::tokens() const {
	return this -> _tokens;
}

expr::expression::operator std::string() const {
	return !this -> _tokens.empty() ? describe(this -> _tokens) : "";
}

const std::string expr::expression::to_string() const {
	return operator std::string();
}

void expr::expression::parse(const std::string& s) {
	this -> _raw = s;
	if ( this -> _tokens.empty())
		this -> _tokens.push_back(expr::TOKEN::UNDEF());
	this -> _tokens = parse_expr(s);
}

expr::expression::expression() {
	this -> _raw = "";
	this -> parse(this -> _raw);
}

expr::expression::expression(const std::string& s) {
	this -> _raw = s;
	this -> parse(this -> _raw);
}

expr::expression::~expression() {
	this -> _raw = std::string();
	this -> _tokens.clear();
}

const std::string describe(const expr::expression& e) {

	auto t = e.tokens();

	if ( t.empty() || ( t.size() == 1 && t[0] == expr::T_UNDEF ))
		return "nullptr";
	return describe(t);
}

std::ostream& expr::operator <<(std::ostream& os, expr::expression const& e) {
	os << describe(e);;
	return os;
}

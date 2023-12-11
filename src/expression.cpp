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

void expr::expression::parse(const std::string& s) {
	// todo: try
	this -> _raw = s;
	this -> _tokens = parse_expr(s);
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
	return describe(e.tokens());;
}

std::ostream& expr::operator <<(std::ostream& os, expr::expression const& e) {
	os << describe(e);;
	return os;
}

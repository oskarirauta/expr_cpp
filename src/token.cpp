#include "common.hpp"
#include "logger.hpp"
#include "expr/token.hpp"

const expr::TYPE expr::TOKEN::type() const {
	return this -> _type;
}

const bool expr::TOKEN::is_type(const expr::TYPE type) const {
	return this -> _type == type;
}

const expr::OP expr::TOKEN::op() const {
	return this -> _op;
}

const bool expr::TOKEN::is_op(const expr::OP op) const {
	return this -> _op == op;
}

const std::string expr::TOKEN::raw() const {
	return this -> _raw;
}

const std::variant<double, std::string, std::nullptr_t> expr::TOKEN::value() const {
	return this -> _value;
}

const std::string expr::TOKEN::name() const {
	return this -> _name;
}

const std::vector<expr::TOKEN> expr::TOKEN::args() const {
	return this -> _args;
}

const std::vector<expr::TOKEN> expr::TOKEN::child() const {
	return this -> _child;
}

const std::vector<expr::TOKEN> expr::TOKEN::cond1() const {
	return this -> _cond1;
}

const std::vector<expr::TOKEN> expr::TOKEN::cond2() const {
	return this -> _cond2;
}

const double expr::TOKEN::raw_double() const {

	try {
		return std::get<double>(this -> _value);
	} catch ( std::bad_variant_access const& e ) {
		logger::error["convert"] << "raw get number failed: " << e.what() << std::endl;
	}

	return 0;
}

const int expr::TOKEN::raw_int() const {
	return (int)this -> raw_double();
}

const std::string expr::TOKEN::raw_string() const {
	try {
		return std::get<std::string>(this -> _value);
	} catch ( std::bad_variant_access const& e ) {
		logger::error["convert"] << "raw get string failed: " << e.what() << std::endl;
	}
	return "";
}

expr::TOKEN& expr::TOKEN::operator=(const expr::TYPE& t) {
	this -> _type = t;
	return *this;
}

expr::TOKEN& expr::TOKEN::operator=(const expr::OP& o) {
	this -> _type = expr::T_OPERATOR;
	this -> _op = o;
	return *this;
}

expr::TOKEN& expr::TOKEN::operator=(const double d) {
	this -> _type = expr::T_NUMBER;
	this -> _value = d;
	return *this;
}

expr::TOKEN& expr::TOKEN::operator=(const int i) {
	this -> _type = expr::T_NUMBER;
	this -> _value = (double)i;
	return *this;
}

expr::TOKEN& expr::TOKEN::operator=(const std::string& s) {
	this -> _type = expr::T_STRING;
	this -> _value = s;
	return *this;
}

expr::TOKEN& expr::TOKEN::operator=(const std::nullptr_t n) {
	this -> _type = expr::T_UNDEF;
	this -> _value = nullptr;
	return *this;
}

expr::TOKEN::operator double() const {

	double n(0);

	if ( this -> is_number()) {
		n = this -> raw_double();
	} else if ( this -> is_string()) {

		std::string s(this -> raw_string());

		try {
			n = std::stod(s);
		} catch ( std::invalid_argument& e ) {
			logger::error["convert"] <<
				"failed to convert string token '" << s <<
				"' to number value" << std::endl;
			n = (double)0;
		}
	} else if ( this -> is_null()) {
		n = 0;
	} else {
		logger::error["convert"] << "failed to convert token to number value" << std::endl;
		n = 0;
	}

	return n;
}

expr::TOKEN::operator int() const {
	return (int)(operator double());
}

expr::TOKEN::operator std::string() const {

	std::string s;

	if ( this -> is_string()) {
		s = this -> raw_string();
	} else if ( this -> is_number()) {
		s = common::to_string(this -> raw_double());
	} else if ( this -> is_null()) {
		logger::vverbose["convert"] << "converted nullptr to value ''" << std::endl;
		s = "null";
	} else {
		logger::error["convert"] << "casting token to string value failed, using value ''" << std::endl;
	}

	return s;
}

const bool expr::TOKEN::is_null() const {
	return this -> is_type(expr::T_UNDEF) || std::holds_alternative<std::nullptr_t>(this -> _value);
}

const bool expr::TOKEN::is_string() const {
	return this -> is_type(expr::T_STRING) && std::holds_alternative<std::string>(this -> _value);
}

const bool expr::TOKEN::is_number() const {
	return this -> is_type(expr::T_NUMBER) && std::holds_alternative<double>(this -> _value);
}

const double expr::TOKEN::to_double() const {
	return operator double();
}

const int expr::TOKEN::to_int() const {
	return operator int();
}

const std::string expr::TOKEN::to_string() const {
	return operator std::string();
}

void expr::TOKEN::reset() {
	this -> _type = expr::T_UNDEF;
	this -> _op = expr::OP_UNDEF;
	this -> _raw = "";
	this -> _value = nullptr;
	this -> _name = "";
	this -> _args.clear();
	this -> _child.clear();
	this -> _cond1.clear();
	this -> _cond2.clear();
}

const std::string describe(const expr::TYPE& type) {

	switch ( type ) {
		case expr::T_UNDEF: return "(undef)";
		case expr::T_NUMBER: return "(number)";
		case expr::T_STRING: return "(string)";
		case expr::T_OPERATOR: return "(operator)";
		case expr::T_VARIABLE: return "(variable)";
		case expr::T_FUNCTION: return "(function)";
		case expr::T_SUB: return "(sub)";
		case expr::T_CONDITIONAL: return "(conditional)";
	}

	return "unknown type";
}

const std::string describe(const expr::OP& op) {

	switch ( op ) {
		case expr::OP_UNDEF: return "(undef)";

		case expr::OP_OR: return "|";
		case expr::OP_AND: return "&";
		case expr::OP_NOT: return "!";
		case expr::OP_NNOT: return "!!";

		case expr::OP_ADD: return "+";
		case expr::OP_SUB: return "-";
		case expr::OP_CAT: return ".";
		case expr::OP_MUL: return "*";
		case expr::OP_DIV: return "/";
		case expr::OP_MOD: return "%";
		case expr::OP_POW: return "^";

		case expr::OP_COM: return ",";

		case expr::OP_NEQ: return "==";
		case expr::OP_NNE: return "!=";
		case expr::OP_NLT: return "<";
		case expr::OP_NLE: return "<=";
		case expr::OP_NGT: return ">";
		case expr::OP_NGE: return ">=";

		case expr::OP_SEQ: return "eq";
		case expr::OP_SNE: return "ne";
		case expr::OP_SLT: return "lt";
		case expr::OP_SLE: return "le";
		case expr::OP_SGT: return "gt";
		case expr::OP_SGE: return "ge";

		case expr::OP_SET: return "=";
	}

	return "unknown operator";
}

const std::string describe(const expr::TOKEN& token) {

	switch ( token.type()) {
		case expr::T_UNDEF: return "undefined";
		case expr::T_NUMBER: return "number(" + token.to_string() + ")";
		case expr::T_STRING: return "string(" + token.to_string() + ")";
		case expr::T_OPERATOR: return "operator(" + describe(token.op()) + ")";
		case expr::T_VARIABLE: return "variable(" + token.name() + ")";
		case expr::T_FUNCTION: return "function(" + token.name() + ")";
		case expr::T_SUB: return "parentheses";
		case expr::T_CONDITIONAL: return "conditional";
	}

	return "unknown type";
}

static const std::string describe(const std::vector<expr::TOKEN>& tokens, bool f_args) {

	if ( tokens.empty())
		return "";

	std::string s;

	for ( auto const &token : tokens ) {

		if ( !s.empty())
			s += ' ';

		switch ( token.type()) {
			case expr::T_OPERATOR:
				if ( token.op() == expr::OP_COM && !s.empty())
					s.pop_back();
				s += describe(token.op());
				break;
			case expr::T_VARIABLE:
				s += common::to_lower(token.name());
				break;
			case expr::T_FUNCTION:
				s += common::to_lower(token.name()) + "(" + describe(token.args(), true) + ")";
				break;
			case expr::T_SUB:
				s += "( " + describe(token.child(), false) + " )";
				break;
			case expr::T_CONDITIONAL:
				s += "?";
				if ( token.cond1().size() == 1 )
					s += " " + describe(token.cond1(), false) + " :";
				else s += " ( " + describe(token.cond1(), false) + " ) :";
				if ( token.cond2().size() == 1 )
					s += " " + describe(token.cond2(), false);
				else s += " ( " + describe(token.cond2(), false) + " )";
				break;
			case expr::T_UNDEF:
				s += "T_UNDEF";
				break;
			default:
				if ( token.is_null()) s += "NULL";
				else if ( token.is_string()) s += "'" + token.to_string() + "'";
				else if ( token.is_number()) s += common::to_string(token.to_double());
				else s += "UNK";
		}
	}

	return s;
}

std::ostream& expr::operator <<(std::ostream& os, expr::TOKEN const& t) {

	switch ( t.type()) {
		case expr::T_UNDEF: os << "null"; break;
		case expr::T_NUMBER: os << t.to_double(); break;
		case expr::T_STRING: os << t.to_string(); break;
		case expr::T_OPERATOR: os << "operator(" << describe(t.op()) << ")"; break;
		case expr::T_VARIABLE: os << "variable(" << t.name() << ")"; break;
		case expr::T_FUNCTION: os << "function(" << t.name() << ")"; break;
		case expr::T_SUB: os << "parentheses"; break;
		case expr::T_CONDITIONAL: os << "conditional"; break;
	}

	return os;
}

std::ostream& operator <<(std::ostream& os, expr::TYPE const &t) {
	os << describe(t);
	return os;
}

std::ostream& operator <<(std::ostream& os, expr::OP const& o) {
	os << describe(o);
	return os;
}

std::ostream& operator <<(std::ostream& os, std::vector<expr::TOKEN> const& t) {
	os << describe(t, false);
	return os;
}

const std::string describe(const std::vector<expr::TOKEN>& tokens) {
	return describe(tokens, false);
}

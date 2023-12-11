#include "common.hpp"
#include "logger.hpp"
#include "expr/variable.hpp"

expr::VARIABLE::VARIABLE() {
	this -> emplace<std::nullptr_t>(std::forward<decltype(nullptr)>(nullptr));
}

expr::VARIABLE::VARIABLE(const int i) {
	double d(i);
	this -> emplace<double>(std::forward<decltype(d)>(d));
}

expr::VARIABLE::VARIABLE(const double d) {
	this -> emplace<double>(std::forward<decltype(d)>(d));
}

expr::VARIABLE::VARIABLE(const std::string& s) {
	this -> emplace<std::string>(std::forward<decltype(s)>(s));
}

const expr::VAR_TYPE expr::VARIABLE::type() const {

	if ( std::holds_alternative<double>(*this)) return expr::V_NUMBER;
	else if ( std::holds_alternative<std::string>(*this)) return expr::V_STRING;
	else return expr::V_NULLPTR;
}

const bool expr::VARIABLE::is_type(const expr::VAR_TYPE type) const {
	return this -> type() == type;
}

const double expr::VARIABLE::raw_double() const {

	try {
		return std::get<double>(*this);
	} catch ( std::bad_variant_access const& e ) {
		logger::error << logger::tag("convert") << "raw get number failed: " << e.what() << std::endl;
	}

	return 0;
}

const int expr::VARIABLE::raw_int() const {
	return (int)this -> raw_double();
}

const std::string expr::VARIABLE::raw_string() const {

	try {
		return std::get<std::string>(*this);
	} catch ( std::bad_variant_access const& e ) {
		logger::error << logger::tag("convert") << "raw get string failed: " << e.what() << std::endl;
	}
	return "";
}

expr::VARIABLE::operator double() const {

	double n(0);

	switch ( this -> type()) {
		case expr::V_NUMBER:
				n = this -> raw_double();
				break;
		case expr::V_STRING:
				{
					std::string s(this -> raw_string());

					try {
						n = std::stod(s);
					} catch ( std::invalid_argument& e ) {
						logger::error << logger::tag("convert") <<
							"failed to convert string '" << s <<
							"' to number value, using value 0" << std::endl;
						n = (double)0;
					}
				}
				break;
		case expr::V_NULLPTR:
				logger::vverbose << logger::tag("convert") <<
					"converted nullptr to value 0" << std::endl;
				break;
		default:
				logger::warning << logger::tag("convert") <<
					"casting from unknown type to number failed, using value 0" << std::endl;
	}
	return n;
}

expr::VARIABLE::operator int() const {
	return (int)(operator double());
}

expr::VARIABLE::operator std::string() const {

	std::string s;

	switch ( this -> type()) {
		case expr::V_NUMBER:
				s = common::to_string(this -> raw_double());
				break;
		case expr::V_STRING:
				s = this -> raw_string();
				break;
		case expr::V_NULLPTR:
				logger::vverbose << logger::tag("convert") <<
					"converted nullptr to value ''" << std::endl;
				s = "";
				break;
		default:
				logger::warning << logger::tag("convert") <<
					"casting from unknown type to string failed, using value ''" << std::endl;
				s = "";
	}

	return s;
}

const double expr::VARIABLE::to_double() const {
	return operator double();
}

const int expr::VARIABLE::to_int() const {
	return operator int();
}

const std::string expr::VARIABLE::to_string() const {
	return operator std::string();
}

const bool expr::VARIABLE::is_null() const {
	return this -> type() == V_NULLPTR;
}

const bool expr::VARIABLE::is_string() const {
	return this -> type() == V_STRING;
}

const bool expr::VARIABLE::is_number() const {
	return this -> type() == V_NUMBER;
}

const std::string expr::VARIABLE::describe() const {

	switch ( this -> type()) {
		case expr::V_NUMBER: return "number(" + this -> to_string() + ")";
		case expr::V_STRING: return "string(" + this -> raw_string() + ")";
		case expr::V_NULLPTR: return "nullptr(NULL)";
		default: return "unknown variable type(UNDEF)";
	}
}

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

expr::VARIABLE::VARIABLE(const expr::VARIABLE& other) {

	if ( other.is_string()) {
		std::string s = other.operator std::string();
		this -> emplace<std::string>(std::forward<decltype(s)>(s));
	} else if ( other.is_number()) {
		double d = other.operator double();
		this -> emplace<double>(std::forward<decltype(d)>(d));
	} else this -> emplace<std::nullptr_t>(std::forward<decltype(nullptr)>(nullptr));
}

expr::VARIABLE::VARIABLE(const std::variant<double, std::string, std::nullptr_t>&v) {

	if ( std::holds_alternative<double>(v)) {

		double d;
		try {
			d = std::get<double>(v);
		} catch ( std::bad_variant_access const& err ) {
			logger::error["convert"] << "bad variant access raised: " << err.what() << std::endl;
			d = 0;
		}

		this -> emplace<double>(std::forward<decltype(d)>(d));
	} else if ( std::holds_alternative<std::string>(v)) {

		std::string s;
		try {
			s = std::get<std::string>(v);
		} catch ( std::bad_variant_access const& err ) {
			logger::error["convert"] << "bad variant access raised: " << err.what() << std::endl;
			s = "";
		}

		this -> emplace<std::string>(std::forward<decltype(s)>(s));
	} else this -> emplace<std::nullptr_t>(std::forward<decltype(nullptr)>(nullptr));
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
		logger::error["convert"] << "raw get number failed: " << e.what() << std::endl;
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
		logger::error["convert"] << "raw get string failed: " << e.what() << std::endl;
	}
	return "";
}

const expr::VARIABLE expr::VARIABLE::lowercase() const {

	if ( !this -> is_string())
		return *this;

	std::string s = operator std::string();
	return expr::VARIABLE(common::to_lower(s));
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
						logger::error["convert"] <<
							"failed to convert string '" << s <<
							"' to number value, using value 0" << std::endl;
						n = (double)0;
					}
				}
				break;
		case expr::V_NULLPTR:
				logger::vverbose["convert"] << "converted nullptr to value 0" << std::endl;
				break;
		default:
				logger::warning["convert"] << "casting from unknown type to number failed, using value 0" << std::endl;
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
				logger::vverbose["convert"] << "converted nullptr to value ''" << std::endl;
				s = "";
				break;
		default:
				logger::warning["convert"] << "casting from unknown type to string failed, using value ''" << std::endl;
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

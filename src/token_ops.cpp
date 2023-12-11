#include <iostream>
#include <string>
#include <cerrno>
#include <cfenv>
#include <cmath>

#include "common.hpp"
#include "logger.hpp"
#include "expr/token.hpp"

static void reset_math_errors() {

	if ( math_errhandling & MATH_ERREXCEPT ) feclearexcept(FE_ALL_EXCEPT);
}

static bool math_error_handler(const std::string& name, const double n1, const double n2, const std::string& sign) {

	bool ret = false;

	if ( math_errhandling & MATH_ERRNO && ( errno == EDOM || errno == ERANGE )) {

		ret = true;
		logger::warning << logger::tag("evaluate") << "math " << ( errno == EDOM ? "domain" : "range" ) <<
					" error on " << name << "( " << n1 << " " << sign << " " << n2 << " )" << std::endl;
	}

	if ( math_errhandling & MATH_ERREXCEPT ) {

		if ( fetestexcept(FE_INEXACT)) {
			// do not set ret = true here, we are good with rounded result
			logger::warning << logger::tag("evaluate") << "inexact result" <<
				" error on " << name << "( " << n1 << " " << sign << " " << n2 << " )" <<
				" , result was rounded to fit in the data type" << std::endl;
		}

		if ( fetestexcept(FE_INVALID)) {
			ret = true;
			logger::warning << logger::tag("evaluate") << "math domain" <<
				" error on " << name << "( " << n1 << " " << sign << " " << n2 << " )" << std::endl;
		}

		if ( fetestexcept(FE_DIVBYZERO)) {
			ret = true;
			logger::warning << logger::tag("evaluate") << "math division" <<
				" by zero error raised by " << name << "( " << n1 << " " << sign << " " << n2 << " )" << std::endl;
		}

		if ( fetestexcept(FE_OVERFLOW)) {
			ret = true;
			logger::warning << logger::tag("evaluate") << "range overflow error" <<
				" was raised by " << name << "( " << n1 << " " << sign << " " << n2 << " )" << std::endl;
		}

		if ( fetestexcept(FE_UNDERFLOW)) {
			ret = true;
			logger::warning << logger::tag("evaluate") << "range undeflow error" <<
				" was raised by " << name << "( " << n1 << " " << sign << " " << n2 << " )" << std::endl;
		}
	}

	return ret;
}

expr::TOKEN expr::TOKEN::UNDEF() {

	expr::TOKEN token;
	return token;
}

expr::TOKEN expr::TOKEN::SGN(const double n) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = -n;
	return token;
}

expr::TOKEN expr::TOKEN::OR(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = n1 == 0 ? (double)( n2 != 0 ) : (double)1;
	return token;
}

expr::TOKEN expr::TOKEN::AND(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = 1 != 0 ? (double)( n2 != 0 ) : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::NOT(const double n) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = (double)( n == 0 );
	return token;
}

expr::TOKEN expr::TOKEN::NNOT(const double n) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = (double)!( n == 0 );
	return token;
}

expr::TOKEN expr::TOKEN::ADD(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = n1 + n2;
	return token;
}

expr::TOKEN expr::TOKEN::SUB(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = n1 - n2;
	return token;
}

expr::TOKEN expr::TOKEN::CAT(const std::string& s1, const std::string& s2) {

	expr::TOKEN token;
	token._type = expr::T_STRING;
	token._value = std::string(s1 + s2);
	return token;
}

expr::TOKEN expr::TOKEN::MUL(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = n1 * n2;
	return token;
}

expr::TOKEN expr::TOKEN::DIV(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	if ( n2 == 0 ) {
		logger::warning << logger::tag("evaluate") << "division by zero (" << n1 << " / 0 )" << std::endl;
		token._value = (double)0;
	} else if ( n1 == 0 ) {
		token._value = (double)0;
	} else {
		reset_math_errors();
		token._value = n1 / n2;
		if ( math_error_handler("divide", n1, n2, "/")) token._value = (double)0;
	}
	return token;
}

expr::TOKEN expr::TOKEN::MOD(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	if ( n2 == 0 ) {
		logger::warning << logger::tag("evaluate") << "modulo by zero (" << n1 << " % 0 )" << std::endl;
		token._value = (double)0;
	} else {
		reset_math_errors();
		token._value = std::fmod(n1, n2);
		if ( math_error_handler("modulo", n1, n2, "%")) token._value = (double)0;
	}
	return token;
}

expr::TOKEN expr::TOKEN::POW(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = std::pow(n1, n2);
	return token;
}

expr::TOKEN expr::TOKEN::NEQ(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = T_NUMBER;
	token._value = n1 == n2 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::NNE(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = n1 != n2 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::NLT(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = n1 < n2 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::NLE(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = n1 <= n2 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::NGT(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = n1 > n2 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::NGE(const double n1, const double n2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = n1 >= n2 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::SEQ(const std::string& s1, const std::string& s2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = s1.compare(s2) == 0 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::SNE(const std::string& s1, const std::string& s2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = s1.compare(s2) != 0 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::SLT(const std::string& s1, const std::string& s2) {

	expr::TOKEN token;
	token._type = T_NUMBER;
	token._value = s1.compare(s2) < 0 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::SLE(const std::string& s1, const std::string& s2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = s1.compare(s2) <= 0 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::SGT(const std::string& s1, const std::string& s2) {

	expr::TOKEN token;
	token._type = T_NUMBER;
	token._value = s1.compare(s2) > 0 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::SGE(const std::string& s1, const std::string& s2) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = s1.compare(s2) >= 0 ? (double)1 : (double)0;
	return token;
}

expr::TOKEN expr::TOKEN::NUMBER(int n) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = (double)n;
	return token;
}

expr::TOKEN expr::TOKEN::NUMBER(double n) {

	expr::TOKEN token;
	token._type = expr::T_NUMBER;
	token._value = n;
	return token;
}

expr::TOKEN expr::TOKEN::STRING(const std::string& s) {

	expr::TOKEN token;
	token._type = expr::T_STRING;
	token._value = s;
	return token;
}

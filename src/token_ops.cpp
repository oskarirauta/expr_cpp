#include <iostream>
#include <string>
#include <cerrno>
#include <cfenv>
#include <cmath>

#include "common.hpp"
#include "logger.hpp"
#include "eval/token.hpp"

// #pragma STDC FENV_ACCESS ON

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

TOKEN TOKEN::UNDEF() {

	TOKEN token;
	return token;
}

TOKEN TOKEN::SGN(const double n) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = -n;
	return token;
}

TOKEN TOKEN::OR(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 == 0 ? (double)( n2 != 0 ) : (double)1;
	return token;
}

TOKEN TOKEN::AND(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = 1 != 0 ? (double)( n2 != 0 ) : (double)0;
	return token;
}

TOKEN TOKEN::NOT(const double n) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = (double)( n == 0 );
	return token;
}

TOKEN TOKEN::ADD(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 + n2;
	return token;
}

TOKEN TOKEN::SUB(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 - n2;
	return token;
}

TOKEN TOKEN::CAT(const std::string& s1, const std::string& s2) {

	TOKEN token;
	token.type = T_STRING;
	token.value = std::string(s1 + s2);
	return token;
}

TOKEN TOKEN::MUL(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 * n2;
	return token;
}

TOKEN TOKEN::DIV(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	if ( n2 == 0 ) {
		logger::warning << logger::tag("evaluate") << "division by zero (" << n1 << " / 0 )" << std::endl;
		token.value = (double)0;
	} else if ( n1 == 0 ) {
		token.value = (double)0;
	} else {
		reset_math_errors();
		token.value = n1 / n2;
		if ( math_error_handler("divide", n1, n2, "/")) token.value = (double)0;
	}
	return token;
}

TOKEN TOKEN::MOD(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	if ( n2 == 0 ) {
		logger::warning << logger::tag("evaluate") << "modulo by zero (" << n1 << " % 0 )" << std::endl;
		token.value = (double)0;
	} else {
		reset_math_errors();
		token.value = std::fmod(n1, n2);
		if ( math_error_handler("modulo", n1, n2, "%")) token.value = (double)0;
	}
	return token;
}

TOKEN TOKEN::POW(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = std::pow(n1, n2);
	return token;
}

TOKEN TOKEN::NEQ(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 == n2 ? (double)1 : (double)0;
	return token;
}

TOKEN TOKEN::NNE(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 != n2 ? (double)1 : (double)0;
	return token;
}

TOKEN TOKEN::NLT(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 < n2 ? (double)1 : (double)0;
	return token;
}

TOKEN TOKEN::NLE(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 <= n2 ? (double)1 : (double)0;
	return token;
}

TOKEN TOKEN::NGT(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 > n2 ? (double)1 : (double)0;
	return token;
}

TOKEN TOKEN::NGE(const double n1, const double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 >= n2 ? (double)1 : (double)0;
	return token;
}

TOKEN TOKEN::SEQ(const std::string& s1, const std::string& s2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = s1.compare(s2) == 0 ? (double)1 : (double)0;
	return token;
}

TOKEN TOKEN::SNE(const std::string& s1, const std::string& s2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = s1.compare(s2) != 0 ? (double)1 : (double)0;
	return token;
}

TOKEN TOKEN::SLT(const std::string& s1, const std::string& s2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = s1.compare(s2) < 0 ? (double)1 : (double)0;
	return token;
}

TOKEN TOKEN::SLE(const std::string& s1, const std::string& s2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = s1.compare(s2) <= 0 ? (double)1 : (double)0;
	return token;
}

TOKEN TOKEN::SGT(const std::string& s1, const std::string& s2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = s1.compare(s2) > 0 ? (double)1 : (double)0;
	return token;
}

TOKEN TOKEN::SGE(const std::string& s1, const std::string& s2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = s1.compare(s2) >= 0 ? (double)1 : (double)0;
	return token;
}

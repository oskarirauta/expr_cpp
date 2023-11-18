#include <iostream>
#include <string>

#include "eval/helpers.hpp"
#include "eval/token.hpp"

TOKEN TOKEN::UNDEF() {

	TOKEN token;
	return token;
}

TOKEN TOKEN::ADD(double n1, double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 + n2;
	return token;
}

TOKEN TOKEN::SUB(double n1, double n2) {

	TOKEN token;
	token.type = T_NUMBER;
	token.value = n1 - n2;
	return token;
}

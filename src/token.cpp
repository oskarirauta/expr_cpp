#include <iostream>
#include <string>

#include "eval/helpers.hpp"
#include "eval/token.hpp"

bool TOKEN::isNull() const {
	return std::holds_alternative<std::nullptr_t>(this -> value) || this -> type == T_UNDEF;
}

bool TOKEN::isString() const {
	return this -> type == T_STRING && std::holds_alternative<std::string>(this -> value);
}

bool TOKEN::isNumber() const {
	return this -> type == T_NUMBER && std::holds_alternative<double>(this -> value);
}

const std::string TOKEN::stringValue() const {

	if ( this -> isString()) {
		return std::get<std::string>(this -> value);
	} else if ( this -> isNumber()) {
		return double2str(std::get<double>(this -> value));
	} else if ( this -> isNull()) {
		return "NULL";
	} else {
		std::cout << "parser: failed to convert token to stringValue" << std::endl;
		return "";
	}
}

const double TOKEN::numberValue() const {

	if ( this -> isNumber()) {
		return std::get<double>(this -> value);
	} else if ( this -> isString()) {

		double n;

		try {
			n = std::stod(std::get<std::string>(this -> value));
		} catch ( std::invalid_argument& e ) {
			std::cout << "parser: error, cannot convert '" << std::get<std::string>(this -> value) << "' to number" << std::endl;
			n = 0;
		}
		return n;
	} else if ( this -> isNull()) {
		return 0;
	} else {
		std::cout << "failed to convert token to numberValue" << std::endl;
		return 0;
	}
}

void TOKEN::reset() {
	this -> type = T_UNDEF;
	this -> op = OP_UNDEF;
	this -> rawString = "";
	this -> value = nullptr;
	this -> name = "";
	this -> args.clear();
	this -> child.clear();
	this -> value = nullptr;
}


const std::string describe(const OP& op) {

	switch ( op ) {
		case OP_UNDEF: return "";
		case OP_ADD: return "+";
		case OP_SUB: return "-";
		case OP_CAT: return ".";

		case OP_COM: return ",";

		case OP_NEQ: return "==";
		case OP_NNE: return "!=";
		case OP_NLE: return "<=";
		case OP_NGE: return ">=";

		case OP_SEQ: return "eq";
		case OP_SNE: return "ne";
		case OP_SLT: return "lt";
		case OP_SLE: return "le";
		case OP_SGT: return "gt";
		case OP_SGE: return "ge";

		case OP_SET: return "=";
		case OP_CND: return "?";
	}

	return "";
}

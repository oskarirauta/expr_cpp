#pragma once

#include <string>
#include <vector>
#include <variant>

enum TYPE {

	T_NUMBER,
	T_STRING,
	T_OPERATOR,
	T_VARIABLE,
	T_FUNCTION,
	T_SUB,
	T_UNDEF
};

enum OP {

	OP_UNDEF,
	OP_ADD,
	OP_SUB,
	OP_CAT,

	OP_COM,

	OP_NEQ,
	OP_NNE,
	OP_NLE,
	OP_NGE,

	OP_SEQ,
	OP_SNE,
	OP_SLT,
	OP_SLE,
	OP_SGT,
	OP_SGE,
	OP_SET,
	OP_CND
};

struct TOKEN {

	TYPE	type	= T_UNDEF;
	OP	op	= OP_UNDEF;

	std::string rawString = "";
	std::variant<double, std::string, std::nullptr_t> value = nullptr;
	std::string name = "";
	std::vector<TOKEN> args;
	std::vector<TOKEN> child;

	bool isNull() const;
	bool isString() const;
	bool isNumber() const;

	const std::string stringValue() const;
	const double numberValue() const;

	void reset();

	static TOKEN UNDEF();
	static TOKEN ADD(double n1, double n2);
	static TOKEN SUB(double n1, double n2);

};

const std::string describe(const OP& op);

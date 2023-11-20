#pragma once

#include <string>
#include <vector>
#include <variant>

enum TYPE {

	T_UNDEF,
	T_NUMBER,
	T_STRING,
	T_OPERATOR,
	T_VARIABLE,
	T_FUNCTION,
	T_CONDITIONAL,
	T_SUB
};

// skip SGN, we handled it with SUB
enum OP {

	OP_UNDEF,

	OP_OR,
	OP_AND,
	OP_NOT,

	OP_ADD,
	OP_SUB,
	OP_CAT,
	OP_MUL,
	OP_DIV,
	OP_MOD,
	OP_POW,

	OP_NEQ,
	OP_NNE,
	OP_NLT,
	OP_NLE,
	OP_NGT,
	OP_NGE,

	OP_SEQ,
	OP_SNE,
	OP_SLT,
	OP_SLE,
	OP_SGT,
	OP_SGE,

	OP_SET,
	OP_COM
};

struct TOKEN {

	TYPE	type	= T_UNDEF;
	OP	op	= OP_UNDEF;

	std::string rawString = "";
	std::variant<double, std::string, std::nullptr_t> value = nullptr;
	std::string name = "";
	std::vector<TOKEN> args;
	std::vector<TOKEN> child;
	std::vector<TOKEN> cond1;
	std::vector<TOKEN> cond2;

	bool isNull() const;
	bool isString() const;
	bool isNumber() const;

	const std::string stringValue() const;
	const double numberValue() const;

	void reset();

	static TOKEN UNDEF();
	static TOKEN SGN(const double n);
	static TOKEN OR(const double n1, const double n2);
	static TOKEN AND(const double n1, const double n2);
	static TOKEN NOT(const double n);
	static TOKEN ADD(const double n1, const double n2);
	static TOKEN SUB(const double n1, const double n2);
	static TOKEN CAT(const std::string& s1, const std::string& s2);
	static TOKEN MUL(const double n1, const double n2);
	static TOKEN DIV(const double n1, const double n2);
	static TOKEN MOD(const double n1, const double n2);
	static TOKEN POW(const double n1, const double n2);
	static TOKEN NEQ(const double n1, const double n2);
	static TOKEN NNE(const double n1, const double n2);
	static TOKEN NLT(const double n1, const double n2);
	static TOKEN NLE(const double n1, const double n2);
	static TOKEN NGT(const double n1, const double n2);
	static TOKEN NGE(const double n1, const double n2);
	static TOKEN SEQ(const std::string& s1, const std::string& s2);
	static TOKEN SNE(const std::string& s1, const std::string& s2);
	static TOKEN SLT(const std::string& s1, const std::string& s2);
	static TOKEN SLE(const std::string& s1, const std::string& s2);
	static TOKEN SGT(const std::string& s1, const std::string& s2);
	static TOKEN SGE(const std::string& s1, const std::string& s2);

};

const std::string describe(const TYPE& type);
const std::string describe(const OP& op);

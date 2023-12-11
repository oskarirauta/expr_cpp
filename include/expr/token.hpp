#pragma once

#include <string>
#include <vector>
#include <variant>

namespace expr {

	class expression;

	enum TYPE {
		T_UNDEF,
		T_NUMBER, T_STRING,
		T_OPERATOR, T_VARIABLE, T_FUNCTION,
		T_CONDITIONAL, T_SUB
	};

	enum OP {
		OP_UNDEF,
		OP_OR, OP_AND, OP_NOT, OP_NNOT,
		OP_ADD, OP_SUB, OP_CAT, OP_MUL, OP_DIV, OP_MOD, OP_POW,
		OP_NEQ, OP_NNE, OP_NLT, OP_NLE, OP_NGT, OP_NGE,
		OP_SEQ, OP_SNE, OP_SLT, OP_SLE, OP_SGT, OP_SGE,
		OP_SET, OP_COM
	};

	class TOKEN {

	friend class expression;

	private:
		TYPE	_type	= T_UNDEF;
        	OP	_op	= OP_UNDEF;

		std::string _raw = "";
		std::variant<double, std::string, std::nullptr_t> _value = nullptr;
		std::string _name = "";
		std::vector<TOKEN> _args;
		std::vector<TOKEN> _child;
		std::vector<TOKEN> _cond1;
		std::vector<TOKEN> _cond2;

		TOKEN& operator=(const TYPE& t);
		TOKEN& operator=(const OP& o);
		TOKEN& operator=(const double d);
		TOKEN& operator=(const int i);
		TOKEN& operator=(const std::string& s);
		TOKEN& operator=(const std::nullptr_t n);

	public:

		const TYPE type() const;
		const bool is_type(const TYPE type) const;

		const OP op() const;
		const bool is_op(const OP op) const;

		const std::string raw() const;
		const std::variant<double, std::string, std::nullptr_t> value() const;
		const std::string name() const;
		const std::vector<TOKEN> args() const;
		const std::vector<TOKEN> child() const;
		const std::vector<TOKEN> cond1() const;
		const std::vector<TOKEN> cond2() const;

		const double raw_double() const;
		const int raw_int() const;
		const std::string raw_string() const;

		operator double() const;
		operator int() const;
		operator std::string() const;

		const bool operator==(const TYPE& other) const {
			return this -> _type == other;
		}

		const bool operator!=(const TYPE& other) const {
			return this -> _type != other;
		}

		const bool operator==(const OP& other) const {
			return this -> _type == T_OPERATOR && this -> _op == other;
		}

		const bool operator!=(const OP& other) const {
			return this -> _type != T_OPERATOR && this -> _op != other;
		}

		const bool is_null() const;
		const bool is_string() const;
		const bool is_number() const;

		const double to_double() const;
		const int to_int() const;
		const std::string to_string() const;

		void reset();

		static TOKEN UNDEF();
		static TOKEN NUMBER(int n);
		static TOKEN NUMBER(double n);
		static TOKEN STRING(const std::string& s);

		static TOKEN SGN(const double n);
		static TOKEN OR(const double n1, const double n2);
		static TOKEN AND(const double n1, const double n2);
		static TOKEN NOT(const double n);
		static TOKEN NNOT(const double n);
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

		friend std::ostream& operator <<(std::ostream& os, TOKEN const& t);
	};

	std::ostream& operator <<(std::ostream& os, TOKEN const& t);

} // end of namespace expr

const std::string describe(const expr::TYPE& type);
const std::string describe(const expr::OP& op);
const std::string describe(const expr::TOKEN& token);
const std::string describe(const std::vector<expr::TOKEN>& tokens);

std::ostream& operator <<(std::ostream& os, expr::TYPE const& t);
std::ostream& operator <<(std::ostream& os, expr::OP const& o);
std::ostream& operator <<(std::ostream& os, std::vector<expr::TOKEN> const& tokens);

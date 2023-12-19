#pragma once

#include <string>
#include <vector>
#include <variant>
#include <functional>
#include "lowercase_map.hpp"
#include "expr/variable.hpp"
#include "expr/function.hpp"
#include "expr/result.hpp"
#include "expr/token.hpp"

namespace expr {

	class expression {

	private:

		std::string _raw;
		std::vector<TOKEN> _tokens;

	public:

		const std::string raw() const;
		const std::vector<TOKEN>& tokens();
		const std::vector<TOKEN> tokens() const;

		operator std::string() const;
		const std::string to_string() const;

		expression(const std::string& s);
		~expression();

		void parse(const std::string& s);
		TOKEN evaluate(FUNCTIONMAP *functions = nullptr, VARIABLEMAP *variables = nullptr);
		TOKEN evaluate(const std::string& s, FUNCTIONMAP *functions, VARIABLEMAP *variables);

		friend std::ostream& operator <<(std::ostream& os, expression const& e);

	private:
		// internal parser functions
		static std::vector<TOKEN> parse_expr(const std::string& expr, bool f_args);
		static bool validate_set_op(std::vector<TOKEN>& tokens, const std::string& expr, bool is_root);
		std::vector<TOKEN> parse_expr(const std::string& s);

		// internal evaluation functions
		static std::vector<std::vector<TOKEN>> get_arg_tokens(const std::vector<TOKEN>& tokens);
		static const VARIABLE get_variable_value(const std::string& name, VARIABLEMAP *variables);
		static TOKEN tokenize_variable_value(const std::string& name, VARIABLEMAP *variables);
		static std::vector<TOKEN> eval_functions(
			std::vector<TOKEN>& tokens, FUNCTIONMAP *functions, VARIABLEMAP *variables);
		static std::vector<TOKEN> eval_variables(
			std::vector<TOKEN>& tokens, FUNCTIONMAP *functions, VARIABLEMAP *variables);
		static std::vector<TOKEN> eval_parentheses(std::vector<TOKEN>& tokens,
			FUNCTIONMAP *functions, VARIABLEMAP *variables);
		static std::vector<TOKEN> eval_conditionals(
			std::vector<TOKEN>&tokens, FUNCTIONMAP *functions, VARIABLEMAP *variables);
		static void process_rhs_token(std::vector<TOKEN>& tokens, size_t index1, size_t index2);
		static std::vector<TOKEN> eval(
			std::vector<TOKEN>& tokens, bool f_args, FUNCTIONMAP *functions, VARIABLEMAP *variables);
		TOKEN evaluate(std::vector<TOKEN>& tokens, FUNCTIONMAP *functions, VARIABLEMAP *variables);

	};

	std::ostream& operator <<(std::ostream& os, expression const& e);

} // end of namespace expr

const std::string describe(const expr::expression& e);

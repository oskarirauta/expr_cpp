#include <stdexcept>
#include <utility>

#include "common.hpp"
#include "logger.hpp"
#include "eval/token.hpp"
#include "eval/parser.hpp"
#include "eval/evaluation.hpp"

static const std::string find_function(const std::string& name, const FUNCTIONS *functions) {

	if ( functions == nullptr || name.empty())
		return "";

	for ( const auto& [key, value] : *functions )
		if ( common::to_lower(std::as_const(key)) == common::to_lower(std::as_const(name)))
			return key;

	return "";
}

static std::vector<std::vector<TOKEN>> get_arg_tokens(const std::vector<TOKEN>& tokens) {

	std::vector<std::vector<TOKEN>> args;
	std::vector<TOKEN> arg;

	for ( const TOKEN& token : tokens ) {

		if ( token.type == T_OPERATOR && token.op == OP_COM ) {
			args.push_back(arg);
			arg.clear();
		} else arg.push_back(token);
	}

	args.push_back(arg);

	return args;
}

static const std::string find_variable(const std::string& name, const VARIABLES *variables) {

	if ( variables == nullptr || name.empty())
		return "";

	for ( const auto& [key, value] : *variables )
		if ( common::to_lower(std::as_const(key)) == common::to_lower(std::as_const(name)))
			return key;

	return "";
}

static const VARIABLE get_variable_value(const std::string& name, const VARIABLES *variables) {

	VARIABLE result = nullptr;

	if ( variables == nullptr || name.empty() || !variables -> contains(name))
		return result;

	for ( const auto& [key, value] : *variables )
		if ( common::to_lower(std::as_const(key)) == common::to_lower(std::as_const(name)))
			return value;

	return result;
}

static TOKEN tokenize_variable_value(const std::string& name, const VARIABLES *variables) {

	TOKEN tok;

	if ( std::string v_name = find_variable(name, variables); !v_name.empty()) {

		VARIABLE v = get_variable_value(v_name, variables);

		if ( std::holds_alternative<std::string>(v)) {
			tok.type = T_STRING;
			tok.value = std::get<std::string>(v);
		} else if ( std::holds_alternative<double>(v)) {
			tok.type = T_NUMBER;
			tok.value = std::get<double>(v);
		}
	}

	return tok;
}

static std::vector<TOKEN> eval(std::vector<TOKEN>& tokens, bool f_args, const FUNCTIONS *functions, VARIABLES *variables);

static std::vector<TOKEN> eval_functions(std::vector<TOKEN>& tokens, const FUNCTIONS *functions, VARIABLES *variables) {

	// evaluate functions first
	for ( size_t i = 0; i < tokens.size(); i++ ) {

		TOKEN tok;

		if ( tokens[i].type != T_FUNCTION ) continue;

		if ( tokens[i].name.empty()) {
			tokens[i] = tok; // T_UNDEF
			return tokens;
		}

		// TODO: use try and abort bool,,
		if ( std::string f_name = find_function(tokens[i].name, functions); !f_name.empty()) {

			std::vector<std::vector<TOKEN>> args = get_arg_tokens(tokens[i].args);
			FUNCTION_ARGS f_args;

			for ( size_t a = 0; a < args.size(); a++ ) {

				eval_f_arg:

				while ( args[a].size() > 1 ) {
					try {
						args[a] = eval(args[a], false, functions, variables);
					} catch ( std::runtime_error& e ) {
						throw e;
					}
				}

				if ( args[a].size() == 1 && args[a].front().type == T_VARIABLE ) {

					try {
						args[a] = eval(args[a], false, functions, variables);
					} catch ( std::runtime_error& e ) {
						throw e;
					}
					goto eval_f_arg;
				}

				if ( args[a].size() == 1 && args[a].front().type == T_FUNCTION ) {

					try {
						args[a] = eval(args[a], false, functions, variables);
					} catch ( std::runtime_error& e ) {
						throw e;
					}
					goto eval_f_arg;
				}

				if ( args[a].size() == 1 && args[a].front().type == T_SUB && !args[a].front().child.empty()) {
					args[a] = args[a].front().child;
					goto eval_f_arg;
				}

				if ( args[a].size() == 1 ) {

					FUNCTION_ARG arg;

					if ( args[a][0].isNumber())
						arg = args[a][0].numberValue();
					else if ( args[a][0].isString())
						arg = args[a][0].stringValue();
					else arg = nullptr;

					f_args.push_back(arg);

				} else f_args.push_back(nullptr);
			}

			FUNCTION_ARG arg = nullptr;

			for ( const auto& [key, value] : *functions ) {
				if ( common::to_lower(std::as_const(key)) == common::to_lower(std::as_const(f_name))) {
					arg = value(f_args);
					break;
				}
			}

			if ( std::holds_alternative<std::string>(arg)) {
				tok.value = std::get<std::string>(arg);
				tok.type = T_STRING;
			} else if ( std::holds_alternative<double>(arg)) {
				tok.value = std::get<double>(arg);
				tok.type = T_NUMBER;
			} /* else if ( std::holds_alternative<std::nullptr_t>(arg)) {
				tokens[i] = tok;
				return tokens;
			} */ // tok already is replacing function token and defaults to T_UNDEF

			tokens[i] = tok;
			return tokens;

		} else {
			logger::warning << logger::tag("evaluator") << "ignored unknown function " << common::to_lower(std::as_const(tokens[i].name)) << std::endl;
			tokens[i] = tok;
			return tokens;
		}
	}

	return tokens;
}

static std::vector<TOKEN> eval_variables(std::vector<TOKEN>& tokens, const FUNCTIONS *functions, VARIABLES *variables) {

	// evaluate variables
	for ( size_t i = 0; i < tokens.size(); i++ ) {

		if ( tokens[i].type != T_VARIABLE ) continue;

		TOKEN token = tokenize_variable_value(tokens[i].name, variables);
		tokens[i] = token;
	}

	return tokens;
}

static std::vector<TOKEN> eval_parentheses(std::vector<TOKEN>& tokens, const FUNCTIONS *functions, VARIABLES *variables) {

	// evaluate parentheses
	for ( size_t i = 0; i < tokens.size(); i++ ) {

		begin_evaluate:

		if ( tokens[i].type == T_SUB && tokens[i].child.size() > 1 ) {

			try {
				tokens[i].child = eval(tokens[i].child, false, functions, variables);
			} catch ( std::runtime_error& e ) {
				throw e;
			}
			goto begin_evaluate;
		}

		if ( tokens[i].type == T_SUB && tokens[i].child.size() == 1 ) {
			if ( tokens[i].child[0].type == T_UNDEF ) {
				std::cout << "evaluate: error while parsing parentheses, result came back as null" << std::endl;
				tokens.erase(i == 0 ? tokens.begin() : ( tokens.begin() + i ));
			} else tokens[i] = tokens[i].child[0];
			return tokens;
		} else if ( tokens[i].type == T_SUB && tokens[i].child.size() == 0 ) {
			std::cout << "evaluate: cannot evaluate value inside parentheses, is it is considered as empty" << std::endl;
			tokens.erase(i == 0 ? tokens.begin() : ( tokens.begin() + i ));
			return tokens;
		}
	}

	return tokens;
}

static std::vector<TOKEN> eval(std::vector<TOKEN>& tokens, bool f_args, const FUNCTIONS *functions, VARIABLES *variables) {

	TOKEN token;

	try {
		tokens = eval_variables(tokens, functions, variables);
	} catch ( std::runtime_error& e ) {
		throw e;
	}

	try {
		tokens = eval_functions(tokens, functions, variables);
	} catch ( std::runtime_error& e ) {
		throw e;
	}

	try {
		tokens = eval_parentheses(tokens, functions, variables);
	} catch ( std::runtime_error& e ) {
		throw e;
	}

	// evaluate ops

	if ( tokens[0].type == T_OPERATOR ) {
		// TODO: change number races from negative to positive etc..

		std::cout << "evaluate: error, left side value missing from expression" << std::endl;
		tokens.erase(tokens.begin());
		return tokens;
	}

	if ( tokens.size() > 0 && tokens[1].type == T_OPERATOR ) {

		switch ( tokens[1].op ) {

			case OP_ADD:
				if ( tokens.size() > 2 ) {

					tokens[2] = TOKEN::ADD(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					std::cout << "evaluate: error, single operator with missng left and right side values" << std::endl;
					tokens[0] = TOKEN::UNDEF();
				}

				return tokens;

			case OP_SUB:

				if ( tokens.size() > 2 ) {
					tokens[2] = TOKEN::SUB(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					std::cout << "evaluate: error, single operator with missng left and right side values" << std::endl;
					tokens[0] = TOKEN::UNDEF();
				}

				return tokens;

			default:
				std::cout << "evaluate: error, unhandled undefined operator " << describe(tokens[1].op) << std::endl;
		}
	}

	// todo: is this useful?
	for ( size_t i = 0; i < tokens.size(); i++ ) {

		switch ( tokens[i].type ) {

			case T_NUMBER:
			case T_STRING:
				continue;
			default:
				std::cout << "evaluate: error, unknown value type" << std::endl;
		}
	}

	return tokens;
}

TOKEN evaluate(std::vector<TOKEN>& tokens, const FUNCTIONS *functions, VARIABLES *variables) {

	TOKEN result;
	bool abort = false;

	begin_evaluate:

	while ( tokens.size() > 1 ) {
		try {
			tokens = eval(tokens, false, functions, variables);
		} catch ( std::runtime_error& e ) {
			std::cout << "evaluate: error: " << e.what() << std::endl;
			abort = true;
		}
	}

	if ( !abort && tokens.front().type == T_VARIABLE ) {
		try {
			tokens = eval(tokens, false, functions, variables);
		} catch ( std::runtime_error& e ) {
			std::cout << "evaluate: error: " << e.what() << std::endl;
			abort = true;
		}
		if ( !abort ) goto begin_evaluate;
	}

	if ( !abort && tokens.front().type == T_FUNCTION ) {
		try {
			tokens = eval(tokens, false, functions, variables);
		} catch ( std::runtime_error& e ) {
			std::cout << "evaluate: error: " << e.what() << std::endl;
			abort = true;
		}
		if ( !abort ) goto begin_evaluate;
	}

	if ( !abort && tokens.front().type == T_SUB && !tokens.front().child.empty()) {
		tokens = tokens.front().child;
		goto begin_evaluate;
	}

	return !abort && tokens.size() == 1 ? tokens.front() : result;
}

TOKEN evaluate(const std::string& s, const FUNCTIONS *functions, VARIABLES *variables) {

	TOKEN result;

	std::vector<TOKEN> tokens = parse(s);

	try {
		result = evaluate(tokens, functions);
	} catch ( std::runtime_error& e ) {
		throw e;
	}

	return result;
}

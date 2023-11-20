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
				logger::error << logger::tag("evaluate") << "error while evaluating parentheses, result was null" << std::endl;
				tokens.erase(i == 0 ? tokens.begin() : ( tokens.begin() + i ));
			} else tokens[i] = tokens[i].child[0];
			return tokens;
		} else if ( tokens[i].type == T_SUB && tokens[i].child.size() == 0 ) {
			logger::error << logger::tag("evaluate") << "cannot evaluate value inside parentheses, it is considered as null" << std::endl;
			tokens.erase(i == 0 ? tokens.begin() : ( tokens.begin() + i ));
			return tokens;
		}
	}

	return tokens;
}

static std::vector<TOKEN> eval_conditionals(std::vector<TOKEN>&tokens, const FUNCTIONS *functions, VARIABLES *variables) {

	// pre-process conditionals
	for ( size_t i = 0; i < tokens.size(); i++ ) {

		if ( tokens[i].type != T_CONDITIONAL )
			continue;

		begin_evaluate:

		if ( tokens[i].cond1.size() > 1 ) {

			try {
				tokens[i].cond1 = eval(tokens[i].cond1, false, functions, variables);
			} catch ( std::runtime_error& e ) {
				throw e;
			}
			goto begin_evaluate;
		}

		if ( tokens[i].cond1.size() == 1 && tokens[i].cond1[0].type == T_UNDEF )
			logger::verbose << logger::tag("evaluate") << "warning, while pre-processing conditional, true result is undefined result" << std::endl;
		else if ( tokens[i].cond1.size() == 0 ) {
			logger::error << logger::tag("evaluate") << "conditional true result evaluated to null" << std::endl;
			tokens[i].cond1.push_back(TOKEN::UNDEF());
		}

		begin_evaluate2:

		if ( tokens[i].cond2.size() > 1 ) {

			try {
				tokens[i].cond2 = eval(tokens[i].cond2, false, functions, variables);
			} catch ( std::runtime_error& e ) {
				throw e;
			}
			goto begin_evaluate2;
		}

		if ( tokens[i].cond2.size() == 1 && tokens[i].cond2[0].type == T_UNDEF )
			logger::verbose << logger::tag("evaluate") << "warning, while pre-processing conditional, false result is undefined result" << std::endl;
		else if ( tokens[i].cond2.size() == 0 ) {
			logger::error << logger::tag("evaluate") << "conditional false result evaluated to null" << std::endl;
			tokens[i].cond1.push_back(TOKEN::UNDEF());
		}

		return tokens;
	}

	return tokens;
}

static void process_rhs_token(std::vector<TOKEN>& tokens, size_t index1, size_t index2) {

	if ( tokens.size() > index2 && tokens[index1].type == T_OPERATOR && tokens[index1].op == OP_NOT ) {

		tokens[1] = TOKEN::NOT(tokens[1].numberValue());
		tokens.erase(tokens.begin() + index1);

		return;
	}

	bool r_negate = false;

	// handle multiple negative signs in row..
	if ( tokens.size() > index2 && tokens[index1].type == T_OPERATOR && tokens[index2].type == T_OPERATOR &&
		tokens[index1].op == OP_SUB && tokens[index2].op == OP_SUB ) {

		while ( tokens.size() > index2 && tokens[index2].type == T_OPERATOR && (
			( tokens[index1].op == OP_SUB && tokens[index2].op == OP_SUB )
		)) {
			if ( tokens[index2].op == OP_SUB ) r_negate = !r_negate;
			tokens.erase(tokens.begin() + index1);
		}

	}

	if ( tokens.size() > index2 &&
		tokens[index1].type == T_OPERATOR && tokens[index1].op == OP_SUB &&
		( tokens[index2].type == T_NUMBER || tokens[index2].type == T_STRING )) {

		tokens[index2] = TOKEN::SGN(r_negate ? -(tokens[index2].numberValue()) : tokens[index2].numberValue());
		tokens.erase(tokens.begin() + index1);
	}
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

	try {
		tokens = eval_conditionals(tokens, functions, variables);
	} catch ( std::runtime_error& e ) {
		throw e;
	}

	// evaluate base ops

	if ( tokens[0].type == T_OPERATOR ) {

		if ( tokens[0].op == OP_SUB || tokens[0].op == OP_NOT ) { // O_SGN
			if ( tokens.size() < 2 ) {
				// logger::debug << logger::tag("evaluate") << "SGN operator with tokens array's size: " << tokens.size() << std::endl;
				logger::warning << logger::tag("evaluate") <<
					"single sign operator(" << ( tokens[0].op == OP_SUB ? "-" : "!" ) <<
					") without left and right side value always returns -0" << std::endl;
				tokens[0] = TOKEN::SGN(double(tokens[0].op == OP_SUB ? 0 : -0));
				return tokens;
			}
			process_rhs_token(tokens, 0, 1);
			return tokens;
		}

		logger::error << logger::tag("evaluate") << "left side value missing from expression" << std::endl;
		tokens.erase(tokens.begin());
		return tokens;
	}

	/* evaluate conditions */

	if ( tokens[0].type == T_CONDITIONAL ) {

		logger::error << logger::tag("evaluate") << "conditional expression without condition, ignoring" << std::endl;
		tokens.erase(tokens.begin());
		return tokens;
	}

	if ( tokens.size() > 1 && tokens[1].type == T_CONDITIONAL ) {

		if ( tokens[0].numberValue() == 0 )
			tokens[0] = tokens[1].cond2[0];
		else
			tokens[0] = tokens[1].cond1[0];

		tokens.erase(tokens.begin() + 1);
		return tokens;
	}

	/* evaluate rest of ops */

	if ( tokens.size() > 1 && tokens[1].type == T_OPERATOR ) {

		switch ( tokens[1].op ) {

			/* basic math */
			case OP_ADD:
				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isString() && tokens[1].isString())
						tokens[2] = TOKEN::CAT(tokens[0].stringValue(), tokens[2].stringValue());
					else
						tokens[2] = TOKEN::ADD(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator ADD(+) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SUB:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = TOKEN::SUB(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator SUB(-) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_CAT:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = TOKEN::CAT(tokens[0].stringValue(), tokens[2].stringValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator CAT(.) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_MUL:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = TOKEN::MUL(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator MUL(*) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_DIV:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = TOKEN::DIV(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator DIV(/) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_MOD:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = TOKEN::MOD(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator MOD(%) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_POW:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = TOKEN::POW(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator POW(^)with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			/* logical operators */
			case OP_OR:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = TOKEN::OR(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator OR(|) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_AND:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = TOKEN::AND(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator AND(&) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			// case OP_NOT: /* Operaotr NOT is handled in process_rhs_tokens, we should never be here */

			/* number comparators */
			case OP_NEQ:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);

					if ( tokens[0].isString() && tokens[1].isString())
						tokens[2] = TOKEN::SEQ(tokens[0].stringValue(), tokens[2].stringValue());
					else
						tokens[2] = TOKEN::NEQ(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator NEQ(==) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_NNE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isString() && tokens[1].isString())
						tokens[2] = TOKEN::SNE(tokens[0].stringValue(), tokens[2].stringValue());
					else
						tokens[2] = TOKEN::NNE(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator NNE(!=) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_NLT:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isString() && tokens[1].isString())
						tokens[2] = TOKEN::SLT(tokens[0].stringValue(), tokens[2].stringValue());
					else
						tokens[2] = TOKEN::NLT(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator NLT(<) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_NLE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isString() && tokens[1].isString())
						tokens[2] = TOKEN::SLE(tokens[0].stringValue(), tokens[2].stringValue());
					else
						tokens[2] = TOKEN::NLE(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator NLE(<=) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_NGT:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isString() && tokens[1].isString())
						tokens[2] = TOKEN::SGT(tokens[0].stringValue(), tokens[2].stringValue());
					else
						tokens[2] = TOKEN::NGT(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator NGT(>) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_NGE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isString() && tokens[1].isString())
						tokens[2] = TOKEN::SGE(tokens[0].stringValue(), tokens[2].stringValue());
					else
						tokens[2] = TOKEN::NGE(tokens[0].numberValue(), tokens[2].numberValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator NGE(>=) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			/* string comparators */
			case OP_SEQ:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isNumber() && tokens[1].isNumber())
						tokens[2] = TOKEN::NEQ(tokens[0].numberValue(), tokens[2].numberValue());
					else
						tokens[2] = TOKEN::SEQ(tokens[0].stringValue(), tokens[2].stringValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator SEQ(eq) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SNE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isNumber() && tokens[1].isNumber())
						tokens[2] = TOKEN::NNE(tokens[0].numberValue(), tokens[2].numberValue());
					else
						tokens[2] = TOKEN::SNE(tokens[0].stringValue(), tokens[2].stringValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator SNE(ne) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SLT:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isNumber() && tokens[1].isNumber())
						tokens[2] = TOKEN::NLT(tokens[0].numberValue(), tokens[2].numberValue());
					else
						tokens[2] = TOKEN::SLT(tokens[0].stringValue(), tokens[2].stringValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator SLT(lt) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SLE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isNumber() && tokens[1].isNumber())
						tokens[2] = TOKEN::NLE(tokens[0].numberValue(), tokens[2].numberValue());
					else
						tokens[2] = TOKEN::SLE(tokens[0].stringValue(), tokens[2].stringValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator SLE(le) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SGT:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isNumber() && tokens[1].isNumber())
						tokens[2] = TOKEN::NGT(tokens[0].numberValue(), tokens[2].numberValue());
					else
						tokens[2] = TOKEN::SGT(tokens[0].stringValue(), tokens[2].stringValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator SGT(gt) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SGE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].isNumber() && tokens[1].isNumber())
						tokens[2] = TOKEN::NGE(tokens[0].numberValue(), tokens[2].numberValue());
					else
						tokens[2] = TOKEN::SGE(tokens[0].stringValue(), tokens[2].stringValue());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error << logger::tag("evaluate") << "operator SGE(>=) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;


			default:
				logger::error << logger::tag("evaluate") << "unhandled unknown operator " << describe(tokens[1].op) << std::endl;
				tokens.erase(tokens.begin());
		}

		return tokens;
	}

	// todo: is this useful?
	for ( size_t i = 0; i < tokens.size(); i++ ) {

		switch ( tokens[i].type ) {

			case T_NUMBER:
				logger::debug << "evaluated token is number(" << tokens[i].numberValue() << ")" << std::endl;
				continue;
			case T_STRING:
				logger::debug << "evaluated token is string(" << tokens[i].stringValue() << ")" << std::endl;
				continue;
			default:
				logger::error << logger::tag("evaluate") << "unknown value type (" << describe(tokens[i].type) << ")" << std::endl;
		}
	}

	return tokens;
}

TOKEN evaluate(std::vector<TOKEN>& tokens, const FUNCTIONS *functions, VARIABLES *variables) {

	bool abort = false;

	begin_evaluate:

	//logger::debug << logger::tag("evaluate") << "evaluating: " << describe(tokens) << std::endl;

	while ( tokens.size() > 1 ) {
		try {
			tokens = eval(tokens, false, functions, variables);
		} catch ( std::runtime_error& e ) {
			logger::error << logger::tag("evaluate") << e.what() << std::endl;
			abort = true;
		}
	}

	if ( !abort && tokens.front().type == T_VARIABLE ) {
		try {
			tokens = eval(tokens, false, functions, variables);
		} catch ( std::runtime_error& e ) {
			logger::error << logger::tag("evaluate") << e.what() << std::endl;
			abort = true;
		}
		if ( !abort ) goto begin_evaluate;
	}

	if ( !abort && tokens.front().type == T_FUNCTION ) {
		try {
			tokens = eval(tokens, false, functions, variables);
		} catch ( std::runtime_error& e ) {
			logger::error << logger::tag("evaluate") << e.what() << std::endl;
			abort = true;
		}
		if ( !abort ) goto begin_evaluate;
	}

	if ( !abort && tokens.front().type == T_SUB && !tokens.front().child.empty()) {
		tokens = tokens.front().child;
		goto begin_evaluate;
	}

	if ( !abort && tokens.front().type == T_OPERATOR && tokens.front().op == OP_SUB ) {
		if ( tokens.size() > 1 ) goto begin_evaluate;
		try {
			tokens = eval(tokens, false, functions, variables);
		} catch ( std::runtime_error&e ) {
			abort = true;
			throw e;
		}
	}

	if ( !abort && tokens.size() == 1 )
		return tokens.front();
	else {
		if ( abort )
			logger::warning << logger::tag("evaluate") << "evaluation was aborted because of errors" << std::endl;
		else
			logger::warning << logger::tag("evaluate") << "evaluating ended up with ambiguous results, result will be null" << std::endl;
		return TOKEN::UNDEF();
	}
}

TOKEN evaluate(const std::string& s, const FUNCTIONS *functions, VARIABLES *variables) {

	TOKEN result;

	std::vector<TOKEN> tokens = parse(s);

	try {
		result = evaluate(tokens, functions);
	} catch ( std::runtime_error& e ) {
		logger::error << logger::tag("evaluate") << e.what() << std::endl;
		throw e;
	}

	return result;
}

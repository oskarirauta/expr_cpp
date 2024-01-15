#include <stdexcept>
#include <utility>
#include "common.hpp"
#include "logger.hpp"
#include "expr/expression.hpp"

std::vector<std::vector<expr::TOKEN>> expr::expression::get_arg_tokens(const std::vector<expr::TOKEN>& tokens) {

	std::vector<std::vector<expr::TOKEN>> args;
	std::vector<expr::TOKEN> arg;

	for ( const expr::TOKEN& token : tokens ) {

		if ( token == OP_COM ) {
			args.push_back(arg);
			arg.clear();
		} else arg.push_back(token);
	}

	if ( !arg.empty())
		args.push_back(arg);

	return args;
}

const expr::VARIABLE expr::expression::get_variable_value(const std::string& name, expr::VARIABLEMAP *variables) {

	if ( variables != nullptr && !name.empty() && !variables -> empty() && variables -> contains(name))
		return (*variables)[name];

	return (VARIABLE)nullptr;
}

expr::TOKEN expr::expression::tokenize_variable_value(const std::string& name, expr::VARIABLEMAP *variables) {

	expr::TOKEN tok;

	if ( variables != nullptr && !name.empty() && !variables -> empty() && variables -> contains(name)) {

		expr::VARIABLE v = get_variable_value(name, variables);

		if ( std::holds_alternative<std::string>(v))
			tok = std::get<std::string>(v);
		else if ( std::holds_alternative<double>(v))
			tok = std::get<double>(v);
	} else if ( !name.empty() && common::to_lower(std::as_const(name)) == "true" ) {
		tok = (double)1;
	} else if ( !name.empty() && common::to_lower(std::as_const(name)) == "false" ) {
		tok = (double)0;
	} else if ( !name.empty() && common::to_lower(std::as_const(name)) == "pi" ) {
		tok = (double)M_PI;
	} else if ( !name.empty() && common::to_lower(std::as_const(name)) == "pi_2" ) {
		tok = (double)M_PI_2;
	} else if ( !name.empty() && common::to_lower(std::as_const(name)) == "pi_4" ) {
		tok = (double)M_PI_4;
	} else if ( !name.empty() && common::to_lower(std::as_const(name)) == "e" ) {
		tok = (double)M_E;
	}

	if ( tok == T_UNDEF ) tok = TOKEN::STRING("");

	return tok;
}

std::vector<expr::TOKEN> expr::expression::eval_functions(
	std::vector<expr::TOKEN>& tokens, expr::FUNCTIONMAP *functions, expr::VARIABLEMAP *variables) {

	// evaluate functions first
	for ( size_t i = 0; i < tokens.size(); i++ ) {

		TOKEN tok;

		if ( tokens[i] != expr::T_FUNCTION ) continue;

		if ( tokens[i]._name.empty()) {
			tokens[i] = tok; // T_UNDEF
			return tokens;
		}

		if ( functions -> contains(tokens[i]._name)) {

			std::vector<std::vector<expr::TOKEN>> args = get_arg_tokens(tokens[i]._args);
			FUNCTION_ARGS f_args;
			bool abort = false;

			for ( size_t a = 0; a < args.size(); a++ ) {

				eval_f_arg:

				if ( abort ) {
					abort = false;
					continue;
				}

				while ( !abort && args[a].size() > 1 ) {

					try {
						args[a] = eval(args[a], false, functions, variables);
					} catch ( std::runtime_error& e ) {
						logger::error["evaluate"] << "evaluation error for <" <<
							describe(args[a]) << ">: " << e.what() << std::endl;
						logger::warning["evaluate"] << "replacing with null" << std::endl;
						args[a].clear();
						args[a].push_back(expr::TOKEN::UNDEF());
						abort = false; // note: this is intentional, we just replaced it
					}

					goto eval_f_arg;
				}

				if ( !abort && args[a].size() == 1 && args[a].front() == expr::T_VARIABLE ) {

					try {
						args[a] = eval(args[a], false, functions, variables);
					} catch ( std::runtime_error& e ) {
						logger::error["evaluate"] << "evaluation error with variable <" <<
							describe(args[a]) << ">: " << e.what() << std::endl;
						abort = true;
					}

					goto eval_f_arg;
				}

				if ( !abort && args[a].size() == 1 && args[a].front() == expr::T_FUNCTION ) {

					try {
						args[a] = eval(args[a], false, functions, variables);
					} catch ( std::runtime_error& e ) {
						logger::error["evaluate"] << "evaluation error with function <" <<
							describe(args[a]) << ">: " << e.what() << std::endl;
						abort = true;
					}

					goto eval_f_arg;
				}

				if ( !abort && args[a].size() == 1 && args[a].front() == expr::T_SUB && !args[a].front()._child.empty()) {
					args[a] = args[a].front()._child;
					goto eval_f_arg;
				}

				if ( args[a].size() == 1 ) {

					VARIABLE arg;

					if ( args[a][0].is_number())
						arg = args[a][0].to_double();
					else if ( args[a][0].is_string())
						arg = args[a][0].to_string();
					else arg = nullptr;

					f_args.push_back(arg);

				} else f_args.push_back(nullptr);

				if ( abort )
					logger::warning["evaluate"] << "evaluation problem, " <<
						"abort caused by error in expression" << std::endl;
			}

			VARIABLE arg = nullptr;

			if ( functions -> contains(tokens[i]._name)) {

				arg = (*functions)[tokens[i]._name](f_args);

				if ( std::holds_alternative<std::string>(arg))
					tok = std::get<std::string>(arg);
				else if ( std::holds_alternative<double>(arg))
					tok = std::get<double>(arg);
			} else tok = "";

			tokens[i] = tok;
			return tokens;

		} else {

			logger::warning["evaluate"] << "ignored unknown function " <<
				common::to_lower(std::as_const(tokens[i]._name)) << std::endl;
			tokens[i] = tok;
			return tokens;
		}
	}

	return tokens;
}

std::vector<expr::TOKEN> expr::expression::eval_variables(
	std::vector<expr::TOKEN>& tokens, expr::FUNCTIONMAP *functions, expr::VARIABLEMAP *variables) {

	// evaluate variables
	for ( size_t i = 0; i < tokens.size(); i++ ) {

		if ( tokens[i] != expr::T_VARIABLE ) continue;

		expr::TOKEN token = tokenize_variable_value(tokens[i]._name, variables);
		tokens[i] = token;
	}

	return tokens;
}

std::vector<expr::TOKEN> expr::expression::eval_parentheses(std::vector<expr::TOKEN>& tokens,
	expr::FUNCTIONMAP *functions, expr::VARIABLEMAP *variables) {

	// evaluate parentheses
	for ( size_t i = 0; i < tokens.size(); i++ ) {

		begin_evaluate:

		if ( tokens[i] == expr::T_SUB && tokens[i]._child.size() > 1 ) {

			try {
				tokens[i]._child = eval(tokens[i]._child, false, functions, variables);
			} catch ( std::runtime_error& e ) {
				logger::error["evaluate"] <<
					"evaluation error inside parentheses <" <<
					describe(tokens[i]._child) << ">: " << e.what() << std::endl;
				logger::warning["evaluate"] << "replacing parentheses with null" << std::endl;

				tokens[i]._child.clear();
				tokens[i]._child.push_back(expr::TOKEN::UNDEF());
			}

			goto begin_evaluate;
		}

		if ( tokens[i] == expr::T_SUB && tokens[i]._child.size() == 1 ) {
			if ( tokens[i]._child[0] == expr::T_UNDEF ) {
				logger::error["evaluate"] << "error while evaluating parentheses, result was null" << std::endl;
				tokens.erase(i == 0 ? tokens.begin() : ( tokens.begin() + i ));
			} else tokens[i] = tokens[i]._child[0];
			return tokens;
		} else if ( tokens[i] == expr::T_SUB && tokens[i]._child.size() == 0 ) {
			logger::error["evaluate"] << "cannot evaluate value inside parentheses, it is considered as null" << std::endl;
			tokens.erase(i == 0 ? tokens.begin() : ( tokens.begin() + i ));
			return tokens;
		}
	}

	return tokens;
}

std::vector<expr::TOKEN> expr::expression::eval_conditionals(
	std::vector<expr::TOKEN>&tokens, expr::FUNCTIONMAP *functions, expr::VARIABLEMAP *variables) {

	// pre-process conditionals
	for ( size_t i = 0; i < tokens.size(); i++ ) {

		if ( tokens[i] != expr::T_CONDITIONAL )
			continue;

		begin_evaluate:

		if ( tokens[i]._cond1.size() > 1 ) {

			try {
				tokens[i]._cond1 = eval(tokens[i]._cond1, false, functions, variables);
			} catch ( std::runtime_error& e ) {

				logger::error["evaluate"] <<
					"evaluation error inside condition's true result <" <<
					describe(tokens[i]._cond1) << ">: " << e.what() << std::endl;
				logger::warning["evaluate"] << "replacing true condition with null" << std::endl;

				tokens[i]._cond1.clear();
				tokens[i]._cond1.push_back(expr::TOKEN::UNDEF());
			}

			goto begin_evaluate;
		}

		if ( tokens[i]._cond1.size() == 1 && tokens[i]._cond1[0] == expr::T_UNDEF )
			logger::verbose["evaluate"] << "warning, while pre-processing conditional, true result is undefined result" << std::endl;
		else if ( tokens[i]._cond1.size() == 0 ) {
			logger::error["evaluate"] << "conditional true result evaluated to null" << std::endl;
			tokens[i]._cond1.push_back(TOKEN::UNDEF());
		}

		begin_evaluate2:

		if ( tokens[i]._cond2.size() > 1 ) {

			try {
				tokens[i]._cond2 = eval(tokens[i]._cond2, false, functions, variables);
			} catch ( std::runtime_error& e ) {
				logger::error["evaluate"] <<
					"evaluation error inside condition's false result <" <<
					describe(tokens[i]._cond2) << ">: " << e.what() << std::endl;
				logger::warning["evaluate"] << "replacing false condition with null" << std::endl;

				tokens[i]._cond2.clear();
				tokens[i]._cond2.push_back(expr::TOKEN::UNDEF());
			}
			goto begin_evaluate2;
		}

		if ( tokens[i]._cond2.size() == 1 && tokens[i]._cond2[0] == expr::T_UNDEF )
			logger::verbose["evaluate"] << "warning, while pre-processing conditional, false result is undefined result" << std::endl;
		else if ( tokens[i]._cond2.size() == 0 ) {
			logger::error["evaluate"] << "conditional false result evaluated to null" << std::endl;
			tokens[i]._cond1.push_back(expr::TOKEN::UNDEF());
		}

		return tokens;
	}

	return tokens;
}

void expr::expression::process_rhs_token(std::vector<expr::TOKEN>& tokens, size_t index1, size_t index2) {

	if ( tokens.size() > index2 &&
		( tokens[index1] == expr::OP_NOT || tokens[index1] == expr::OP_NNOT ) &&
		( tokens[index2] == expr::T_NUMBER || tokens[index2] == expr::T_STRING )) {

		if ( tokens[index1] == expr::OP_NOT )
			tokens[index2] = expr::TOKEN::NOT(tokens[index2].to_double());
		else tokens[index2] = expr::TOKEN::NNOT(tokens[index2].to_double());

		tokens.erase(tokens.begin() + index1);

		return;
	}

	bool r_negate = false;

	// handle multiple negative signs in row..
	if ( tokens.size() > index2 && tokens[index1] == expr::OP_SUB && tokens[index2] == expr::OP_SUB ) {

		while ( tokens.size() > index2 && tokens[index1] == expr::OP_SUB && tokens[index2] == expr::OP_SUB ) {
			if ( tokens[index2] == expr::OP_SUB ) r_negate = !r_negate;
			tokens.erase(tokens.begin() + index1);
		}

	}

	if ( tokens.size() > index2 && tokens[index1] == expr::OP_SUB &&
		( tokens[index2] == expr::T_NUMBER || tokens[index2] == expr::T_STRING )) {

		tokens[index2] = expr::TOKEN::SGN(r_negate ? -(tokens[index2].to_double()) : tokens[index2].to_double());
		tokens.erase(tokens.begin() + index1);
	}
}

std::vector<expr::TOKEN> expr::expression::eval(
	std::vector<expr::TOKEN>& tokens, bool f_args, expr::FUNCTIONMAP *functions, expr::VARIABLEMAP *variables) {

	expr::TOKEN token;

	tokens = eval_variables(tokens, functions, variables);
	tokens = eval_functions(tokens, functions, variables);
	tokens = eval_parentheses(tokens, functions, variables);
	tokens = eval_conditionals(tokens, functions, variables);

	// evaluate base ops

	if ( tokens[0] == expr::T_OPERATOR ) {

		if ( tokens[0] == expr::OP_SUB || tokens[0] == expr::OP_NOT || tokens[0] == expr::OP_NNOT ) { // O_SGN
			if ( tokens.size() < 2 ) {
				logger::warning["evaluate"] <<
					"single operator(" << describe(tokens[0]._op) << ") without " <<
					"left and right side value always returns 0" << std::endl;
				tokens[0] = expr::TOKEN::NUMBER(0);
				return tokens;
			}

			process_rhs_token(tokens, 0, 1);

			if ( tokens.size() < 2 || ( tokens[0] != expr::T_OPERATOR && tokens[1] != expr::T_OPERATOR ))
				return tokens;
		}

		if ( !( tokens.size() > 1 && tokens[1] == expr::OP_SUB &&
			( tokens[0] == expr::OP_NOT || tokens[0] == expr::OP_NNOT ))) {

			logger::error["evaluate"] << "left side value missing from expression" << std::endl;
			tokens.erase(tokens.begin());
			return tokens;
		}
	}

	/* evaluate conditions */

	if ( tokens[0] == expr::T_CONDITIONAL ) {

		logger::error["evaluate"] << "conditional expression without condition, ignoring" << std::endl;
		tokens.erase(tokens.begin());
		return tokens;
	}

	if ( tokens.size() > 1 && tokens[1] == expr::T_CONDITIONAL ) {

		if ( tokens[0].to_double() == 0 )
			tokens[0] = tokens[1]._cond2[0];
		else
			tokens[0] = tokens[1]._cond1[0];

		tokens.erase(tokens.begin() + 1);
		return tokens;
	}

	/* evaluate rest of ops */

	if ( tokens.size() > 1 && tokens[1] == expr::T_OPERATOR ) {

		switch ( tokens[1]._op ) {

			/* basic math */
			case expr::OP_ADD:
				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].is_string() && tokens[2].is_string())
						tokens[2] = expr::TOKEN::CAT(tokens[0].to_string(), tokens[2].to_string());
					else if ( tokens[0].is_number() && tokens[2].is_string())
						tokens[2] = expr::TOKEN::CAT(tokens[0].to_string(), tokens[2].to_string());
					else
						tokens[2] = expr::TOKEN::ADD(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator ADD(+) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SUB:

				if ( tokens.size() > 2 ) {

					if ( tokens[0] == expr::OP_NOT || tokens[0] == expr::OP_NNOT ) {

						if ( tokens[2] == expr::T_NUMBER || tokens[2] == expr::T_STRING ) {

							tokens[2] = expr::TOKEN::NUMBER(-tokens[2].to_double());
							tokens.erase(tokens.begin() + 1);
						}

						break;
					}

					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::SUB(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator SUB(-) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_CAT:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::CAT(tokens[0].to_string(), tokens[2].to_string());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator CAT(.) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_MUL:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::MUL(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator MUL(*) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_DIV:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::DIV(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator DIV(/) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_MOD:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::MOD(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator MOD(%) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_POW:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::POW(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator POW(^)with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			/* logical operators */
			case OP_OR:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::OR(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator OR(|) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_AND:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::AND(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator AND(&) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_NOT: /* Operatos NOT and NNOT are handled in */
				break;
			case OP_NNOT: /* process_rhs_token, we should never be here */
				break;

			/* number comparators */
			case OP_NEQ:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].is_string() || tokens[2].is_string())
						tokens[2] = expr::TOKEN::SEQ(tokens[0].to_string(), tokens[2].to_string());
					else
						tokens[2] = expr::TOKEN::NEQ(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator NEQ(==) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_NNE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].is_string() || tokens[2].is_string())
						tokens[2] = expr::TOKEN::SNE(tokens[0].to_string(), tokens[2].to_string());
					else
						tokens[2] = expr::TOKEN::NNE(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator NNE(!=) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_NLT:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].is_string() || tokens[2].is_string())
						tokens[2] = expr::TOKEN::SLT(tokens[0].to_string(), tokens[2].to_string());
					else
						tokens[2] = expr::TOKEN::NLT(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator NLT(<) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_NLE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].is_string() || tokens[2].is_string())
						tokens[2] = expr::TOKEN::SLE(tokens[0].to_string(), tokens[2].to_string());
					else
						tokens[2] = expr::TOKEN::NLE(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator NLE(<=) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_NGT:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].is_string() || tokens[2].is_string())
						tokens[2] = TOKEN::SGT(tokens[0].to_string(), tokens[2].to_string());
					else
						tokens[2] = TOKEN::NGT(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator NGT(>) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_NGE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					if ( tokens[0].is_string() || tokens[2].is_string())
						tokens[2] = TOKEN::SGE(tokens[0].to_string(), tokens[2].to_string());
					else
						tokens[2] = TOKEN::NGE(tokens[0].to_double(), tokens[2].to_double());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator NGE(>=) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			/* string comparators */
			case OP_SEQ:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::SEQ(tokens[0].to_string(), tokens[2].to_string());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator SEQ(eq) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SNE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::SNE(tokens[0].to_string(), tokens[2].to_string());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator SNE(ne) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SLT:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::SLT(tokens[0].to_string(), tokens[2].to_string());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator SLT(lt) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SLE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::SLE(tokens[0].to_string(), tokens[2].to_string());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator SLE(le) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SGT:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::SGT(tokens[0].to_string(), tokens[2].to_string());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator SGT(gt) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;

			case OP_SGE:

				if ( tokens.size() > 2 ) {
					process_rhs_token(tokens, 2, 3);
					tokens[2] = expr::TOKEN::SGE(tokens[0].to_string(), tokens[2].to_string());
					tokens.erase(tokens.begin(), tokens.begin() + 2);
				} else {
					logger::error["evaluate"] << "operator SGE(>=) with missing right side value" << std::endl;
					tokens.erase(tokens.begin() + 1);
				}
				break;


			default:
				logger::error["evaluate"] << "unhandled unknown operator " << describe(tokens[1]._op) << std::endl;
				tokens.erase(tokens.begin());
		}

		return tokens;

	} else if ( tokens.size() > 1 && tokens[0] == T_NUMBER && tokens[1] == T_STRING ) {

		if ( !tokens[1].to_string().empty())
			tokens[0] = expr::TOKEN::STRING(tokens[0].to_string() + tokens[1].to_string());
		tokens.erase(tokens.begin() + 1);

	} else if ( tokens.size() > 1 && tokens[0] == T_STRING && tokens[1] == T_NUMBER ) {

		if ( !tokens[0].to_string().empty())
			tokens[1] = expr::TOKEN::STRING(tokens[0].to_string() + tokens[1].to_string());
		tokens.erase(tokens.begin());

	} else if ( tokens.size() > 1 && tokens[0] == T_NUMBER && tokens[1] == T_NUMBER ) {

		tokens[0] = expr::TOKEN::NUMBER(tokens[0].to_double() + tokens[1].to_double());
		tokens.erase(tokens.begin() + 1);

	} else if ( tokens.size() > 1 && tokens[0] == T_STRING && tokens[1] == T_STRING ) {

		tokens[0] = expr::TOKEN::STRING(tokens[0].to_string() + tokens[1].to_string());
		tokens.erase(tokens.begin() + 1);

	} else if ( tokens.size() > 1 && tokens[0] == T_UNDEF ) {

		tokens.erase(tokens.begin());

	} else if ( tokens.size() > 1 && tokens[1] == T_UNDEF ) {

		tokens.erase(tokens.begin() + 1);

	}

	return tokens;
}

expr::TOKEN expr::expression::evaluate(std::vector<expr::TOKEN>& tokens, expr::FUNCTIONMAP *functions, expr::VARIABLEMAP *variables) {

	bool abort = false;
	std::string set_variable;

	if ( tokens.empty())
		return expr::TOKEN::UNDEF();

	if ( tokens.size() > 1 && tokens[1] == expr::OP_SET &&
		tokens[0] == expr::T_VARIABLE && !tokens[0]._name.empty()) {

		set_variable = tokens[0]._name;
		tokens.erase(tokens.begin(), tokens.begin() + 2);
	}

	begin_evaluate:

	while ( tokens.size() > 1 ) {
		try {
			tokens = eval(tokens, false, functions, variables);
		} catch ( std::runtime_error& e ) {
			logger::error["evaluate"] << e.what() << std::endl;
			abort = true;
		}
	}

	if ( !abort && tokens.front() == expr::T_VARIABLE ) {
		try {
			tokens = eval(tokens, false, functions, variables);
		} catch ( std::runtime_error& e ) {
			logger::error["evaluate"] << e.what() << std::endl;
			abort = true;
		}
		if ( !abort ) goto begin_evaluate;
	}

	if ( !abort && tokens.front() == expr::T_FUNCTION ) {
		try {
			tokens = eval(tokens, false, functions, variables);
		} catch ( std::runtime_error& e ) {
			logger::error["evaluate"] << e.what() << std::endl;
			abort = true;
		}
		if ( !abort ) goto begin_evaluate;
	}

	if ( !abort && tokens.front() == expr::T_SUB && !tokens.front()._child.empty()) {
		tokens = tokens.front()._child;
		goto begin_evaluate;
	}

	if ( !abort && tokens.front() == expr::OP_SUB ) {
		if ( tokens.size() > 1 ) goto begin_evaluate;
		try {
			tokens = eval(tokens, false, functions, variables);
		} catch ( std::runtime_error&e ) {
			logger::error["evaluate"] << e.what() << std::endl;
			abort = true;
		}
	}

	if ( !abort && tokens.size() == 1 ) {

		if ( !set_variable.empty() && variables != nullptr ) {

			if ( tokens.front().is_number())
				(*variables)[set_variable] = tokens.front().to_double();
			else if ( tokens.front().is_string())
				(*variables)[set_variable] = tokens.front().to_string();
			else {
				logger::verbose["evaluate"] << "ambiguos result of expr, variable " << set_variable <<
					" was set to null" << std::endl;
				(*variables)[set_variable] = nullptr;
			}
		}
		return tokens.front();

	}

	if ( abort )
		logger::warning["evaluate"] << "evaluation was aborted because of errors";
	else
		logger::warning["evaluate"] << "evaluating ended up with ambiguous results, result will be null";

	if ( !set_variable.empty() && variables != nullptr ) {
		logger::warning << " and variable " << set_variable << " was set to nullptr";
		(*variables)[set_variable] = nullptr;
	}

	logger::warning << std::endl;
	return expr::TOKEN::UNDEF();
}

expr::TOKEN expr::expression::evaluate(expr::FUNCTIONMAP *functions, expr::VARIABLEMAP *variables) {

	std::vector<expr::TOKEN> tokens = this -> _tokens;
	expr::TOKEN result = evaluate(tokens, functions, variables);

	return result;
}

expr::TOKEN expr::expression::evaluate(const std::string& s, expr::FUNCTIONMAP *functions, expr::VARIABLEMAP *variables) {

	this -> _raw = s;
	this -> _tokens = parse_expr(s);

	std::vector<expr::TOKEN> tokens = this -> _tokens;
	expr::TOKEN result = evaluate(tokens, functions, variables);

	return result;
}

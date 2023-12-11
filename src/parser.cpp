#include <map>
#include "common.hpp"
#include "logger.hpp"
#include "expr/expression.hpp"

// important note: longest operator patterns in the beginning, shortest in the end
static std::map<std::string, expr::OP> Pattern1 = {
	{ "==", expr::OP_NEQ },
	{ "!=", expr::OP_NNE },
	{ "<=", expr::OP_NLE },
	{ "=<", expr::OP_NLE },
	{ ">=", expr::OP_NGE },
	{ "=>", expr::OP_NGE },
	{ "<", expr::OP_NLT },
	{ ">", expr::OP_NGT },
	{ "|", expr::OP_OR },
	{ "&", expr::OP_AND },
	{ "!", expr::OP_NOT },
	{ "+", expr::OP_ADD },
	{ "-", expr::OP_SUB },
	{ ".", expr::OP_CAT },
	{ "*", expr::OP_MUL },
	{ "/", expr::OP_DIV },
	{ ",", expr::OP_COM },
	{ "%", expr::OP_MOD },
	{ "^", expr::OP_POW },
	{ "=", expr::OP_SET },
};

// important note: longest operator patterns in the beginning, shortest in the end
static std::map<std::string, expr::OP> Pattern2 = {
	{ "eq", expr::OP_SEQ },
	{ "ne", expr::OP_SNE },
	{ "lt", expr::OP_SLT },
	{ "le", expr::OP_SLE },
	{ "gt", expr::OP_SGT },
	{ "ge", expr::OP_SGE }
};

std::vector<expr::TOKEN> expr::expression::parse_expr(const std::string& expr, bool f_args) {

	std::string s(expr);
	std::vector<TOKEN> tokens;
	unsigned char quote = 0;
	std::string word;
	bool ignore;

	TOKEN token;

	while ( !s.empty()) {

		token.reset();
		ignore = false;

		while ( common::is_space(s))
			s.erase(0, 1);

		if ( s.empty())
			break;

		if ( common::is_alpha(s)) { /* names */

			word += common::erase_front(s);

			while ( common::is_alnum(s))
				word += common::erase_front(s);

			if ( s.size() >= 3 && s.starts_with("::") && common::is_alpha(s.at(2))) {

				word += common::erase_prefix(s, 3);
				while ( common::is_alnum(s))
					word += common::erase_front(s);
			}

			token = expr::T_VARIABLE;

			/* check for alphanumeric operators */
			for ( const auto& [key, op] : Pattern2 ) {

				if ( word == key ) {
					token = op;
					break;
				}
			}

			if ( token == expr::T_VARIABLE )
				token._name = word;

		} else if ( common::is_digit(s) || ( s.front() == '.' && common::is_digit(s.at(1)))) { /* numbers */

			while ( common::is_digit(s))
				word += common::erase_front(s);

			if ( s.front() == '.' ) {

				word += common::erase_front(s);

				while ( common::is_digit(s))
					word += common::erase_front(s);
			}

			token = expr::T_NUMBER;
			try {
				token = std::stod(word);
			} catch ( std::invalid_argument& e ) {
				logger::error << logger::tag("parser") << "cannot convert '" << word << "' to number" << std::endl;
				token = (double)0;
			}

		} else if ( s.front() == '\'' || s.front() == '"' ) { /* string */

			quote = common::erase_front(s);

			while ( !s.empty() && s.front() != quote ) {

				if ( s.front() == '\\' ) {

					switch ( s.size() > 1 ? s.at(1) : 0 ) {
						case '\\':
						case '\'':
							word += '\'';
							s.erase(0, 2);
							break;
						case 'a':
							word += '\a';
							s.erase(0, 2);
							break;
						case 'b':
							word += '\b';
							s.erase(0, 2);
							break;
						case 't':
							word += '\t';
							s.erase(0, 2);
							break;
						case 'n':
							word += '\n';
							s.erase(0, 2);
							break;
						case 'v':
							word += '\v';
							s.erase(0, 2);
							break;
						case 'f':
							word += '\f';
							s.erase(0, 2);
							break;
						case 'r':
							word += '\r';
							s.erase(0, 2);
							break;
						case 'x': {
								int i;
								unsigned char hexC;
								int hex[2];

								for ( i = 0; i < 2; i++ ) {

									hexC = s.at(2 + i);

									if ( hexC >= '0' && hexC <= '9' )
										hex[i] = hexC - '0';
									if ( hexC >= 'a' && hexC <= 'f' )
										hex[i] = hexC - 'a' + 10;
									if ( hexC >= 'A' && hexC <= 'F' )
										hex[i] = hexC - 'A' + 10;
									else break;
								}

								switch ( i ) {
									case 1:
										hexC = hex[0];
										s.erase(0, 3);
										break;
									case 2:
										hexC = hex[0] * 16 + hex[1];
										s.erase(0, 4);
										break;
									default:
										logger::warning << logger::tag("parser") <<
												"Illegal hex sequence '\\x" << s.at(2) << "' in <" <<
												expr << "> keeps unchanged" << std::endl;
										hexC = '\\';
										s.erase(0, 1);
								}

								if ( hexC == 0 )
									logger::warning << logger::tag("parser") <<
											"Null character(s) in <" << expr << "> will be ignored" << std::endl;
								else word += hexC;
							}
							break;
						case '0':
						case '1':
						case '2':
						case '3':
							if ( s.at(2) >= '0' && s.at(2) <= '7' &&
								s.at(3) >= '0' && s.at(3) <= '7' ) {

								word += (( s.at(1) - '0' ) * 64 + ( s.at(2) - '0' ) * 8 + ( s.at(3) - '0' ));
								s.erase(0, 4);
							} else {
								logger::warning << logger::tag("parser") <<
											"illegal octal sequence '\\" <<
											s.at(1) << s.at(2) << s.at(3) <<
											"' in <" << expr << ">" << std::endl;
								word += common::erase_front(s);
							}
							break;
						default:
							logger::warning << logger::tag("parser") <<
									"unknown escape sequence '\\" << s.at(1) <<
									"' in <" << expr << ">" << std::endl;
							word += common::erase_front(s);
					}
				} else word += common::erase_front(s);
			}

			token = expr::T_STRING;

			if ( s.front() == quote )
				s.erase(0, 1);
			else
				logger::warning << logger::tag("parser") <<
						"unterminated string in <" << expr << ">" << std::endl;

			token._value = word;

		} else { /* non-alpha operators */

			bool skip = false;

			for ( const auto& [key, op] : Pattern1 ) {

				if ( s.size() >= key.size() && s.starts_with(key)) {

					if ( op == expr::OP_COM && !f_args ) {
						logger::warning << logger::tag("parser") <<
							"comma operator is allowed only when defining function arguments <" << expr << ">" << std::endl;
						continue;
					}

					token = op;
					word = common::erase_prefix(s, key.size());

					if ( tokens.size() > 1 && token == expr::OP_SUB &&
						tokens[tokens.size() - 1] == expr::OP_SUB &&
						tokens[tokens.size() - 2] == expr::OP_SUB ) {
						tokens.pop_back(); // 2 negation signs in row negate their selves
						skip = true;
					} else if ( tokens.size() == 1 && token == expr::OP_SUB && tokens.back() == expr::OP_SUB ) {
						tokens.pop_back(); // 2 negation signs in row negate their selves
						skip = true;
					} else if ( token == expr::OP_NOT && !tokens.empty() && tokens.back() == expr::OP_NOT ) {
						tokens.back() = expr::OP_NNOT;
						skip = true;
					} else if ( token == expr::OP_NOT && !tokens.empty() && tokens.back() == expr::OP_NNOT ) {
						tokens.back() = expr::OP_NOT;
						skip = true;
					} else if ( token == expr::OP_NOT && !tokens.empty() && tokens.back() == expr::OP_SUB ) {
						logger::error << logger::tag("parser") <<
							"operator NOT(!) is not allowed to follow operator SGN or SUB (-), if you must " <<
							"use it, place NOT expression inside parentheses, ignoring NOT(!) now" << std::endl;
						skip = true;
					}

					break;
				}
			}

			if ( skip ) {
				token.reset();
				word = "";
				continue;
			}

		}

		if ( !s.empty() && s.front() == '(' && token != expr::OP_SUB ) {

			bool escaping = false;
			char quote = 0;
			int brace_level = 1;
			std::string child_expr;

			s.erase(0, 1);
			while ( !s.empty() && brace_level > 0 ) {

				if ( quote == 0 && ( s.front() == '\'' || s.front() == '"' )) quote = s.front();
				else if ( quote != 0 && !escaping && s.front() == '\\' ) escaping = true;
				else if ( quote != 0 && !escaping && s.front() == quote ) quote = 0;
				else if ( quote == 0 && s.front() == '(' ) brace_level++;
				else if ( quote == 0 && s.front() == ')' ) {
					brace_level--;
					if ( brace_level == 0 ) {
						s.erase(0, 1);
						break;
					}
				} else if ( escaping ) escaping = false;

				child_expr += common::erase_front(s);
			}

			if ( brace_level != 0 )
				logger::warning << logger::tag("parser") << "uneven braces in <" << expr << ">" << std::endl;

			if ( token == expr::T_VARIABLE ) {
				token = expr::T_FUNCTION;
				token._args = parse_expr(child_expr, true);

			} else if ( token == expr::T_UNDEF ) {

				token = expr::T_SUB;
				token._child = parse_expr(child_expr, false);
			}
		}

		if ( !s.empty() && s.front() == '?' && token == expr::T_UNDEF ) {

			bool escaping = false;
			char quote = 0;
			int brace_level = 0;
			std::string expr1;
			std::string expr2;
			bool cnd_complete = false;
			bool abort = false;

			s.erase(0, 1);

			while (common::is_space(s))
				s.erase(0, 1);

			while ( !s.empty() && !cnd_complete ) {

				if ( quote == 0 && ( s.front() == '\'' || s.front() == '"' )) quote = s.front();
				else if ( quote != 0 && !escaping && s.front() == '\\' ) escaping = true;
				else if ( quote != 0 && !escaping && s.front() == quote ) quote = 0;
				else if ( quote == 0 && s.front() == '(' )
					brace_level++;
				else if ( quote == 0 && s.front() == ')' )
					brace_level--;
				else if ( quote == 0 && brace_level == 0 && s.front() == ':' ) {
					cnd_complete = true;
					s.erase(0, 1);
					break;
				} else if ( escaping ) escaping = false;

				expr1 += common::erase_front(s);
			}

			if ( !cnd_complete ) {
				if ( quote != 0 )
					logger::error << logger::tag("parser") << "uneven quotes inside condition <" << expr << ">" << std::endl;
				else if ( brace_level != 0 )
					logger::error << logger::tag("parser") << "uneven braces inside condition <" << expr << ">" << std::endl;
				else {
					logger::error << logger::tag("parser") << "invalid condition, operator COL(:) and false result missing, " <<
						"syntax is x( != 0 ) ? true : false" << std::endl;
					logger::verbose << logger::tag("parser") << "invalid condition found from <" << expr << ">" << std::endl;
				}

				abort = true;
			}

			if ( !abort && expr1.empty()) {
				logger::error << logger::tag("parser") << "error, conditionals true result is null <" << expr << " >" << std::endl;
				expr1 = "0";
			}

			while (common::is_space(s))
                                s.erase(0, 1);

			escaping = false;
			quote = 0;
			brace_level = 0;
			cnd_complete = false;

			while ( !abort && !s.empty() && !cnd_complete ) {

				if ( quote == 0 && ( s.front() == '\'' || s.front() == '"' )) quote = s.front();
				else if ( quote != 0 && !escaping && s.front() == '\\' ) escaping = true;
				else if ( quote != 0 && !escaping && s.front() == quote ) quote = 0;
				else if ( quote == 0 && s.front() == '(' )
					brace_level++;
				else if ( quote == 0 && s.front() == ')' )
					brace_level--;
				else if ( quote == 0 && brace_level == 0 && s.front() == ' ' ) {
					cnd_complete = true;
					s.erase(0, 1);
					break;
				} else if ( escaping ) escaping = false;

                                expr2 += common::erase_front(s);
                        }

			if ( !abort && s.empty() && quote == 0 && brace_level == 0 )
				cnd_complete = true;

			if ( !abort && !cnd_complete ) {
				if ( quote != 0 )
					logger::error << logger::tag("parser") << "uneven quotes inside condition <" << expr << ">" << std::endl;
				else if ( brace_level != 0 )
					logger::error << logger::tag("parser") << "uneven braces inside condition <" << expr << ">" << std::endl;
				else
					logger::error << logger::tag("parser") << "unknown condition parsing error with < " << expr << ">" << std::endl;

				abort = true;
			}

			if ( !abort && expr2.empty()) {
				logger::error << logger::tag("parser") << "error, conditionals false result is null <" << expr << " >" << std::endl;
				expr2 = "0";
			}

			if ( !abort ) {
				token = expr::T_CONDITIONAL;
				token._cond1 = parse_expr(expr1, false);
				token._cond2 = parse_expr(expr2, false);
			}

			s += "   "; // Add some white-space, next part may remove it..
		}

		if ( !f_args && token == expr::T_UNDEF && !s.empty())
			s.erase(0, 1);

		while ( common::is_space(s))
			s.erase(0, 1);

		// last checks..

		if ( !ignore && !tokens.empty() && token == expr::T_OPERATOR && tokens.back() == expr::T_OPERATOR ) {

			if ( !(( token == expr::OP_SUB && tokens.back() == expr::OP_SUB ) ||
				( token == expr::OP_SUB && tokens.back() == expr::OP_NOT ) ||
				( token == expr::OP_SUB && tokens.back() == expr::OP_NNOT ) ||
				( token == expr::OP_NOT && tokens.back() == expr::OP_SUB ) ||
				( token == expr::OP_NNOT && tokens.back() == expr::OP_SUB ))) {

				logger::warning << logger::tag("parser") << "2 mathematical modifier operators ( " << describe(token._op) <<
					" and " << describe(tokens.back()._op) << " cannot be in row, ignoring operator " <<
					describe(token._op) << std::endl;

				ignore = true;
			}
		}

		if ( !ignore )
			tokens.push_back(token);

		token.reset();
		word = "";

	}

	return tokens;
}

bool expr::expression::validate_set_op(std::vector<expr::TOKEN>& tokens, const std::string& expr, bool is_root) {

	bool result = true;
	bool restart;

	begin_validate:

	restart = false;

	for ( size_t i = 0; i < tokens.size(); i++ ) {

		if ( is_root && i == 1 && tokens[i] == T_OPERATOR && tokens[i] == OP_SET ) {

			if ( tokens[0] == T_VARIABLE && tokens.size() > 2 &&
				tokens[2] != OP_COM ) continue;
			else if ( tokens[0] == T_VARIABLE && tokens.size() > 2 &&
				tokens[2] == OP_SET ) {

				logger::warning << logger::tag("validator") << "multiple SET operators in sequence, removing " <<
					"extra operators <" << expr << ">" << std::endl;

					while ( tokens.size() > 2 && tokens[2] == OP_SET )
						tokens.erase(tokens.begin() + 2);

					result = false;
					restart = true;
					break;

			} else if ( tokens[0] != expr::T_VARIABLE || tokens[0]._name.empty()) {

				logger::error << logger::tag("validator") << "SET operator used, but left side " <<
					"argument's type is not a variable, ignoring SET <" << expr << ">" << std::endl;

				tokens.erase(tokens.begin(), tokens.begin() + 2);
				result = false;
				restart = true;
				break;

			} else if ( tokens.size() == 2 ) {

				logger::error << logger::tag("validator") << "SET operator used, but right side " <<
					"argument is missing, ignoring SET <" << expr << ">" << std::endl;

				tokens.erase(tokens.begin() + 1);
				result = false;
				restart = true;
				break;
			}

			logger::error << logger::tag("validator") << "SET operator used and unexpected unknown " <<
				"error occurred, ignoring SET <" << expr << ">" << std::endl;
			tokens.erase(tokens.begin() + 1);
			result = false;
			restart = true;
			break;
		}

		if ( is_root && i == 0 && tokens[i] == T_OPERATOR && tokens[i] == OP_SET ) {

			logger::error << logger::tag("validator") << "SET operator used, but left side " <<
				"argument's type is not a variable, ignoring SET <" << expr << ">" << std::endl;

			tokens.erase(tokens.begin());
			result = false;
			restart = true;
			break;
		}

		if ( tokens[i] == T_OPERATOR && tokens[i] == OP_SET ) {

			logger::error << logger::tag("validator") << "SET operator in wrong place, SET can " <<
				"only be used in beginning of expression as second argument after variable argument" << std::endl;
			logger::error << logger::tag("validator") << "ignoring SET <" << expr << ">" << std::endl;

			tokens.erase(i == 0 ? tokens.begin() : ( tokens.begin() + ( i + 1 )));
			result = false;
			restart = true;
			break;
		}

		if ( tokens[i] == T_CONDITIONAL ) {

			if ( !tokens[i]._cond1.empty() && !validate_set_op(tokens[i]._cond1, expr, false)) {

				result = false;
				logger::vverbose << logger::tag("validator") << "SET expression failure in conditionals true result" << std::endl;
				break;

			} else if ( !validate_set_op(tokens[i]._cond2, expr, false)) {

				result = false;
				logger::vverbose << logger::tag("validator") << "SET expression failure in conditionals false result" << std::endl;
				break;
			}

		} else if ( tokens[i] == expr::T_SUB && !tokens[i]._child.empty() &&
			!validate_set_op(tokens[i]._child, expr, false)) {

				result = false;
				logger::vverbose << logger::tag("validator") << "SET expression failure in expression inside parentheses" << std::endl;
				break;

		} else if ( tokens[i] == expr::T_FUNCTION ) {

			bool func_reset;

			begin_func_validation:

			func_reset = false;

			for ( size_t arg_i = 0; arg_i < tokens[i]._args.size(); arg_i++ ) {

				if ( tokens[i]._args[arg_i] == expr::OP_SET ) {

					logger::error << logger::tag("validator") << "SET operator in wrong place, SET is not allowed " <<
						"in functions arguments, ignoring SET <" << expr << ">" << std::endl;

					result = false;
					tokens[i]._args.erase(arg_i == 0 ? tokens[i]._args.begin() : ( tokens[i]._args.begin() + arg_i ));
					func_reset = true;
					break;
				}
			}

			if ( func_reset )
				goto begin_func_validation;

			begin_comma_validation:

			func_reset = false;

			for ( size_t arg_i = 0; arg_i < tokens[i]._args.size(); arg_i++ ) {

				if ( arg_i == 0 && tokens[i]._args[arg_i] == expr::OP_COM ) {

					logger::verbose << logger::tag("sanitizer") << "Double COM(,) operator after validating SET " <<
						"operators from function args, ignoring COM operator <" << expr << ">" << std::endl;
					tokens[i]._args.erase(arg_i == 0 ? tokens[i]._args.begin() : ( tokens[i]._args.begin() + arg_i ));
					result = false;
					func_reset = true;
					break;

				} else if ( arg_i < ( tokens[i]._args.size() - 1 ) &&
					tokens[i]._args[arg_i] == expr::OP_COM && tokens[i]._args[arg_i + 1] == expr::OP_COM ) {

					logger::verbose << logger::tag("sanitizer") << "Double COM(,) operator after validating SET " <<
						"operators from function args, ignoring COM operator <" << expr << ">" << std::endl;
					tokens[i]._args.erase(arg_i == 0 ? tokens[i]._args.begin() : ( tokens[i]._args.begin() + arg_i ));
					result = false;
					func_reset = true;
					break;
				} else if ( arg_i < (tokens[i]._args.size() - 1 ) &&
					tokens[i]._args[arg_i] == expr::T_OPERATOR && tokens[i]._args[arg_i + 1] == expr::OP_COM ) {
					logger::verbose << logger::tag("sanitizer") << "function argument's last argument is operator " <<
						describe(tokens[i]._args[arg_i]._op) << " which cannot work, ignoring it <" << expr << ">" << std::endl;
					tokens[i]._args.erase(arg_i == 0 ? tokens[i]._args.begin() : ( tokens[i]._args.begin() + arg_i ));
					result = false;
					func_reset = true;
					break;
				}
			}

			if ( func_reset )
				goto begin_comma_validation;
		}
	}

	if ( restart )
		goto begin_validate;

	return result;
}

std::vector<expr::TOKEN> expr::expression::parse_expr(const std::string& s) {

	std::vector<expr::TOKEN> tokens = parse_expr(s, false);

	if ( !tokens.empty() && !validate_set_op(tokens, describe(tokens), true)) {
		logger::warning << logger::tag("sanitizer") << "failures in expression with SET argument <" << s << ">" << std::endl;
		logger::vverbose << logger::tag("sanitizer") << "expression might be broken, expression after sanitizing: <" << describe(tokens) << ">" << std::endl;
	}

	return tokens;
}

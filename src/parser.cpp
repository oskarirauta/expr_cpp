#include <iostream>

#include <map>

#include "common.hpp"
#include "logger.hpp"
#include "eval/token.hpp"
#include "eval/parser.hpp"

// important note: longest operator patterns in the beginning, shortest in the end
static std::map<std::string, OP> Pattern1 = {
	{ "==", OP_NEQ },
	{ "!=", OP_NNE },
	{ "<=", OP_NLE },
	{ "=<", OP_NLE },
	{ ">=", OP_NGE },
	{ "=>", OP_NGE },
	{ "<", OP_NLT },
	{ ">", OP_NGT },
	{ "|", OP_OR },
	{ "&", OP_AND },
	{ "!", OP_NOT },
	{ "+", OP_ADD },
	{ "-", OP_SUB },
	{ ".", OP_CAT },
	{ "*", OP_MUL },
	{ "/", OP_DIV },
	{ ",", OP_COM },
	{ "%", OP_MOD },
	{ "^", OP_POW },
/* // Handling this differently..
	{ "=", OP_SET },
*/
};

// important note: longest operator patterns in the beginning, shortest in the end
static std::map<std::string, OP> Pattern2 = {
	{ "eq", OP_SEQ },
	{ "ne", OP_SNE },
	{ "lt", OP_SLT },
	{ "le", OP_SLE },
	{ "gt", OP_SGT },
	{ "ge", OP_SGE }
};

static std::vector<TOKEN> parse(const std::string& expr, bool f_args) {

	std::string s(expr);
	std::vector<TOKEN> tokens;
	unsigned char quote = 0;
	std::string word;

	TOKEN token;

	while ( !s.empty()) {

		token.reset();

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

			token.type = T_VARIABLE;

			/* check for alphanumeric operators */
			for ( const auto& [key, op] : Pattern2 ) {

				if ( word == key ) {
					token.type = T_OPERATOR;
					token.op = op;
					break;
				}
			}

			if ( token.type == T_VARIABLE )
				token.name = word;

		} else if ( common::is_digit(s) || ( s.front() == '.' && common::is_digit(s.at(1)))) { /* numbers */

			while ( common::is_digit(s))
				word += common::erase_front(s);

			if ( s.front() == '.' ) {

				word += common::erase_front(s);

				while ( common::is_digit(s))
					word += common::erase_front(s);
			}

			token.type = T_NUMBER;
			try {
				token.value = std::stod(word);
			} catch ( std::invalid_argument& e ) {
				logger::error << logger::tag("parser") << "cannot convert '" << word << "' to number" << std::endl;
				token.value = (double)0;
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

			token.type = T_STRING;

			if ( s.front() == quote )
				s.erase(0, 1);
			else
				logger::warning << logger::tag("parser") <<
						"unterminated string in <" << expr << ">" << std::endl;

			token.value = word;

		} else { /* non-alpha operators */

			bool skip = false;

			for ( const auto& [key, op] : Pattern1 ) {

				if ( s.size() >= key.size() && s.starts_with(key)) {

					if ( op == OP_COM && !f_args ) {
						logger::warning << logger::tag("parser") <<
							"comma operator is allowed only when defining function arguments <" << expr << ">" << std::endl;
						continue;
					}

					token.type = T_OPERATOR;
					token.op = op;
					word = common::erase_prefix(s, key.size());

					if ( token.op == OP_NOT && !tokens.empty() && tokens.back().type == T_OPERATOR &&
						tokens.back().op == OP_NOT ) {
						logger::error << logger::tag("parser") <<
							"multiple NOT(!) operators in sequence are not allowed, if you must use them, " <<
							"place them inside parentheses, ignoring operator now" << std::endl;
						skip = true;
					} else if ( token.op == OP_SUB && !tokens.empty() && tokens.back().type == T_OPERATOR &&
						tokens.back().op == OP_NOT ) {
						logger::error << logger::tag("parser") <<
							"operator SGN(-) is not allowed to follow operator NOT(!), if you must use NOT " <<
							"on negative value, place value inside parentheses, ignoring now" << std::endl;
						skip = true;
					} else if ( token.op == OP_NOT && !tokens.empty() && tokens.back().type == T_OPERATOR &&
						tokens.back().op == OP_SUB ) {
						logger::error << logger::tag("parser") <<
							"operator NOT(!) is not allowed to follow operator SGN or SUB (-), if you must " <<
							"use it, place NOT expression inside parentheses, ignoring now" << std::endl;
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

		if ( !s.empty() && s.front() == '(' && (
			( token.type != T_OPERATOR && token.op != OP_SUB )
			)) {

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

			if ( token.type == T_VARIABLE ) {
				token.type = T_FUNCTION;
				token.args = parse(child_expr, true);

			} else if ( token.type == T_UNDEF ) {

				token.type = T_SUB;
				token.child = parse(child_expr, false);
			}
		}

		if ( !s.empty() && s.front() == '?' && token.type == T_UNDEF ) {

			bool escaping = false;
			char quote = 0;
			int brace_level = 0;
			std::string expr1;
			std::string expr2;
			bool cnd_complete = false;

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

				// todo: cancel here..
			}

			if ( expr1.empty()) {
				logger::error << logger::tag("parser") << "error, conditionals true result is null <" << expr << " >" << std::endl;
				expr1 = "0";
			}

			while (common::is_space(s))
                                s.erase(0, 1);

			escaping = false;
			quote = 0;
			brace_level = 0;
			cnd_complete = false;

			while ( !s.empty() && !cnd_complete ) {

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

			if ( s.empty() && quote == 0 && brace_level == 0 )
				cnd_complete = true;

			if ( !cnd_complete ) {
				if ( quote != 0 )
					logger::error << logger::tag("parser") << "uneven quotes inside condition <" << expr << ">" << std::endl;
				else if ( brace_level != 0 )
					logger::error << logger::tag("parser") << "uneven braces inside condition <" << expr << ">" << std::endl;
				else
					logger::error << logger::tag("parser") << "unknown condition parsing error with < " << expr << ">" << std::endl;

				// todo: cancel here
			}

			if ( expr2.empty()) {
				logger::error << logger::tag("parser") << "error, conditionals false result is null <" << expr << " >" << std::endl;
				expr2 = "0";
			}

			token.type = T_CONDITIONAL;
			token.cond1 = parse(expr1, false);
			token.cond2 = parse(expr2, false);
		}

		if ( !f_args && token.type == T_UNDEF && !s.empty())
			s.erase(0, 1);

		while ( common::is_space(s))
			s.erase(0, 1);

		tokens.push_back(token);
		token.reset();
		word = "";

	}

	return tokens;
}

std::vector<TOKEN> parse(const std::string& expr) {

	return parse(expr, false);
}

static const std::string describe(const std::vector<TOKEN>& tokens, bool f_args) {

	if ( tokens.empty())
		return "";

	std::string s;

	for ( size_t i = 0; i < tokens.size(); i++ ) {

		if ( i != 0 ) s += ' ';

		switch ( tokens[i].type ) {
			case T_OPERATOR:
				if ( tokens[i].op == OP_COM && !s.empty())
					s.pop_back();
				s += describe(tokens[i].op);
				break;
			case T_VARIABLE:
				s += common::to_lower(tokens[i].name);
				break;
			case T_FUNCTION:
				s += common::to_lower(tokens[i].name);
				s += '(';
				s += describe(tokens[i].args, true);
				s += ')';
				break;
			case T_SUB:
				s += "( ";
				s += describe(tokens[i].child, false);
				s += " )";
				break;
			case T_CONDITIONAL:
				s += "?";
				if ( tokens[i].cond1.size() == 1 )
					s += " " + describe(tokens[i].cond1, false) + " :";
				else s += " ( " + describe(tokens[i].cond1, false) + " ) :";
				if ( tokens[i].cond2.size() == 1 )
					s += " " + describe(tokens[i].cond2, false);
				else s += " ( " + describe(tokens[i].cond2, false) + " )";
				break;
			case T_UNDEF:
				s += "T_UNDEF";
				break;
			default:
				if ( tokens[i].isNull()) s += "NULL";
				else if ( tokens[i].isString()) s += "'" + std::get<std::string>(tokens[i].value) + "'";
				else if ( tokens[i].isNumber()) s += common::to_string(std::get<double>(tokens[i].value));
				else s += "UNK";
		}
	}

	return s;
}

const std::string describe(const std::vector<TOKEN>& tokens) {

	return describe(tokens, false);
}

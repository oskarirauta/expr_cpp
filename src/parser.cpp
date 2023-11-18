#include <iostream>

#include <map>

#include "common.hpp"
#include "logger.hpp"
#include "eval/token.hpp"
#include "eval/parser.hpp"

static std::map<std::string, OP> Pattern1 = {
	{ "==", OP_NEQ },
	{ "!=", OP_NNE },
	{ "<=", OP_NLE },
	{ ">=", OP_NGE },
	{ "=", OP_SET },
	{ "?", OP_CND },
	{ "+", OP_ADD },
	{ "-", OP_SUB },
	{ ".", OP_CAT },
	{ ",", OP_COM }
};

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
	int skip = 0;
	unsigned char quote = 0;
	std::string word;

	TOKEN token;

	while ( !s.empty()) {

		while ( common::is_space(s))
			s.erase(0, 1);

		if ( skip ) {
			s.erase(0, 1);
			skip--;
			continue;
		}

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
				std::cout << "parser: error, cannot convert '" << word << "' to number" << std::endl;
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
										std::cout << "parser: Illegal hex sequence '\\x" << s.at(2) << "' in <" << expr <<
												"> keeps unchanged" << std::endl;
										hexC = '\\';
										s.erase(0, 1);
								}

								if ( hexC == 0 )
									std::cout << "parser: Null character(s) in <" << expr << "> will be ignored" << std::endl;
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
								std::cout << "parser: illegal octal sequence '\\"
									<< s.at(1) << s.at(2) << s.at(3) << "' in <" << expr << ">" << std::endl;
								word += common::erase_front(s);
							}
							break;
						default:
							std::cout << "parser: unknown escape sequence '\\" << s.at(1) << "' in <" << expr << ">" << std::endl;
							word += common::erase_front(s);
					}
				} else word += common::erase_front(s);
			}

			token.type = T_STRING;

			if ( s.front() == quote )
				s.erase(0, 1);
			else
				std::cout << "parser: unterminated string in <" << expr << ">" << std::endl;

			token.value = word;

		} else { /* non-alpha operators */

			for ( const auto& [key, op] : Pattern1 ) {

				if ( s.size() >= key.size() && s.starts_with(key)) {

					if ( op == OP_COM && !f_args ) {
						std::cout << "parser: comma operator is allowed only when defining function arguments <" << expr << ">" << std::endl;
						continue;
					}

					token.type = T_OPERATOR;
					token.op = op;
					word = common::erase_prefix(s, key.size());
					break;
				}
			}
		}

		if ( !s.empty() && s.front() == '(' ) {

			int brace_level = 1;
			std::string child_expr;

			s.erase(0, 1);
			while ( !s.empty() && brace_level > 0 ) {

				if ( s.front() == '(' ) brace_level++;
				else if ( s.front() == ')' ) {
					brace_level--;
					if ( brace_level == 0 ) break;
				}

				child_expr += common::erase_front(s);
			}

			if ( brace_level == 0 && !s.empty())
				s.erase(0, 1);
			else std::cout << "parser: uneven braces in <" << expr << ">" << std::endl;

			if ( token.type == T_VARIABLE ) {

				token.type = T_FUNCTION;
				token.args = parse(child_expr, true);

			} else if ( token.type == T_UNDEF ) {

				token.type = T_SUB;
				token.child = parse(child_expr, false);
				word += child_expr;
			}
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

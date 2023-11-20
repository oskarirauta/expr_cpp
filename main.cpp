#include <iostream>
#include <map>
#include <string>
#include <stdexcept>

#include "eval/token.hpp"
#include "eval/parser.hpp"
#include "eval/functions.hpp"
#include "eval/variables.hpp"
#include "eval/evaluation.hpp"
#include "logger.hpp"

#include <cmath>

FUNCTION_ARG hello(FUNCTION_ARGS args) {

	FUNCTION_ARG ret;
	double d = 0;


	int i = 0;

	for ( FUNCTION_ARG& arg : args ) {
		i++;
		if ( std::holds_alternative<std::nullptr_t>(arg))
			std::cout << "problem: arg #" << i << " is null" << std::endl;
		else if ( std::holds_alternative<std::string>(arg))
			std::cout << "problem: arg #" << i << " is string (" << std::get<std::string>(arg) << ")" << std::endl;
		else {
			//std::cout << "parsing: arg #" << i << "(" << std::get<double>(arg) << ")" << std::endl;
			d += std::get<double>(arg);
		}
	}

	ret = d;
	return ret;
}

int main(int argc, char **argv) {

	logger::loglevel(logger::debug);

	std::cout << "evaluator++\n" << std::endl;

	//std::string s = "  hello::a+   world 123 + 456\t'str1' \"str2\" func1() + func2('hello', 'world') + ('sub1' + 'sub2')";
	//std::string s = "    999 +  1 + 1 + ( 5 + 2 )";
	//std::string s = "1 + hello(1, 2, 5 + 6 + 9, (7 + 2))";
	//std::string s = "1 + hello((1 + 1), 1 + ( 1 + 1), 1, 1 + hello(1, 1))";
	//std::string s = "(1+1)";
	//std::string s = "(xxx)";
	//std::string s = "hello(1,1, 1 + 1)";
	//std::string s = "hello(1, hello(1, hello(1)), 1) + 1 + hello(1 + xxx)";
	//std::string s = "xxx";
	//std::string s = "hello(xxx)";
	//std::string s = "xxx + ( 10 + xxx + xxx + hello(xxx))";
	//std::string s = "xxx + ( 10 + 10 )";
	//std::string s = "xxx + 10 + 10";
	//std::string s = "10 - 5";
	//std::string s = "'hello' . ' ' . 'world'";
	//std::string s = "'1' . '0' + 10";
	//std::string s = "-1";
	//std::string s = "-0";
	//std::string s = "-(1 + 9)";
	//std::string s = "-(-10 + 2)";
	//std::string s = "5 + - - - 4";
	//std::string s = "- - - - 5";
	//std::string s = "2 * 5";
	//std::string s = "10 / 0";
	//std::string s = "0 / 10";
	//std::string s = "10 / 2";
	//std::string s = "1 < 0";
	std::string s = "bb ? ( 'hello ' . 'world' ) : 'not today'";
	//std::string s = "? 1 : 0";

	std::vector<TOKEN> tokens = parse(s);

	std::string pretty = describe(tokens);
	std::cout << "dirty: " << s << std::endl;
	std::cout << "expression: " << pretty << std::endl;

	FUNCTIONS functions = {
		{ "hello", hello }
	};

	VARIABLES variables = {
		{ "xxx", (double)10 },
		{ "bb", (double)1 }
	};

	TOKEN token;

	try {
		token = evaluate(tokens, &functions, &variables);
	} catch ( std::runtime_error &e ) {
		token = TOKEN::UNDEF();
		std::cout << "runtime error, " << e.what() << std::endl;
	}

	std::cout << "result: ";

	if ( token.type == T_NUMBER )
		std::cout << std::get<double>(token.value) << ( std::get<double>(token.value) == 0 ? " (zero)" : "" ) << std::endl;
	else if ( token.type == T_STRING )
		std::cout << std::get<std::string>(token.value) << std::endl;
	else std::cout << "undef(error)" << std::endl;

	std::cout << std::endl;

	return 0;
}

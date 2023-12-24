#include <iostream>
#include <map>
#include <string>
#include <stdexcept>

#include "logger.hpp"
#include "expr/expression.hpp"

expr::VARIABLE hello(expr::FUNCTION_ARGS args) {

	expr::VARIABLE ret = 0;
	int i = 0;

	for ( expr::VARIABLE& arg : args ) {
		i++;
		if ( arg == expr::V_NULLPTR )
			std::cout << "problem: arg #" << i << " is null" << std::endl;
		else if ( arg == expr::V_STRING )
			std::cout << "problem: arg #" << i << " is string (" << (std::string)arg << ")" << std::endl;
		else if ( arg == expr::V_NUMBER )
			ret = (double)ret + (double)arg;
		else std::cout << "hello function: error with arg#" << i << ": unknown arg type" << std::endl;
	}

	return ret;
}

int main(int argc, char **argv) {

	logger::loglevel(logger::debug);

	std::cout << "evaluator++\n" << std::endl;

	//std::string s = "1";
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
	//std::string s = "bb ? ( 'hello ' . 'world' ) : 'not today'";
	//std::string s = "? 1 : 0";

	//std::string s = "Z = 150";
	//std::string s = "!10";
	//std::string s = "!-10";
	//std::string s = "!!!!!!!!10";
	//std::string s = "!!!!!!!!-10";

	//std::string s = "1 ++ 1";
	//std::string s = "'HELLO WORLD'";
	//std::string s = "myvar = 'hello'";

	//std::string s = "         myvar     = 'hello'";
	//std::string s = "s";

	expr::expression e(s);

	std::string pretty = describe(e);
	std::cout << "dirty: " << e.raw() << std::endl;
	std::cout << "expression: " << e << std::endl;

	expr::FUNCTIONMAP functions = {
		{ "hello", hello }
	};

	expr::VARIABLEMAP variables = {
		{ "xxx", (double)10 },
		{ "bb", (double)1 },
		{ "s", "hello world" },
	};

	expr::RESULT result;

	try {
		result = e.evaluate(&functions, &variables);
	} catch ( std::runtime_error &e ) {
		result = nullptr;
		std::cout << "runtime error, " << e.what() << std::endl;
	}

	std::cout << "result: " << result/*.lowercase()*/ << " description: " << describe(result) << std::endl;
	return 0;
}

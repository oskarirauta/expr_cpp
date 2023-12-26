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

	std::string s;
	//s = "1";
	//s = "  hello::a+   world 123 + 456\t'str1' \"str2\" func1() + func2('hello', 'world') + ('sub1' + 'sub2')";
	//s = "    999 +  1 + 1 + ( 5 + 2 )";
	//s = "1 + hello(1, 2, 5 + 6 + 9, (7 + 2))";
	//s = "1 + hello((1 + 1), 1 + ( 1 + 1), 1, 1 + hello(1, 1))";
	//s = "(1+1)";
	//s = "(xxx)";
	//s = "hello(1,1, 1 + 1)";
	//s = "hello(1, hello(1, hello(1)), 1) + 1 + hello(1 + xxx)";
	//s = "xxx";
	//s = "hello(xxx)";
	//s = "xxx + ( 10 + xxx + xxx + hello(xxx))";
	//s = "xxx + ( 10 + 10 )";
	//s = "xxx + 10 + 10";
	//s = "10 - 5";
	//s = "'hello' . ' ' . 'world'";
	//s = "'1' . '0' + 10";
	//s = "-1";
	//s = "-0";
	//s = "-(1 + 9)";
	//s = "-(-10 + 2)";
	//s = "5 + - - - 4";
	//s = "- - - - 5";
	//s = "2 * 5";
	//s = "10 / 0";
	//s = "0 / 10";
	//s = "10 / 2";
	//s = "1 < 0";
	//s = "bb ? ( 'hello ' . 'world' ) : 'not today'";
	//s = "? 1 : 0";

	//s = "Z = 150";
	//s = "!10";
	//s = "!-10";
	//s = "!!!!!!!!10";
	//s = "!!!!!!!!-10";

	//s = "1 ++ 1";
	//s = "'HELLO WORLD'";
	//s = "myvar = 'hello'";

	//s = "         myvar     = 'hello'";
	//s = "s";
	s = "1 == 1 ? 2 : 5";

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

	std::cout << "\nExample with property helper:" << std::endl;
	expr::PROPERTYMAP properties = {
		{ "hello", "'hello world'" },
		{ "world", "'hello ' . 'world'" },
		{ "s1", "helloworld1" },
		{ "s2", "hello world2" },
		{ "s3", "'hello ' . world "},
		{ "d1", "10" }
	};

	expr::PROPERTY property;
	property = expr::PROPERTY(&properties, &functions, &variables);
	//expr::PROPERTY property(&properties, &functions, &variables);

	std::cout << "hello: " << property.pretty("hello") << " = " << property["hello"] << std::endl;
	std::cout << "world: " << property.pretty("world") << " = " << property["world"] << std::endl;
	std::cout << "s1: " << property.pretty("s1") << " = " << property["s1"] << std::endl;
	std::cout << "s2: " << property.pretty("s2") << " = " << property["s2"] << std::endl;
	std::cout << "d1: " << property.pretty("d1") << " = " << property["d1"] << std::endl;
	std::cout << "d2: " << property.pretty("d2") << " = " << property["d2"] << std::endl;
	std::cout << "d2: " << property.pretty("d2") << " = " << property["d2", "9"] << " (default: 9)" << std::endl;

	std::cout << "\nIterate through property keys:" << std::endl;
	for ( const std::string p : property ) {

		std::cout << p << std::endl;
	}

	return 0;
}

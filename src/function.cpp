#include <ctime>

#include "expr/function.hpp"

/*
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
*/


expr::VARIABLE expr::functions::time_hour(expr::FUNCTION_ARGS args) {

	std::time_t v = !args.empty() && args[0] == expr::V_NUMBER ? (std::time_t)((double)args[0]) : std::time_t(0);
	std::time_t t = std::time(&v);
	return std::localtime(&t) -> tm_hour;
}

expr::VARIABLE expr::functions::time_min(expr::FUNCTION_ARGS args) {

	std::time_t v = !args.empty() && args[0] == expr::V_NUMBER ? (std::time_t)((double)args[0]) : std::time_t(0);
	std::time_t t = std::time(&v);
	return std::localtime(&t) -> tm_min;
}

expr::VARIABLE expr::functions::time_sec(expr::FUNCTION_ARGS args) {

	std::time_t v = !args.empty() && args[0] == expr::V_NUMBER ? (std::time_t)((double)args[0]) : std::time_t(0);
	std::time_t t = std::time(&v);
	return std::localtime(&t) -> tm_sec;
}

expr::VARIABLE expr::functions::date_day(expr::FUNCTION_ARGS args) {

	std::time_t v = !args.empty() && args[0] == expr::V_NUMBER ? (std::time_t)((double)args[0]) : std::time_t(0);
	std::time_t t = std::time(&v);
	return std::localtime(&t) -> tm_mday;
}

expr::VARIABLE expr::functions::date_month(expr::FUNCTION_ARGS args) {

	std::time_t v = !args.empty() && args[0] == expr::V_NUMBER ? (std::time_t)((double)args[0]) : std::time_t(0);
	std::time_t t = std::time(&v);
	return ( 1 + std::localtime(&t) -> tm_mon );
}

expr::VARIABLE expr::functions::date_year(expr::FUNCTION_ARGS args) {

	std::time_t v = !args.empty() && args[0] == expr::V_NUMBER ? (std::time_t)((double)args[0]) : std::time_t(0);
	std::time_t t = std::time(&v);
	return ( 1900 + std::localtime(&t) -> tm_year );
}

expr::VARIABLE expr::functions::date_weekday(expr::FUNCTION_ARGS args) {

	std::time_t v = !args.empty() && args[0] == expr::V_NUMBER ? (std::time_t)((double)args[0]) : std::time_t(0);
	std::time_t t = std::time(&v);
	return std::localtime(&t) -> tm_wday;
}

expr::VARIABLE expr::functions::date_day_name(expr::FUNCTION_ARGS args) {

	std::time_t v = !args.empty() && args[0] == expr::V_NUMBER ? (std::time_t)((double)args[0]) : std::time_t(0);
	std::time_t t = std::time(&v);
	switch ( std::localtime(&t) -> tm_wday ) {
		case 0: return "Sun";
		case 1: return "Mon";
		case 2: return "Tue";
		case 3: return "Wed";
		case 4: return "Thu";
		case 5: return "Fri";
		case 6: return "Sat";
		default: return "Unk";
	}
}

expr::FUNCTIONMAP expr::functions::common = {
	{ "time::hour", expr::functions::time_hour },
	{ "time::min", expr::functions::time_min },
	{ "time::sec", expr::functions::time_sec },

	{ "date::day", expr::functions::date_day },
	{ "date::month", expr::functions::date_month },
	{ "date::year", expr::functions::date_year },
	{ "date::weekday", expr::functions::date_weekday },
	{ "date::day::name", expr::functions::date_day_name },

};

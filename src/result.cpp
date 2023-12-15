#include <utility>
#include "logger.hpp"
#include "expr/token.hpp"
#include "expr/result.hpp"

static std::string typedesc(const expr::TOKEN& t) {
	switch ( t.type()) {
                case expr::T_UNDEF: return "undefined";
                case expr::T_NUMBER: return "number";
                case expr::T_STRING: return "string";
                case expr::T_OPERATOR: return "operator";
                case expr::T_VARIABLE: return "variable";
                case expr::T_FUNCTION: return "function";
                case expr::T_SUB: return "parentheses";
                case expr::T_CONDITIONAL: return "conditional";
	}

	return "unknown";
}

expr::RESULT& expr::RESULT::operator=(const expr::TOKEN& t) {

	if ( t == expr::T_NUMBER ) this -> emplace<double>(std::forward<decltype(t.to_double())>(t.to_double()));
	else if ( t == expr::T_STRING ) this -> emplace<std::string>(std::forward<decltype(t.to_string())>(t.to_string()));
	else if ( t == expr::T_UNDEF ) this -> emplace<std::nullptr_t>(std::forward<decltype(nullptr)>(nullptr));
	else {
		logger::verbose << logger::tag("evaluate") <<
			"cannot get result from " <<
			typedesc(t) << " token" << std::endl;
		logger::vverbose << logger::tag("evaluate") <<
			"using null result instead" << std::endl;

		this -> emplace<std::nullptr_t>(std::forward<decltype(nullptr)>(nullptr));
	}
	return *this;
}

expr::RESULT& expr::RESULT::operator=(const expr::VARIABLE& v) {

	if ( v.is_string()) this -> emplace<std::string>(std::forward<decltype(v.to_string())>(v.to_string()));
	else if ( v.is_number()) this -> emplace<double>(std::forward<decltype(v.to_double())>(v.to_double()));
	else this -> emplace<std::nullptr_t>(std::forward<decltype(nullptr)>(nullptr));
	return *this;
}

expr::RESULT::RESULT() {

	this -> emplace<std::nullptr_t>(std::forward<decltype(nullptr)>(nullptr));
}

expr::RESULT::RESULT(const expr::TOKEN& t) {

	if ( t == expr::T_NUMBER ) this -> emplace<double>(std::forward<decltype(t.to_double())>(t.to_double()));
	else if ( t == expr::T_STRING ) this -> emplace<std::string>(std::forward<decltype(t.to_string())>(t.to_string()));
	else if ( t == expr::T_UNDEF ) this -> emplace<std::nullptr_t>(std::forward<decltype(nullptr)>(nullptr));
	else {
		logger::verbose << logger::tag("evaluate") <<
			"cannot get result from " <<
			typedesc(t) << " token" << std::endl;
		logger::vverbose << logger::tag("evaluate") <<
			"using null result instead" << std::endl;

		this -> emplace<std::nullptr_t>(std::forward<decltype(nullptr)>(nullptr));
	}
}

expr::RESULT::RESULT(const expr::VARIABLE& v) {

	if ( v.is_string()) this -> emplace<std::string>(std::forward<decltype(v.to_string())>(v.to_string()));
	else if ( v.is_number()) this -> emplace<double>(std::forward<decltype(v.to_double())>(v.to_double()));
	else this -> emplace<std::nullptr_t>(std::forward<decltype(nullptr)>(nullptr));
}

const std::string describe(const expr::RESULT& r); {
	return r.describe();
}

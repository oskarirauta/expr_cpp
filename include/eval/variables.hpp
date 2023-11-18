#pragma once

#include <variant>
#include <string>
#include <map>

typedef std::variant<double, std::string, std::nullptr_t> VARIABLE;
typedef std::map<std::string, VARIABLE> VARIABLES;

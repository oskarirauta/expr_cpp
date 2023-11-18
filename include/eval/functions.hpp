#pragma once

#include <functional>
#include <variant>
#include <string>
#include <vector>
#include <map>

typedef std::variant<double, std::string, std::nullptr_t> FUNCTION_ARG;
typedef std::vector<FUNCTION_ARG> FUNCTION_ARGS;
typedef std::function<FUNCTION_ARG(const std::vector<FUNCTION_ARG>&)> FUNCTION;
typedef std::map<std::string, FUNCTION> FUNCTIONS;

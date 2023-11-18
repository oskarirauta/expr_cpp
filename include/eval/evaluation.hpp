#pragma once

#include <string>
#include <vector>

#include "eval/variables.hpp"
#include "eval/functions.hpp"
#include "eval/token.hpp"

TOKEN evaluate(std::vector<TOKEN>& tokens, const FUNCTIONS *functions = nullptr, VARIABLES *variables = nullptr);
TOKEN evaluate(const std::string& s, const FUNCTIONS *functions = nullptr, VARIABLES *variables = nullptr);

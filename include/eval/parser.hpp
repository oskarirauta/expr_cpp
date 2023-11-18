#pragma once

#include <vector>

#include "eval/token.hpp"

std::vector<TOKEN> parse(const std::string& expr);
const std::string describe(const std::vector<TOKEN>& tokens);

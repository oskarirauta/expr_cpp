[![License:MIT](https://img.shields.io/badge/License-MIT-blue?style=plastic)](LICENSE)
[![C++ CI build](https://github.com/oskarirauta/expr_cpp/actions/workflows/build.yml/badge.svg)](https://github.com/oskarirauta/expr_cpp/actions/workflows/build.yml)

# expr_cpp
expression evaluation library for C++

### Description

More or less, a modern c++ version of expression evaluator used
in lcd4linux. One could say that this is a fork, but it isn't
as it's completely hand-written from scratch just following
original's idea and features. But yet, for original work I do
give my respects for author of expression module in lcd4linux,
purely genious even when it's pretty old. Haven't yet found
anything with as good idea as it had.

I am attempting to make a new version of lcd4linux (name
to be decided later, currently name just is lcd2) - though
it first will only support DPF driver, as I only have
display of that type and plan to only support graphic
displays. On other hand, this version will be more versatile
and feature rich than original, and on other hand - it is
possible that some features will be left out as well or
some behaviour changes should be expected.

This will be written as a git submodule that can be easily
included with other projects. On it's own, this is useful
mostly for demonstration purposes.

### Usage

```cpp
#include <iostream>
#include "expr/expression.hpp"

// a custom function: sums its numeric arguments
expr::VARIABLE sum(const expr::FUNCTION_ARGS& args) {
    double total = 0;
    for ( const expr::VARIABLE& arg : args )
        if ( arg == expr::V_NUMBER )
            total += (double)arg;
    return total;
}

int main() {

    expr::VARIABLEMAP variables = { { "name", "OpenWrt" }, { "width", (double)128 } };
    expr::FUNCTIONMAP functions = { { "sum", sum } };

    expr::expression e("'hi, ' . name . ' (' . to_string(sum(width, 2)) . ')'");
    expr::RESULT result = e.evaluate(&functions, &variables);

    std::cout << result << std::endl;   // hi, OpenWrt (130)
    return 0;
}
```

Expressions support numbers and strings, arithmetic (`+ - * / %`), comparison
(`== != < > <= >=`), logical (`&& || !`), the ternary `?:`, string concatenation
(`.`), variables, custom functions and a set of built-ins (`to_string`, `time`,
`strftime`, `date::*`, `time::*`, `pi`, ...). See [`main.cpp`](main.cpp) for more.

### dependencies
 - gcc 13 or newer and c++ standard c++20 or newer, c++23 recommended
 - common: [common_cpp](https://github.com/oskarirauta/common_cpp.git)
 - logger: [logger_cpp](https://github.com/oskarirauta/logger_cpp.git)

### development status
on-going; but works pretty nicely and is usable.

### license
MIT

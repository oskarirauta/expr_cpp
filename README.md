# expr_cpp
expression evalution library for C++

[![C++ CI build](https://github.com/oskarirauta/expr_cpp/actions/workflows/build.yml/badge.svg)](https://github.com/oskarirauta/expr_cpp/actions/workflows/build.yml)

### Requirement
Relatively new gcc/g++ version is required for building, meaning that
minimum version to build succesfully is gcc-13. Best results when
built with parameter ```--std=c++23```

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

### depencies
 - gcc 13 or newer and c++ standard c++20 or newer, c++23 recommended
 - common: [https://github.com/oskarirauta/common_cpp.git](https://github.com/oskarirauta/common_cpp)
 - logger: [https://github.com/oskarirauta/logcpp.git](https://github.com/oskarirauta/logcpp)

### development status
on-going; but works pretty nicely and is usable.

### license
MIT

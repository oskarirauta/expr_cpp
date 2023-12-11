all: world

CXX?=g++
CXXFLAGS?=--std=c++23 -Wall -fPIC -g
LDFLAGS?=-L/lib -L/usr/lib
INCLUDES+= -I.

EXPRCPP_DIR:=.
include common/Makefile.inc
include logger/Makefile.inc
include Makefile.inc

world: example

objs/main.o: main.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

example: $(COMMON_OBJS) $(LOGGER_OBJS) $(EXPR_OBJS) objs/main.o
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -L. $(LIBS) $^ -o $@;

.PHONY: clean
clean:
	rm -f objs/*.o example

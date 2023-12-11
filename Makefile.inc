EXPRCPP_DIR?=expr
INCLUDES += -I./$(EXPRCPP_DIR)/include

EXPR_OBJS:= \
	objs/expr_variable.o \
	objs/expr_token.o \
	objs/expr_token_ops.o \
	objs/expr_expression.o \
	objs/expr_parser.o \
	objs/expr_evaluate.o

objs/expr_variable.o: src/variable.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_token.o: src/token.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_token_ops.o: src/token_ops.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_expression.o: src/expression.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_parser.o: src/parser.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_evaluate.o: src/evaluate.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

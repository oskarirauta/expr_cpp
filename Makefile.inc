EXPRCPP_DIR?=expr
INCLUDES += -I./$(EXPRCPP_DIR)/include

EXPR_OBJS:= \
	objs/expr_variable.o \
	objs/expr_result.o \
	objs/expr_property.o \
	objs/expr_token.o \
	objs/expr_token_ops.o \
	objs/expr_expression.o \
	objs/expr_parser.o \
	objs/expr_evaluate.o

objs/expr_variable.o: $(EXPRCPP_DIR)/src/variable.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_result.o: $(EXPRCPP_DIR)/src/result.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_property.o: $(EXPRCPP_DIR)/src/property.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_token.o: $(EXPRCPP_DIR)/src/token.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_token_ops.o: $(EXPRCPP_DIR)/src/token_ops.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_expression.o: $(EXPRCPP_DIR)/src/expression.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_parser.o: $(EXPRCPP_DIR)/src/parser.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

objs/expr_evaluate.o: $(EXPRCPP_DIR)/src/evaluate.cpp
	$(CXX) $(CXXFLAGS) $(INCLUDES) -c -o $@ $<;

SRCDIR = src
ODIR = bin
LIBS = -ll
CFLAGS = -g -w -std=c++14
CC = g++

FLEX_FILE = minijava_lexer.ll
PARSE_FILE = minijava_parser.yy

FLEX_OUT = $(patsubst %.ll, $(SRCDIR)/%.yy.c, $(FLEX_FILE))
PARSER_OUT = $(patsubst %.yy, $(SRCDIR)/%.tab.cc, $(PARSE_FILE))
PARSER_HEADER = $(patsubst %.cc, %.hh, $(PARSER_OUT))
SRC = $(wildcard $(SRCDIR)/*.cpp)

PROGRAM_OUT = ./compiler

TEST_FOLDER = ./test_files
TEST_FILE = ./experiments/testText3.java
TEST_FILE = $(TEST_FOLDER)/syntax_errors/InvalidMethodCall2.java
TEST_FILE = $(TEST_FOLDER)/valid/Factorial.java
TEST_FILE = $(TEST_FOLDER)/valid/SemanticMethodCallInBooleanExpression.java
TEST_FILE = $(TEST_FOLDER)/semantic_errors/InvalidNestedMethodCalls.java

all: $(PROGRAM_OUT)

# Compile the program
$(PROGRAM_OUT): $(FLEX_OUT) $(PARSER_OUT) $(SRC) $(LIBS)
	$(CC) $(CFLAGS) -o $@ $^
	cp $(TEST_FILE) $(ODIR)/inputfile.java

# Compile the parser
$(PARSER_OUT): $(PARSE_FILE)
	bison -d -o $@ $(PARSE_FILE)

# Compile the lexer
$(FLEX_OUT): $(PARSER_OUT) $(FLEX_FILE) 
	flex -o $@ $(FLEX_FILE)

run: compiler
	./$(PROGRAM_OUT) $(TEST_FILE)

clean:
	rm -f $(FLEX_OUT) $(PARSER_OUT) $(PARSER_HEADER) $(ODIR)/*.o $(PROGRAM_OUT) tree.dot tree.pdf

tree: tree.dot
	dot -Tpdf tree.dot -o tree.pdf

lexical_test: all
	python3 ./testScript.py -lexical

syntax_test: all
	python3 ./testScript.py -syntax

semantic_test: all
	python3 ./testScript.py -semantic

valid_test: all
	python3 ./testScript.py -valid

test_all: compiler lexical_test syntax_test semantic_test valid_test

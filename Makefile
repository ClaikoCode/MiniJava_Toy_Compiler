SRCDIR = src
ODIR = bin
LIBS = -ll
CFLAGS = -g -w -std=c++14
CC = g++

FLEX_FILE = minijava_lexer.l
PARSE_FILE = minijava_parser.yy

FLEX_OUT = $(patsubst %.l, $(SRCDIR)/%.yy.c, $(FLEX_FILE))
PARSER_OUT = $(patsubst %.yy, $(SRCDIR)/%.tab.cc, $(PARSE_FILE))
PARSER_HEADER = $(patsubst %.cc, %.hh, $(PARSER_OUT))
SRC = $(wildcard $(SRCDIR)/*.cpp)


PROGRAM_OUT = $(ODIR)/compiler

TEST_FOLDER = ./test_files
TEST_FILE = ./experiments/testText3.java
TEST_FILE = $(TEST_FOLDER)/syntax_errors/InvalidMethodCall2.java
TEST_FILE = $(TEST_FOLDER)/valid/Factorial.java
TEST_FILE = $(TEST_FOLDER)/valid/SemanticMethodCallInBooleanExpression.java
TEST_FILE = $(TEST_FOLDER)/semantic_errors/InvalidArrayInteger.java



compiler: scanner parser
	$(CC) $(CFLAGS) -o $(PROGRAM_OUT) $(PARSER_OUT) $(FLEX_OUT) $(SRC) $(LIBS)
	cp $(TEST_FILE) $(ODIR)/inputfile.java

parser: $(PARSE_FILE)
	bison -d -o $(PARSER_OUT) $(PARSE_FILE)
	
scanner:
	flex -o $(FLEX_OUT) $(FLEX_FILE)

run: compiler
	./$(PROGRAM_OUT) < $(TEST_FILE)

clean:
	rm -f $(FLEX_OUT) $(PARSER_OUT) $(PARSER_HEADER) $(ODIR)/*.o $(PROGRAM_OUT) tree.dot tree.pdf

tree:
	dot -Tpdf tree.dot -o tree.pdf

syntax_test: compiler
	python3 ./testScript.py -syntax

all: run tree

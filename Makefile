SRCDIR = src
ODIR = bin
LIBS = -ll
CFLAGS = -g -w -std=c++17
CC = g++

FLEX_FILE = $(SRCDIR)/minijava_lexer.ll
PARSE_FILE = $(SRCDIR)/minijava_parser.yy

FLEX_OUT = $(patsubst %.ll, %.yy.c, $(FLEX_FILE))
PARSER_OUT = $(patsubst %.yy, %.tab.cc, $(PARSE_FILE))
PARSER_HEADER = $(patsubst %.cc, %.hh, $(PARSER_OUT))
SRC = $(wildcard $(SRCDIR)/*.cpp)

PROGRAM_OUT = ./compiler

TEST_FOLDER = ./test_files
TEST_FILE = ./experiments/testText3.java
TEST_FILE = $(TEST_FOLDER)/syntax_errors/InvalidMethodCall2.java
TEST_FILE = $(TEST_FOLDER)/valid/SemanticMethodCallInBooleanExpression.java
TEST_FILE = $(TEST_FOLDER)/semantic_errors/InvalidNestedMethodCalls.java
TEST_FILE = $(TEST_FOLDER)/valid/QuickSort.java
TEST_FILE = $(TEST_FOLDER)/assignment3_valid/B.java

all: $(PROGRAM_OUT)
	cp $(TEST_FILE) $(ODIR)/inputfile.java

# Compile the program
$(PROGRAM_OUT): $(FLEX_OUT) $(PARSER_OUT) $(SRC)
	$(CC) $(CFLAGS) -o $@ $^ $(LIBS)

# Compile the parser
$(PARSER_OUT): $(PARSE_FILE)
	bison -d -o $@ $(PARSE_FILE)

# Compile the lexer
$(FLEX_OUT): $(PARSER_OUT) $(FLEX_FILE) 
	flex -o $@ $(FLEX_FILE)

run: all
	$(PROGRAM_OUT) $(ODIR)/inputfile.java
	$(MAKE) tree
	$(MAKE) CFG

clean:
	rm -f $(FLEX_OUT) $(PARSER_OUT) $(PARSER_HEADER) $(ODIR)/*.o $(PROGRAM_OUT) tree.dot tree.pdf CFG.dot CFG.pdf

tree: tree.dot
	dot -Tpdf tree.dot -o tree.pdf

CFG: CFG.dot
	dot -Tpdf CFG.dot -o CFG.pdf

lexical_test: all
	python3 ./testScript.py -lexical

syntax_test: all
	python3 ./testScript.py -syntax

semantic_test: all
	python3 ./testScript.py -semantic

valid_test: all
	python3 ./testScript.py -valid

test_all: compiler lexical_test syntax_test semantic_test valid_test

SRCDIR = src
ODIR = bin
LIBS = -ll
CFLAGS = -g -w -std=c++14
CC = g++

FLEX_FILE = minijava_lexer.l
PARSE_FILE = minijava_parser.y

FLEX_OUT = $(patsubst %.l, $(SRCDIR)/%.yy.c, $(FLEX_FILE))
PARSER_OUT = $(patsubst %.y, $(SRCDIR)/%.tab.c, $(PARSE_FILE))
PARSER_HEADER = $(patsubst %.c, %.h, $(PARSER_OUT))
SRC = $(wildcard $(SRCDIR)/*.cpp)


PROGRAM_OUT = $(ODIR)/compiler

TEST_FOLDER = ./test_files
TEST_FILE = ./experiments/testText3.java
TEST_FILE = $(TEST_FOLDER)/syntax_errors/InvalidMethodCall2.java
TEST_FILE = $(TEST_FOLDER)/valid/Factorial.java


compiler: scanner parser
	$(CC) $(CFLAGS) -o $(PROGRAM_OUT) $(PARSER_OUT) $(FLEX_OUT) $(SRC) $(LIBS)

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

all: run tree

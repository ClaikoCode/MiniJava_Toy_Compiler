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

PROGRAM_OUT = $(ODIR)/compiler

TEST_FOLDER = ./test_files
TEST_FILE = $(TEST_FOLDER)/assignment3_valid/A.java

compiler: scanner parser
	$(CC) $(CFLAGS) -o $(PROGRAM_OUT) $(PARSER_OUT) $(FLEX_OUT) $(LIBS) 

parser: $(PARSE_FILE)
	bison -d -o $(PARSER_OUT) $(PARSE_FILE)

scanner:
	flex -o $(FLEX_OUT) $(FLEX_FILE)

run: compiler
	./$(PROGRAM_OUT) < $(TEST_FILE)

clean:
	rm -f $(FLEX_OUT) $(PARSER_OUT) $(PARSER_HEADER) $(ODIR)/*.o $(PROGRAM_OUT)

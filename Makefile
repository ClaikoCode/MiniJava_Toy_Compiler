LEXER_FILE_NAME = minijava_lexer
LEXER_OUTPUT_NAME = $(LEXER_FILE_NAME)

PARSER_FILE_NAME = minijava_parser
PARSER_OUTPUT_NAME = $(PARSER_FILE_NAME)

FLEX_OUTPUT_NAME = lex.yy.c
BISON_OUTPUT_NAMES = $(PARSER_FILE_NAME).tab.c $(PARSER_FILE_NAME).tab.h

TEST_NAME = testJavaCode.java
TEST_NAME = ./test_files/assignment3_valid/A.java

PROGRAM_OUTPUT_NAME = testCompiler

compiler: lexer parser
	gcc -g -w -o $(PROGRAM_OUTPUT_NAME) $(FLEX_OUTPUT_NAME) $(BISON_OUTPUT_NAMES) -ll

parser: $(PARSER_FILE_NAME).y
	bison -d $(PARSER_FILE_NAME).y

lexer: $(LEXER_FILE_NAME).l parser
	flex $(LEXER_FILE_NAME).l

run: compiler
	./$(PROGRAM_OUTPUT_NAME) < $(TEST_NAME)

clean:
	rm -f $(PROGRAM_OUTPUT_NAME) $(FLEX_OUTPUT_NAME) $(BISON_OUTPUT_NAMES)

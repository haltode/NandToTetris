#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

#define NUMBER_KEYWORD 21

#include "JackAnalyzer.h"

enum tokenType
{
	TOKEN_ERROR 	= -1,
	KEYWORD 		= 0,
	SYMBOL 		= 1,
	INT_CONST 	= 2,
	STRING_CONST 	= 3,
	IDENTIFIER 	= 4
};

enum keywordType
{
	KEYWORD_ERROR 	= -1,
	CLASS 			= 0,
	METHOD 			= 1,
	FUNCTION 			= 2,
	CONSTRUCTOR 		= 3,
	INT 				= 4,
	BOOLEAN 			= 5,
	CHAR 			= 6,
	VOID 			= 7,
	VAR 				= 8,
	STATIC 			= 9,
	FIELD 			= 10,
	LET 				= 11,
	DO 				= 12,
	IF 				= 13,
	ELSE 				= 14,
	WHILE 			= 15,
	RETURN 			= 16,
	TRUE 			= 17,
	FALSE 			= 18,
	NULL_KEYWORD 	= 19,
	THIS 				= 20
};

enum string
{
	STRING 		= 5,
	END_STRING 	= 6
};

typedef struct keyword keyword;
struct keyword
{
	const char *name;
	const int type;
};


int isKeyword(const char *string);
int isIntegerConstant(const char *string);
int isIdentifier(const char *string);
int isSymbol(const char letter);
int tokenType(void);


int hasMoreTokens(void);
void advance(char currentLine[LENGTH_MAX]);
void writeToken(FILE *tokenizerOutput, const int tokenType);
void writeCurrentToken(FILE *tokenizerOutput);


void createTokenizerOutputFileName(char *string);
void jackTokenizer(void);

#endif

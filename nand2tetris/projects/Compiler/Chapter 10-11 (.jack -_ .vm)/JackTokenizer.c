#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include "JackTokenizer.h"

FILE *tokenizerInput;
char currentToken[LENGTH_MAX];
int currentLineNumber;

/* Return the exact keyword of string */
int isKeyword(const char *string)
{
	static const keyword baseKeyword[NUMBER_KEYWORD] = 
	{
		{"class"		, CLASS},
		{"method"		, METHOD},
		{"function"	, FUNCTION},
		{"constructor"	, CONSTRUCTOR},
		{"int"		, INT},
		{"boolean"		, BOOLEAN},
		{"char"		, CHAR},
		{"void"		, VOID},
		{"var"		, VAR},
		{"static"		, STATIC},
		{"field"		, FIELD},
		{"let"		, LET},
		{"do"			, DO},
		{"if"			, IF},
		{"else"		, ELSE},
		{"while"		, WHILE},
		{"return"		, RETURN},
		{"true"		, TRUE},
		{"false"		, FALSE},
		{"null"		, NULL_KEYWORD},
		{"this"		, THIS}
	};
	int indexKeyword;

	for(indexKeyword = 0; indexKeyword < NUMBER_KEYWORD; ++indexKeyword)
	{
		if(strcmp(string, baseKeyword[indexKeyword].name) == 0)
			return baseKeyword[indexKeyword].type;
	}

	return KEYWORD_ERROR;
}

/* Return 1 if string is an integer constant and 0 else */
int isIntegerConstant(const char *string)
{
	int index;

	for(index = 0; string[index] != '\0'; ++index)
		if(!isdigit(string[index]))
			return 0;

	return 1;
}

/* Return 1 if string is an identifier and 0 else */
int isIdentifier(const char *string)
{
	int index;

	for(index = 0; string[index] != '\0'; ++index)
		if(!isalnum(string[index]) && string[index] != '_')
			return 0;

	return 1;
}

/* Return 1 if the letter is a symbol and 0 else */
int isSymbol(const char letter)
{
	if(strchr("{}()[].,;+-*/&|<>=~", letter) != NULL)
		return 1;
	else
		return 0;
}

/* Return the type of the current token (KEYWORD, SYMBOL, INT_CONST, STRING_CONST, IDENTIFIER) */
int tokenType(void)
{
	if(isKeyword(currentToken) != KEYWORD_ERROR)
		return KEYWORD;
	else if(strpbrk(currentToken, "{}()[].,;+-*/&|<>=~") != NULL)
		return SYMBOL;
	else if(isIntegerConstant(currentToken))
		return INT_CONST;
	else if(strchr(currentToken, '\"') != NULL)
		return STRING_CONST;
	else if(isIdentifier(currentToken))
		return IDENTIFIER;

	return TOKEN_ERROR;
}

/* Return 1 if there is more token in the input and 0 else */
int hasMoreTokens(void)
{
	if(feof(tokenizerInput))
		return 0;
	else
		return 1;
}

/* Read the nexte line in the input */
void advance(char currentLine[LENGTH_MAX])
{
	if(fgets(currentLine, LENGTH_MAX, tokenizerInput) != NULL)
		currentLine[strlen(currentLine) - 1] = '\0';
	else
		currentLine[0] = '\0';
}

/* Write the token tokenType in the output file tokenizerOutput */
void writeToken(FILE *tokenizerOutput, const int tokenType)
{
	/* Keep the line number of the keyword in the tokenizer output */
	fprintf(tokenizerOutput, "%d ", currentLineNumber);

	if(tokenType == KEYWORD)
		fprintf(tokenizerOutput, "<keyword> %s </keyword>\n", currentToken);
	else if(tokenType == SYMBOL)
	{
		if(currentToken[0] == '<')
			fprintf(tokenizerOutput, "<symbol> &lt; </symbol>\n");
		else if(currentToken[0] == '>')
			fprintf(tokenizerOutput, "<symbol> &gt; </symbol>\n");
		else if(currentToken[0] == '&')
			fprintf(tokenizerOutput, "<symbol> &amp; </symbol>\n");
		else
			fprintf(tokenizerOutput, "<symbol> %s </symbol>\n", currentToken);
	}
	else if(tokenType == INT_CONST)
		fprintf(tokenizerOutput, "<integerConstant> %s </integerConstant>\n", currentToken);
	else if(tokenType == STRING_CONST)
		fprintf(tokenizerOutput, "<stringConstant> %s </stringConstant>\n", currentToken);
	else if(tokenType == IDENTIFIER)
		fprintf(tokenizerOutput, "<identifier> %s </identifier>\n", currentToken);
}

/* Get the current token and write it in the output file tokenizerOutput */
void writeCurrentToken(FILE *tokenizerOutput)
{
	char currentLine[LENGTH_MAX];
	char buffer[LENGTH_MAX];
	char lineNumber[LENGTH_MAX];
	int indexLine, indexBuffer;
	int lastTokenType;

	advance(currentLine);

	if(currentLine[0] == '\0')
		return ;

	/* Get the line number */
	for(indexLine = 0; isdigit(currentLine[indexLine]) && currentLine[indexLine] != '\0'; ++indexLine)
		lineNumber[indexLine] = currentLine[indexLine];

	lineNumber[indexLine] = '\0';
	currentLineNumber = atoi(lineNumber);

	/* Skip the space character */
	++indexLine;

	for(indexBuffer = 0, lastTokenType = TOKEN_ERROR; currentLine[indexLine] != '\0'; ++indexLine)
	{
		/* If it is a character different than an alpha or a digit AND the last token type was a string (If this is the end of a string) */
		if(!isalnum(currentLine[indexLine]) && lastTokenType == STRING)
		{
			buffer[indexBuffer] = '\0';

			/* Copy the string in the current token */
			strcpy(currentToken, buffer);

			/* Determine if the string is a keyword, an integer constant or a identifier */
			if(isKeyword(currentToken) != KEYWORD_ERROR)
				writeToken(tokenizerOutput, KEYWORD);
			else if(isIntegerConstant(currentToken))
				writeToken(tokenizerOutput, INT_CONST);
			else
				writeToken(tokenizerOutput, IDENTIFIER);

			indexBuffer = 0;
			lastTokenType = END_STRING;
		}
		/* If the current character is not a space */
		if(currentLine[indexLine] != ' ')
		{
			/* If it is a symbol */
			if(isSymbol(currentLine[indexLine]))
			{
				currentToken[0] = currentLine[indexLine];
				currentToken[1] = '\0';

				writeToken(tokenizerOutput, SYMBOL);
				lastTokenType = SYMBOL;
			}
			/* If it is a string constant */
			else if(currentLine[indexLine] == '\"')
			{
				int indexString, indexToken;

				/* Get the string starting after the first " and finishing before the second " */
				for(indexString = indexLine + 1, indexToken = 0; currentLine[indexString] != '\"'; ++indexString, ++indexToken)
					currentToken[indexToken] = currentLine[indexString];

				currentToken[indexToken] = '\0';

				/* Skip the last " */
				indexLine = indexString;

				writeToken(tokenizerOutput, STRING_CONST);
				lastTokenType = STRING_CONST;
			}
			/* Else it is a character (part of a string : keyword, integer constant or identifier) */
			else
			{
				/* Write the character in the buffer */
				buffer[indexBuffer] = currentLine[indexLine];
				++indexBuffer;

				lastTokenType = STRING;
			}
		}
	}

	if(indexBuffer != 0)
	{
		/* Special case : if there is a word at the end of the line like : "class Main" or "else" */
		buffer[indexBuffer] = '\0';

		/* Copy the string in the current token */
		strcpy(currentToken, buffer);

		/* Determine if the string is a keyword, an integer constant or a identifier */
		if(isKeyword(currentToken) != KEYWORD_ERROR)
			writeToken(tokenizerOutput, KEYWORD);
		else
			writeToken(tokenizerOutput, IDENTIFIER);
	}
}

/* Create the name of the tokenizer output (xT.xml) */
void createTokenizerOutputFileName(char *string)
{
	int length;

	length = strlen(string);
	string[length - 5] = 'T';
	string[length - 4] = '.';
	string[length - 3] = 'x';
	string[length - 2] = 'm';
	string[length - 1] = 'l';
	string[length] = '\0';
}

/* Tokenize the current Jack file */
void jackTokenizer(void)
{
	FILE *tokenizerOutput;
	char path[LENGTH_MAX];

	strcpy(path, currentFileName);
	createTokenizerOutputFileName(path);
	strcat(path, "_clean.xml");
	tokenizerInput = fopen(path, "r");

	strcpy(path, currentFileName);
	createTokenizerOutputFileName(path);
	tokenizerOutput = fopen(path, "w+");

	/* Write each token in the tokenizer output */

	//fprintf(tokenizerOutput, "<tokens>\n");

	currentLineNumber = 0;
	while(hasMoreTokens())
		writeCurrentToken(tokenizerOutput);

	//fprintf(tokenizerOutput, "</tokens>\n");

	fclose(tokenizerInput);
	fclose(tokenizerOutput);

	/* Remove the X_clean.xml file (because the tokenizer output is in the file xT.xml) */
	//remove(path);
}

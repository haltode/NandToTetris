#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "SymbolTable.h"

void ConstructorSymbolTable(void)
{
	/* Create a new symbol table */

	symbolTable.indexClassScope = 0;
	symbolTable.indexSubroutineScope = 0;

	symbolTable.indexClass = 0;
	symbolTable.indexSubroutine = 0;
	symbolTable.indexStatic = 0;
	symbolTable.indexField = 0;
	symbolTable.indexArgument = 0;
	symbolTable.indexVar = 0;

	symbolTable.currentScope = NONE;
}

void startSubroutine(void)
{
	/* Each time there is a new subroutine, we reinit the subroutine scope symbol table */

	symbolTable.indexSubroutineScope = 0;

	symbolTable.indexArgument = 0;
	symbolTable.indexVar = 0;
}

void DefineJackStandardLibrarySubroutine(void)
{
	/* Math.jack */
	Define("abs", "function", SUBROUTINE_KIND);
	Define("multiply", "function", SUBROUTINE_KIND);
	Define("divide", "function", SUBROUTINE_KIND);
	Define("min", "function", SUBROUTINE_KIND);
	Define("max", "function", SUBROUTINE_KIND);
	Define("sqrt", "function", SUBROUTINE_KIND);
	/* String.jack */
	Define("new", "constructor", SUBROUTINE_KIND);
	Define("dispose", "method", SUBROUTINE_KIND);
	Define("length", "method", SUBROUTINE_KIND);
	Define("charAt", "method", SUBROUTINE_KIND);
	Define("setCharAt", "method", SUBROUTINE_KIND);
	Define("appendChar", "method", SUBROUTINE_KIND);
	Define("eraseLastChar", "method", SUBROUTINE_KIND);
	Define("intValue", "method", SUBROUTINE_KIND);
	Define("setInt", "method", SUBROUTINE_KIND);
	Define("backSpace", "function", SUBROUTINE_KIND);
	Define("doubleQuote", "function", SUBROUTINE_KIND);
	Define("newLine", "function", SUBROUTINE_KIND);
	/* Array.jack */
	Define("new", "function", SUBROUTINE_KIND);
	Define("dispose", "method", SUBROUTINE_KIND);
	/* Output.jack */
	Define("moveCursor", "function", SUBROUTINE_KIND);
	Define("printChar", "function", SUBROUTINE_KIND);
	Define("printString", "function", SUBROUTINE_KIND);
	Define("printInt", "function", SUBROUTINE_KIND);
	Define("println", "function", SUBROUTINE_KIND);
	Define("backSpace", "function", SUBROUTINE_KIND);
	/* Screen.jack */
	Define("clearScreen", "function", SUBROUTINE_KIND);
	Define("setColor", "function", SUBROUTINE_KIND);
	Define("drawPixel", "function", SUBROUTINE_KIND);
	Define("drawLine", "function", SUBROUTINE_KIND);
	Define("drawRectangle", "function", SUBROUTINE_KIND);
	Define("drawCircle", "function", SUBROUTINE_KIND);
	/* Keyboard.jack */
	Define("keyPressed", "function", SUBROUTINE_KIND);
	Define("readChar", "function", SUBROUTINE_KIND);
	Define("readLine", "function", SUBROUTINE_KIND);
	Define("readInt", "function", SUBROUTINE_KIND);
	/* Memory.jack */
	Define("peek", "function", SUBROUTINE_KIND);
	Define("poke", "function", SUBROUTINE_KIND);
	Define("alloc", "function", SUBROUTINE_KIND);
	Define("deAlloc", "function", SUBROUTINE_KIND);
	/* Sys.jack */
	Define("halt", "function", SUBROUTINE_KIND);
	Define("error", "function", SUBROUTINE_KIND);
	Define("wait", "function", SUBROUTINE_KIND);
}

void DefineClassDec(const  char *line)
{
	/* class : 'class' className '{' classVarDec* subroutineDec* '}' */

	char className[LENGTH_MAX];
	int indexLine, indexName;

	/* Here indexLine is init to 6 (CLASS_KEYWORD_LENGTH + 1) because we skip the keyword : "class" and the space character (6 letters in total) */
	for(indexLine = CLASS_KEYWORD_LENGTH + 1, indexName = 0; line[indexLine] != ' ' && !iscntrl(line[indexLine]); ++indexLine, ++indexName)
		className[indexName] = line[indexLine];

	className[indexName] = '\0';

	/* Define the class */
	Define(className, "class", CLASS_KIND);
}

void DefineClassVarDec(const char *line, const int varKind)
{
	/* classVarDec : ('static' | 'field') type varName (',' varName)* ';' */

	char varName[LENGTH_MAX];
	char varType[LENGTH_MAX];
	int indexLine, indexType, indexName;

	/* Here indexLine is init to STATIC_KEYWORD_LENGTH + 1 or FIELD_KEYWORD_LENGTH + 1
	(it depend on the variable kind : static or field) because we skip the keyword : "static" or 
	"field" and the space character */
	if(varKind == STATIC_KIND)
		indexLine = STATIC_KEYWORD_LENGTH + 1;
	else if(varKind == FIELD_KIND)
		indexLine = FIELD_KEYWORD_LENGTH + 1;

	/* Get the variable type */
	for(indexType = 0; line[indexLine] != ' '; ++indexType, ++indexLine)
		varType[indexType] = line[indexLine];

	varType[indexType] = '\0';

	/* We skip the space character */
	++indexLine;

	/* Get the variable name */
	for(indexName = 0; isalnum(line[indexLine]) || line[indexLine] == '_'; ++indexName, ++indexLine)
		varName[indexName] = line[indexLine];

	varName[indexName] = '\0';

	/* Define a new variable with the name, type, kind */
	Define(varName, varType, varKind);

	while(line[indexLine] == ',')
	{
		/* Skip the ', and the space character */
		indexLine += 2;

		/* Get the variable name */
		for(indexName = 0; isalnum(line[indexLine]) || line[indexLine] == '_'; ++indexName, ++indexLine)
			varName[indexName] = line[indexLine];

		varName[indexName] = '\0';

		/* Define a new variable with the name, type, kind */
		Define(varName, varType, varKind);
	}
}

void DefineSubroutineDec(const char *line, const int type)
{
	/* subroutineDec : ('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody */

	char subroutineName[LENGTH_MAX];
	char subroutineType[LENGTH_MAX];
	int indexLine, indexName;

	/* Get the subroutine type, and skip the keyword "function" or "constructor" or "method" and the space character */
	switch(type)
	{
		case FUNCTION_SUB :
			strcpy(subroutineType, "function");
			indexLine = FUNCTION_KEYWORD_LENGTH + 1;
		break;

		case CONSTRUCTOR_SUB :
			strcpy(subroutineType, "constructor");
			indexLine = CONSTRUCTOR_KEYWORD_LENGTH + 1;
		break;

		case METHOD_SUB :
			strcpy(subroutineType, "method");
			indexLine = METHOD_KEYWORD_LENGTH + 1;
		break;
	}

	/* We skip the type of the subroutine */
	for(; isalnum(line[indexLine]) || line[indexLine] == '_'; ++indexLine)
		;

	/* We skip the space character */
	++indexLine;

	/* Here indexLine is init to 6 because we skip the keyword : "class" and the space character (6 letters in total) */
	for(indexName = 0; isalnum(line[indexLine]) || line[indexLine] == '_'; ++indexLine, ++indexName)
		subroutineName[indexName] = line[indexLine];

	subroutineName[indexName] = '\0';

	/* Define the subroutine */
	Define(subroutineName, subroutineType, SUBROUTINE_KIND);
}

void Define(const char *name, const char *type, const int kind)
{
	/* If we are in a class scope */
	if(symbolTable.currentScope == CLASS_SCOPE)
	{	
		/* Copy the name, type, kind */
		strcpy(symbolTable.classScope[symbolTable.indexClassScope].name, name);
		strcpy(symbolTable.classScope[symbolTable.indexClassScope].type, type);
		symbolTable.classScope[symbolTable.indexClassScope].kind = kind;

		/* Get the index */
		switch(kind)
		{
			case CLASS_KIND :
				symbolTable.classScope[symbolTable.indexClassScope].index = symbolTable.indexClass;
				++(symbolTable.indexClass);
			break;

			case STATIC_KIND :
				symbolTable.classScope[symbolTable.indexClassScope].index = symbolTable.indexStatic;
				++(symbolTable.indexStatic);
			break;

			case FIELD_KIND :
				symbolTable.classScope[symbolTable.indexClassScope].index = symbolTable.indexField;
				++(symbolTable.indexField);
			break;

			case SUBROUTINE_KIND :
				symbolTable.classScope[symbolTable.indexClassScope].index = symbolTable.indexSubroutine;
				++(symbolTable.indexSubroutine);
			break;
		}
	
		++(symbolTable.indexClassScope);
	}
	/* If we are in a subroutine scope */
	else if(symbolTable.currentScope == SUBROUTINE_SCOPE)
	{
		/* Copy the name, type, kind */
		strcpy(symbolTable.subroutineScope[symbolTable.indexSubroutineScope].name, name);
		strcpy(symbolTable.subroutineScope[symbolTable.indexSubroutineScope].type, type);
		symbolTable.subroutineScope[symbolTable.indexSubroutineScope].kind = kind;

		/* Get the index */
		switch(kind)
		{
			case ARG_KIND :
				symbolTable.subroutineScope[symbolTable.indexSubroutineScope].index = symbolTable.indexArgument;
				++(symbolTable.indexArgument);
			break;

			case VAR_KIND :
				symbolTable.subroutineScope[symbolTable.indexSubroutineScope].index = symbolTable.indexVar;
				++(symbolTable.indexVar);
			break;
		}

		++(symbolTable.indexSubroutineScope);
	}
}

int VarCount(const int kind)
{
	/* Return the index corresponding to the kind */

	switch(kind)
	{
		case CLASS_KIND :
			return symbolTable.indexClass;
		break;

		case STATIC_KIND :
			return symbolTable.indexStatic;
		break;

		case FIELD_KIND :
			return symbolTable.indexField;
		break;

		case SUBROUTINE_KIND :
			return symbolTable.indexSubroutine;
		break;

		case ARG_KIND :
			return symbolTable.indexArgument;
		break;

		case VAR_KIND :
			return symbolTable.indexVar;
		break;
	}

	return NONE;
}

int KindOf(const char *name)
{
	/* Return the kind corresponding to the name */

	int indexTable;

	for(indexTable = 0; indexTable < symbolTable.indexSubroutineScope; ++indexTable)
		if(strcmp(symbolTable.subroutineScope[indexTable].name, name) == 0)
			return symbolTable.subroutineScope[indexTable].kind;

	for(indexTable = 0; indexTable < symbolTable.indexClassScope; ++indexTable)
		if(strcmp(symbolTable.classScope[indexTable].name, name) == 0)
			return symbolTable.classScope[indexTable].kind;

	return NONE;
}

void NameOf(const char *type, char *name)
{
	/* Return the name corresponding to the type */

	int indexTable;

	for(indexTable = 0; indexTable < symbolTable.indexSubroutineScope; ++indexTable)
	{
		if(strcmp(symbolTable.subroutineScope[indexTable].type, type) == 0)
		{
			strcpy(name, symbolTable.subroutineScope[indexTable].name);
			return ;
		}
	}

	for(indexTable = 0; indexTable < symbolTable.indexClassScope; ++indexTable)
	{
		if(strcmp(symbolTable.classScope[indexTable].type, type) == 0)
		{
			strcpy(name, symbolTable.classScope[indexTable].name);
			return ;
		}
	}

	name[0] = '\0';
}

void TypeOf(const char *name, char *type)
{
	/* Return the type corresponding to the name */

	int indexTable;

	for(indexTable = 0; indexTable < symbolTable.indexSubroutineScope; ++indexTable)
	{
		if(strcmp(symbolTable.subroutineScope[indexTable].name, name) == 0)
		{
			strcpy(type, symbolTable.subroutineScope[indexTable].type);
			return ;
		}
	}

	for(indexTable = 0; indexTable < symbolTable.indexClassScope; ++indexTable)
	{
		if(strcmp(symbolTable.classScope[indexTable].name, name) == 0)
		{
			strcpy(type, symbolTable.classScope[indexTable].type);
			return ;
		}
	}

	type[0] = '\0';
}

int IndexOf(const char *name)
{
	/* Return the index corresponding to the name */
	
	int indexTable;

	for(indexTable = 0; indexTable < symbolTable.indexSubroutineScope; ++indexTable)
		if(strcmp(symbolTable.subroutineScope[indexTable].name, name) == 0)
			return symbolTable.subroutineScope[indexTable].index;

	for(indexTable = 0; indexTable < symbolTable.indexClassScope; ++indexTable)
		if(strcmp(symbolTable.classScope[indexTable].name, name) == 0)
			return symbolTable.classScope[indexTable].index;

	return NONE;
}

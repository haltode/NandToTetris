#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include "JackAnalyzer.h"

#define SYMBOL_TABLE_LENGTH 256

#define CLASS_KEYWORD_LENGTH 5
#define STATIC_KEYWORD_LENGTH 6
#define FIELD_KEYWORD_LENGTH 5
#define FUNCTION_KEYWORD_LENGTH 8
#define CONSTRUCTOR_KEYWORD_LENGTH 11
#define METHOD_KEYWORD_LENGTH 6

enum subroutine
{
	FUNCTION_SUB 		= 0,
	CONSTRUCTOR_SUB 	= 1,
	METHOD_SUB 		= 2
};

enum identifierKind
{
	CLASS_KIND 		= 0,
	STATIC_KIND 		= 1,
	FIELD_KIND 		= 2,
	SUBROUTINE_KIND 	= 3,
	ARG_KIND 			= 4,
	VAR_KIND 			= 5,
	NONE	 		= 6
};

enum scopeKind
{
	CLASS_SCOPE 		= 0,
	SUBROUTINE_SCOPE 	= 1
};

typedef struct HashTable HashTable;
struct HashTable
{
	char name[LENGTH_MAX];
	char type[LENGTH_MAX];
	int kind;
	int index;
};

typedef struct SymbolTable SymbolTable;
struct SymbolTable
{
	HashTable classScope[SYMBOL_TABLE_LENGTH];
	HashTable subroutineScope[SYMBOL_TABLE_LENGTH];

	int indexClassScope;
	int indexSubroutineScope;

	int indexClass;
	int indexStatic;
	int indexField;
	int indexSubroutine;

	
	int indexArgument;
	int indexVar;

	int currentScope;
};

SymbolTable symbolTable;


void ConstructorSymbolTable(void);
void startSubroutine(void);


void DefineJackStandardLibrarySubroutine(void);
void DefineClassDec(const  char *line);
void DefineClassVarDec(const char *line, const int varKind);
void DefineSubroutineDec(const char *line, const int type);
void Define(const char *name, const char *type, const int kind);


int VarCount(const int kind);
int KindOf(const char *name);
void NameOf(const char *type, char *name);
void TypeOf(const char *name, char *type);
int IndexOf(const char *name);

#endif

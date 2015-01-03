#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "JackAnalyzer.h"

#define NUMBER_SPACE_INDENTATION 2

#define NUMBER_STATEMENT 	5
#define NUMBER_OP			9

char currentToken[LENGTH_MAX];
char currentClassName[LENGTH_MAX];
char currentSubroutineName[LENGTH_MAX];


void error(const char *errorMessage);


void readWriteTerminalElement(const int type, const char *model);
void writeTerminalElement(const int type, const char *model);
void printIndentation(void);
void writeNonTerminalElementBeginning(const char *name);
void writeNonTerminalElementEnd(const char *name);


void compileClass(void);
void compileClassVarDec(void);


void compileSubroutine(void);
void compileParameterList(void);
void compileSubroutineBody(const char *subroutineType);
int compileVarDec(void);


int isCurrentTokenStatement(void);
void compileStatements(void);
int isType(const char *token);
void compileLet(void);
void compileIf(void);
void compileWhile(void);
void compileDo(void);
void compileReturn(void);


int isCurrentTokenOp(void);
void compileExpression(void);
int isCurrentTokenUnaryOp(void);
void compileTerm(void);
int compileExpressionList(void);


void createCompilationEngineOutputFileName(char *string);
void writeCompilationEngineOutput(void);
void readCompilationEngineInput(void);
void compilationEngine(void);

#endif

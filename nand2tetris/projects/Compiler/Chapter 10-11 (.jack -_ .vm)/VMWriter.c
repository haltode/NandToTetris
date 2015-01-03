#include <stdio.h>
#include <string.h>
#include "VMWriter.h"

#include "JackAnalyzer.h"
#include "SymbolTable.h"
#include "CompilationEngine.h"
#include "JackTokenizer.h"

void writeSegment(const int segment)
{
	/* Write the segment (VM code) */

	switch(segment)
	{
		case CONST_SEGMENT :
			fprintf(vmFile, "constant");
		break;

		case ARG_SEGMENT :
			fprintf(vmFile, "argument");
		break;

		case LOCAL_SEGMENT :
			fprintf(vmFile, "local");
		break;

		case STATIC_SEGMENT :
			fprintf(vmFile, "static");
		break;

		case THIS_SEGMENT :
			fprintf(vmFile, "this");
		break;

		case THAT_SEGMENT :
			fprintf(vmFile, "that");
		break;

		case POINTER_SEGMENT :
			fprintf(vmFile, "pointer");
		break;

		case TEMP_SEGMENT :
			fprintf(vmFile, "temp");
		break;
	}
}

void writePush(const int segment, const int index)
{
	/* Write a push command in segment index (VM code) */

	fprintf(vmFile, "push ");
	writeSegment(segment);
	fprintf(vmFile, " %d\n", index);
}

void writePop(const int segment, const int index)
{
	/* Write a pop command in segment index (VM code) */

	fprintf(vmFile, "pop ");
	writeSegment(segment);
	fprintf(vmFile, " %d\n", index);
}

void writeArithmetic(const int command)
{
	/* Write an arithmetic command (VM code) */

	switch(command)
	{
		case ADD :
			fprintf(vmFile, "add");
		break;

		case SUB :
			fprintf(vmFile, "sub");
		break;

		case NEG :
			fprintf(vmFile, "neg");
		break;

		case EQ :
			fprintf(vmFile, "eq");
		break;

		case GT :
			fprintf(vmFile, "gt");
		break;

		case LT :
			fprintf(vmFile, "lt");
		break;

		case AND :
			fprintf(vmFile, "and");
		break;

		case OR :
			fprintf(vmFile, "or");
		break;

		case NOT :
			fprintf(vmFile, "not");
		break;
	}

	fprintf(vmFile, "\n");
}

void writeLabel(const char *label)
{
	/* Write a label command (VM code) */

	fprintf(vmFile, "label %s\n", label);
}

void writeGoto(const char *label)
{
	/* Write a goto command (VM code) */

	fprintf(vmFile, "goto %s\n", label);
}

void writeIf(const char *label)
{
	/* Write a if-goto command (VM code) */

	fprintf(vmFile, "if-goto %s\n", label);
}

void writeCall(const char *name, const int nArgs)
{
	/* Write a call command (VM code) */

	fprintf(vmFile, "call %s %d\n", name, nArgs);
}

void writeFunction(const char *name, const int nLocals)
{
	/* Write a function command (VM code) */

	fprintf(vmFile, "function %s %d\n", name, nLocals);
}

void writeReturn(void)
{
	/* Write a return command (VM code) */

	fprintf(vmFile, "return\n");
}

void printOp(const char *op)
{
	/* Write the op (VM code) */

	if(strcmp(op, "+") == 0)
		writeArithmetic(ADD);
	else if(strcmp(op, "-") == 0)
		writeArithmetic(SUB);
	else if(strcmp(op, "*") == 0)
		writeCall("Math.multiply", 2);
	else if(strcmp(op, "/") == 0)
		writeCall("Math.divide", 2);
	else if(strcmp(op, "=") == 0)
		writeArithmetic(EQ);
	else if(strcmp(op, "&lt;") == 0)
		writeArithmetic(LT);
	else if(strcmp(op, "&gt;") == 0)
		writeArithmetic(GT);
	else if(strcmp(op, "&amp;") == 0)
		writeArithmetic(AND);
	else if(strcmp(op, "|") == 0)
		writeArithmetic(OR);
}

void printUnaryOp(const char *op)
{
	/* Write the unary op (VM code) */

	if(strcmp(op, "-") == 0)
		writeArithmetic(NEG);
	else if(strcmp(op, "~") == 0)
		writeArithmetic(NOT);
}

int getSegmentFromKind(const int kind)
{
	/* Return the segment corresponding to the kind */

	switch(kind)
	{
		case STATIC_KIND :
			return STATIC_SEGMENT;
		break;

		case FIELD_KIND :
			return THIS_SEGMENT;
		break;

		case ARG_KIND :
			return ARG_SEGMENT;
		break;

		case VAR_KIND :
			return LOCAL_SEGMENT;
		break;
	}

	return NONE;
}

void writeAllocateMemoryBlockConstructor(void)
{
	int nbVar;

	/* Get the number of field variable in the class */
	nbVar = symbolTable.indexField;
	/* Push the number on the stack */
	writePush(CONST_SEGMENT, nbVar);
	/* Call the function Memory.alloc */
	writeCall("Memory.alloc", 1);
	/* Pop the address */
	writePop(POINTER_SEGMENT, 0);
}

void writeSetObjectInMemoryMethod(void)
{
	/* The first argument in a method subroutine is the object */

	/* Push the object */
	writePush(ARG_SEGMENT, 0);
	/* Get the this pointer on the object's field variable */
	writePop(POINTER_SEGMENT, 0);
}

void writeArrayMemory(char *varName, char *subroutineName)
{
	/* If we are in a method subroutine and the variable is an argument */
	TypeOf(currentSubroutineName, subroutineName);
	if(strcmp(subroutineName, "method") == 0 && KindOf(varName) == ARG_KIND)
		/* We have to push the variable with his index + 1 (because the first argument is always the object in a method) */
		writePush(getSegmentFromKind(KindOf(varName)), IndexOf(varName) + 1);
	else
		/* We just push the variable */
		writePush(getSegmentFromKind(KindOf(varName)), IndexOf(varName));
	/* Add the variable and the expression (to get the address where the result should go) */
	writeArithmetic(ADD);
	/* Store the address in the TEMP segment */
	writePop(TEMP_SEGMENT, 3);
}

void writeArrayAllocation(void)
{
	/* Store the result of the expression in the TEMP segment */
	writePop(TEMP_SEGMENT, 4);
	/* Restore the address */
	writePush(TEMP_SEGMENT, 3);
	/* Access to the address */
	writePop(POINTER_SEGMENT, 1);
	/* Restore the result */
	writePush(TEMP_SEGMENT, 4);
	/* Pop the result in the address */
	writePop(THAT_SEGMENT, 0);
}

void writeArrayExpression(char *name)
{
	char subroutineName[LENGTH_MAX];

	/* The variable is an array, so we add the expression in the '[' and ']' to the variable reference, then we push it */
	TypeOf(currentSubroutineName, subroutineName);
	if(strcmp(subroutineName, "method") == 0 && KindOf(name) == ARG_KIND)
		writePush(getSegmentFromKind(KindOf(name)), IndexOf(name) + 1);
	else
		writePush(getSegmentFromKind(KindOf(name)), IndexOf(name));
	writeArithmetic(ADD);
	writePop(POINTER_SEGMENT, 1);
	writePush(THAT_SEGMENT, 0);
}

void writeMethodCall(char *name)
{
	char functionName[LENGTH_MAX];
	int nbSubroutineVar;

	/* '(' */
	writeTerminalElement(SYMBOL, "(");
	/* expressionList */
	/* Push the object */
	writePush(POINTER_SEGMENT, 0);
	/* Push the expressionList and get the number of variable in it */
	nbSubroutineVar = compileExpressionList();
	/* ')' */
	readWriteTerminalElement(SYMBOL, ")");

	/* Create the method name like : className.methodName */
	sprintf(functionName, "%s.%s", currentClassName, name);
	/* Call the function with the number of variable in the expressionList + 1 (because of the object) */
	writeCall(functionName, nbSubroutineVar + 1);
}

void writeSubroutineCall(char *name)
{
	char varName[LENGTH_MAX];
	char varType[LENGTH_MAX];
	int varKind;
	int nbSubroutineVar;

	nbSubroutineVar = 0;

	/* Get the varName */
	strcpy(varName, name);
	/* Get the varType */
	TypeOf(varName, varType);
	/* Get the varKind */
	varKind = KindOf(varName);
	/* If it is not a class (so the subroutine call is method) */
	if(varKind != CLASS_KIND && varKind != NONE)
	{
		/* Push the variable (the object) */
		writePush(getSegmentFromKind(varKind), IndexOf(varName));
		strcpy(name, varType);
		/* Increment the number of subroutine varialbe in the expressionList (because of the object) */
		++nbSubroutineVar;
	}

	/* Create the subroutineCall name ((className | varName) '.' subroutineName) */
	/* '.' */
	writeTerminalElement(SYMBOL, ".");
	strcat(name, currentToken);
	/* subroutineName */
	readWriteTerminalElement(IDENTIFIER, NULL);
	strcat(name, currentToken);
	if(KindOf(currentToken) == NONE)
		error("Unknow subroutine name here.");
	/* '(' */
	readWriteTerminalElement(SYMBOL, "(");
	/* expressionList */
	nbSubroutineVar += compileExpressionList();
	/* ')' */
	readWriteTerminalElement(SYMBOL, ")");
	
	/* Call the function with nbSubourineVar argument */
	writeCall(name, nbSubroutineVar);
}

void writeStringConstant(const char *string)
{
	int lengthString;
	int indexString;

	/* If it is a stringConstant, we create a string from the function String.new and String.appendChar */

	/* Get the length of the string */
	lengthString = strlen(string);
	writePush(CONST_SEGMENT, lengthString);
	/* Create a new string of lengthString */
	writeCall("String.new", 1);

	/* Copy all the characters in the new string */
	for(indexString = 0; indexString < lengthString; ++indexString)
	{
		writePush(CONST_SEGMENT, (int)string[indexString]);
		writeCall("String.appendChar", 2);
	}
}

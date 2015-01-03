#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>
#include "CompilationEngine.h"

#include "JackTokenizer.h"
#include "SymbolTable.h"
#include "VMWriter.h"

FILE *compilationEngineInput;
FILE *compilationEngineOutput;
char currentType[LENGTH_MAX];
int indentation;
int lineNumber;

void error(const char *errorMessage)
{
	char path[LENGTH_MAX];

	printf("JackCompiler > (ERROR) in %s, line %d \"%s\" : %s\n", currentFileName, lineNumber, currentToken, errorMessage);

	/* If there is an error in the Jack code */
	/* Remove all the output files */

	/* X_clean.xml */
	strcpy(path, currentFileName);
	createTokenizerOutputFileName(path);
	strcat(path, "_clean.xml");
	remove(path);
	/* xT.xml */
	strcpy(path, currentFileName);
	createTokenizerOutputFileName(path);
	remove(path);
	/* x.xml */
	strcpy(path, currentFileName);
	createCompilationEngineOutputFileName(path);
	remove(path);
	/* x.vm */
	strcpy(path, currentFileName);
	getVMOutputFileName(path);
	remove(path);

	exit(EXIT_FAILURE);
}

void readWriteTerminalElement(const int type, const char *model)
{
	readCompilationEngineInput();
	writeTerminalElement(type, model);
}

void writeTerminalElement(const int type, const char *model)
{
	char errorMessage[LENGTH_MAX];

	/* Check if we have a model to check */
	if(type != TOKEN_ERROR)
	{
		switch(type)
		{
			case KEYWORD :
				if(model == NULL && isKeyword(currentToken) == KEYWORD_ERROR)
					error("Expecting keyword here.");
				else if(model != NULL && strcmp(currentToken, model) != 0)
				{
					sprintf(errorMessage, "Expecting the keyword : %s", model);
					error(errorMessage);
				}
			break;

			case SYMBOL :
				if(model == NULL && !isSymbol(currentToken[0]))
					error("Expecting symbol here.");
				else if(model != NULL && strcmp(currentToken, model) != 0)
				{
					sprintf(errorMessage, "Expecting the symbol : %s", model);
					error(errorMessage);
				}
			break;

			case IDENTIFIER :
				if(!isIdentifier(currentToken))
					error("Expecting identifier here.");
			break;
		}
	}

	printIndentation();
	fprintf(compilationEngineOutput, "<%s> %s </%s>\n", currentType, currentToken, currentType);
}

void printIndentation(void)
{
	int indexIndentation, indexSpace;

	for(indexIndentation = 0; indexIndentation < indentation; ++indexIndentation)
		for(indexSpace = 0; indexSpace < NUMBER_SPACE_INDENTATION; ++indexSpace)
			fprintf(compilationEngineOutput, " ");
}

void writeNonTerminalElementBeginning(const char *name)
{
	printIndentation();

	fprintf(compilationEngineOutput, "<%s>\n", name);
	++indentation;
}

void writeNonTerminalElementEnd(const char *name)
{
	--indentation;
	printIndentation();

	fprintf(compilationEngineOutput, "</%s>\n", name);
}

void compileClass(void)
{
	/* class : 'class' className '{' classVarDec* subroutineDec* '}' */
	/* classVarDec : ('static' | 'field') type varName (',' varName)* ';' */
	/* subroutineDec : ('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody */

	symbolTable.currentScope = CLASS_SCOPE;


	writeNonTerminalElementBeginning("class");

	/* 'class' */
	writeTerminalElement(KEYWORD, "class");
	/* className */
	readWriteTerminalElement(IDENTIFIER, NULL);
	strcpy(currentClassName, currentToken);
	/* '{' */
	readWriteTerminalElement(SYMBOL, "{");
	/* classVarDec* subroutineDec* '}' */
	readCompilationEngineInput();
	while(strcmp(currentToken, "}") != 0)
	{
		if(strcmp(currentToken, "static") == 0 || strcmp(currentToken, "field") == 0)
			/* classVarDec* */
			compileClassVarDec();
		else if(strcmp(currentToken, "constructor") == 0 || strcmp(currentToken, "function") == 0 || strcmp(currentToken, "method") == 0)
			/* subroutineDec* */
			compileSubroutine();
		else
			error("Expecting keyword here (class variable declaration : \"static\" or \"field\", subroutine declaration : \"constructor\" or \"function\" or \"method\").");

		readCompilationEngineInput();
	} 
	/* '}' */
	writeTerminalElement(SYMBOL, "}");

	writeNonTerminalElementEnd("class");
}

void compileClassVarDec(void)
{
	/* classVarDec : ('static' | 'field') type varName (',' varName)* ';' */


	writeNonTerminalElementBeginning("classVarDec");

	/* ('static' | 'field') has already be read so we juste print it */
	writeTerminalElement(TOKEN_ERROR, NULL);
	if(strcmp(currentToken, "static") != 0 && strcmp(currentToken, "field") != 0)
		error("Expecting the keyword \"static\" or \"field\" here.");
	/* type */
	readWriteTerminalElement(TOKEN_ERROR, NULL);
	if(!isType(currentToken))
		error("Expecting a variable type here (\"int\", \"boolean\", \"char\") or a class name.");
	/* varName */
	readWriteTerminalElement(IDENTIFIER, NULL);
	/* other varName each separate by a coma */
	readCompilationEngineInput();

	while(strcmp(currentToken, ",") == 0)
	{
		/* ',' */
		writeTerminalElement(SYMBOL, ",");
		/* varName */
		readWriteTerminalElement(IDENTIFIER, NULL);

		readCompilationEngineInput();
	}
	/* ';' */
	writeTerminalElement(SYMBOL, ";");

	writeNonTerminalElementEnd("classVarDec");
}

void compileSubroutine(void)
{
	/* subroutineDec : ('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody */
	/* parameterList : ((type varName) (',' type varName)* )? */
	/* subroutineBody : '{'  varDec* statements '}' */

	char subroutineType[LENGTH_MAX];
	char subroutineName[LENGTH_MAX];

	symbolTable.currentScope = SUBROUTINE_SCOPE;
	/* Create a new symbol table for the subroutine */
	startSubroutine();

	writeNonTerminalElementBeginning("subroutineDec");

	/* ('constructor' | 'function' | 'method') has already be read by the function compileClass so we juste print it */
	writeTerminalElement(TOKEN_ERROR, NULL);
	strcpy(subroutineType, currentToken);
	if(strcmp(subroutineType, "constructor") != 0 && strcmp(subroutineType, "function") != 0 && strcmp(subroutineType, "method") != 0)
		error("Expecting the keyword \"constructor\" or \"function\" or \"method\" here.");
	/* ('void' | type) */
	readWriteTerminalElement(TOKEN_ERROR, NULL);
	if(strcmp(currentToken, "void") != 0 && !isType(currentToken))
		error("Expecting the return type of the subroutine here (\"void\", \"int\", \"boolean\", \"char\") or a class name.");
	/* subroutineName */
	readWriteTerminalElement(IDENTIFIER, NULL);
	strcpy(subroutineName, currentToken);
	strcpy(currentSubroutineName, subroutineName);
	/* Print the function name (className.subroutineName) in the VM file */
	fprintf(vmFile, "function %s.%s ", currentClassName, subroutineName);
	/* '(' */
	readWriteTerminalElement(SYMBOL, "(");
	/* parameterList */
	compileParameterList();
	/* ')' */
	readWriteTerminalElement(SYMBOL, ")");
	/* subroutineBody */
	compileSubroutineBody(subroutineType);

	writeNonTerminalElementEnd("subroutineDec");	
}

void compileParameterList(void)
{
	/* parameterList : ((type varName) (',' type varName)* )? */


	char varName[LENGTH_MAX];
	char varType[LENGTH_MAX];
	long int inputFilePosition;

	writeNonTerminalElementBeginning("parameterList");

	/* Get the position indicator of the input file */
	inputFilePosition = ftell(compilationEngineInput);
	readCompilationEngineInput();
	/* If the parameterList is not empty */
	if(strcmp(currentToken, ")") != 0)
	{
		/* type */
		writeTerminalElement(TOKEN_ERROR, NULL);
		strcpy(varType, currentToken);
		if(!isType(varType))
			error("Expecting a variable type here (\"int\", \"boolean\", \"char\") or a class name.");
		/* varName */
		readWriteTerminalElement(IDENTIFIER, NULL);
		strcpy(varName, currentToken);
		/* Define a new variable */
		Define(varName, varType, ARG_KIND);
		/* other parameter */
		/* Get the position indicator of the input file */
		inputFilePosition = ftell(compilationEngineInput);
		readCompilationEngineInput();
		while(strcmp(currentToken, ",") == 0)
		{
			/* ',' */
			writeTerminalElement(SYMBOL, ",");
			/* type */
			readWriteTerminalElement(TOKEN_ERROR, NULL);
			strcpy(varType, currentToken);
			if(!isType(varType))
				error("Expecting a variable type here (\"int\", \"boolean\", \"char\") or a class name.");
			/* varName */
			readWriteTerminalElement(IDENTIFIER, NULL);
			strcpy(varName, currentToken);
			/* Define other variable */
			Define(varName, varType, ARG_KIND);

			/* Get the position indicator of the input file */
			inputFilePosition = ftell(compilationEngineInput);
			readCompilationEngineInput();
		}
		/* If the currentToken is not ',' (no more parameter), we restore the last position indicator of the input file */
		fseek(compilationEngineInput, inputFilePosition, SEEK_SET);
	}
	/* If the parameterList is empty we restore the last position indicator of the input file */
	else
		fseek(compilationEngineInput, inputFilePosition, SEEK_SET);

	writeNonTerminalElementEnd("parameterList");
}

void compileSubroutineBody(const char *subroutineType)
{
	/* subroutineBody : '{' varDec* statements '}' */

	int nbSubroutineVar;
	long int inputFilePosition;

	nbSubroutineVar = 0;

	writeNonTerminalElementBeginning("subroutineBody");

	/* '{' */
	readWriteTerminalElement(SYMBOL, "{");
	/* varDec* */
	/* Get the position indicator of the input file */
	inputFilePosition = ftell(compilationEngineInput);
	readCompilationEngineInput();
	while(strcmp(currentToken, "var") == 0)
	{
		nbSubroutineVar += compileVarDec();
		/* Get the position indicator of the input file */
		inputFilePosition = ftell(compilationEngineInput);
		readCompilationEngineInput();
	}
	/* Read all variable declaration, and if the current token is not var (no more declaration), we restore the last position indicator of the input file */
	fseek(compilationEngineInput, inputFilePosition, SEEK_SET);
	/* Print the number of subroutine locals variable in the VM file (just after printint the function name) */
	fprintf(vmFile, "%d\n", nbSubroutineVar);
	/* statements */
	/* If the subroutine is a constructor, we have to allocate the memory space for the object */
	if(strcmp(subroutineType, "constructor") == 0)
		writeAllocateMemoryBlockConstructor();
	/* Else if the subroutine is a method, we have to set the this segment to the object */
	else if(strcmp(subroutineType, "method") == 0)
		writeSetObjectInMemoryMethod();
	compileStatements();
	/* '}' */
	readWriteTerminalElement(SYMBOL, "}");

	writeNonTerminalElementEnd("subroutineBody");
}

int compileVarDec(void)
{
	/* varDec : var type varName (',' varName)* ';' */

	char varName[LENGTH_MAX];
	char varType[LENGTH_MAX];
	int nbVar;

	nbVar = 0;

	writeNonTerminalElementBeginning("varDec");

	/* var has already be read so we just print it */
	writeTerminalElement(KEYWORD, "var");
	/* type */
	readWriteTerminalElement(TOKEN_ERROR, NULL);
	strcpy(varType, currentToken);
	if(!isType(varType))
		error("Expecting a variable type here (\"int\", \"boolean\", \"char\") or a class name.");
	/* varName */
	readWriteTerminalElement(IDENTIFIER, NULL);
	strcpy(varName, currentToken);
	/* Define a variable */
	Define(varName, varType, VAR_KIND);
	++nbVar;
	/* other varName each separate by a coma */
	readCompilationEngineInput();
	while(strcmp(currentToken, ",") == 0)
	{
		/* ',' */
		writeTerminalElement(SYMBOL, ",");
		/* varName */
		readWriteTerminalElement(IDENTIFIER, NULL);
		strcpy(varName, currentToken);
		/* Define other variable */
		Define(varName, varType, VAR_KIND);
		readCompilationEngineInput();
		++nbVar;
	}
	/* ';' */
	writeTerminalElement(SYMBOL, ";");

	writeNonTerminalElementEnd("varDec");

	return nbVar;
}

int isCurrentTokenStatement(void)
{
	/* Return 1 if the current token is a statement and 0 else */

	static const char *baseStatement[NUMBER_STATEMENT] =
	{
		"let",
		"if",
		"while",
		"do",
		"return"
	};
	int indexStatement;

	for(indexStatement = 0; indexStatement < NUMBER_STATEMENT; ++indexStatement)
		if(strcmp(currentToken, baseStatement[indexStatement]) == 0)
			return 1;

	return 0;
}

void compileStatements(void)
{
	/* statements : statement* */
	/* statement : letStatement | ifStatement | whileStatement | doStatement | returnStatement */

	long int inputFilePosition;

	writeNonTerminalElementBeginning("statements");

	/* statement* */
	/* Get the position indicator of the input file */
	inputFilePosition = ftell(compilationEngineInput);
	readCompilationEngineInput();
	while(isCurrentTokenStatement())
	{
		/* Test which statement the token is */
		if(strcmp(currentToken, "let") == 0)
			compileLet();
		else if(strcmp(currentToken, "if") == 0)
			compileIf();
		else if(strcmp(currentToken, "while") == 0)
			compileWhile();
		else if(strcmp(currentToken, "do") == 0)
			compileDo();
		else if(strcmp(currentToken, "return") == 0)
			compileReturn();

		/* Get the position indicator of the input file */
		inputFilePosition = ftell(compilationEngineInput);
		readCompilationEngineInput();
	}
	/* If the current token is not a statement (no more statement), we restore the last position indicator of the input file */
	fseek(compilationEngineInput, inputFilePosition, SEEK_SET);

	writeNonTerminalElementEnd("statements");
}

int isType(const char *token)
{
	/* Return 1 if token is a type (int, char, boolean, String, Array, className) and 0 else */

	if(strcmp(token, "int") == 0 || strcmp(token, "char") == 0 || strcmp(token, "boolean") == 0 || strcmp(token, "String") == 0 || strcmp(token, "Array") == 0 || KindOf(token) != NONE)
		return 1;
	else
		return 0;
}

void compileLet(void)
{
	/* letStatement : 'let' varName ('[' expression ']')? '=' expression ';' */

	/* If the variable is :
		- an array : get the address of the result, get the expression's result and push it in the address
		- an object : get the expression's result and push it in the object like a reference address
		- a variable : get the expression's result and push it into the variable
	*/

	char varName[LENGTH_MAX];
	char varType[LENGTH_MAX];
	char subroutineName[LENGTH_MAX];

	writeNonTerminalElementBeginning("letStatement");

	/* 'let' has already be read so we just print it */
	writeTerminalElement(KEYWORD, "let");
	/* varName */
	readWriteTerminalElement(IDENTIFIER, NULL);
	strcpy(varName, currentToken);
	if(KindOf(varName) == NONE)
		error("Unknow variable here.");
	/* Get the type of the variable */
	TypeOf(varName, varType);
	/* ('[' expression ']')? '=' */
	readCompilationEngineInput();
	/* If it is an array */
	if(strcmp(currentToken, "[") == 0)
	{
		/* '[' */
		writeTerminalElement(SYMBOL, "[");
		/* expression */
		compileExpression();
		/* Compute the expression's result with the array to find the memory address of the result and store it in the TEMP segment */
		writeArrayMemory(varName, subroutineName);
		/* ']' */
		readWriteTerminalElement(SYMBOL, "]");
		/* '=' */
		readWriteTerminalElement(SYMBOL, "=");
		/* expression */
		compileExpression();
		/* Put the expression's result in the address memory calculated before */
		writeArrayAllocation();
		/* ';' */
		readWriteTerminalElement(SYMBOL, ";");
	}
	/* Else if it is an object of any type (so not 'int', 'char', 'boolean') */
	else if(!isType(varType))
	{
		/* When we effect an expression to an object, the expression is taken for an address memory */

		/* '=' */
		writeTerminalElement(SYMBOL, "=");
		/* expression */
		compileExpression();
		/* Pop the expression's result in the object (like an address memory for the object) */
		writePop(getSegmentFromKind(KindOf(varName)), IndexOf(varName));
		/* ';' */
		readWriteTerminalElement(SYMBOL, ";");
	}
	/* Else it is a variable ('int', 'char', 'boolean') */
	else
	{
		/* '=' */
		writeTerminalElement(SYMBOL, "=");
		/* expression */
		compileExpression();
		/* ';' */
		readWriteTerminalElement(SYMBOL, ";");

		/* If we are in a method subroutine and the variable is an argument */
		TypeOf(currentSubroutineName, subroutineName);
		if(strcmp(subroutineName, "method") == 0 && KindOf(varName) == ARG_KIND)
			/* Pop the variable but with index + 1 (because the first argument is an object in a method) */
			writePop(getSegmentFromKind(KindOf(varName)), IndexOf(varName) + 1);
		else
			/* Pop the variable */
			writePop(getSegmentFromKind(KindOf(varName)), IndexOf(varName));
	}

	writeNonTerminalElementEnd("letStatement");
}

void compileIf(void)
{
	/* ifStatement : 'if' '(' expression ')' '{' statements '}' 
			        ('else' '{' statements '}')? */

	/* if(cond)	-> VM code for computing ~(cond)
		s1	-> if-goto L1
	else 		-> VM code for executing s1
		s2	-> goto L2
			-> label L1
			-> VM code for executing s2
			-> label L2
	*/

	static int indexIfStatement = 0, indexElseStatement = 0;
	char ifLabel[LENGTH_MAX];
	char elseLabel[LENGTH_MAX];
	long int inputFilePosition;
	
	writeNonTerminalElementBeginning("ifStatement");
	
	/* 'if' has already be read so we just print it */
	writeTerminalElement(KEYWORD, "if");
	/* '(' */
	readWriteTerminalElement(SYMBOL, "(");
	/* expression */
	/* VM code for computing ~(cond) */
	compileExpression();
	writeArithmetic(NOT);
	sprintf(elseLabel, "ELSE_LABEL_%d", indexElseStatement);
	/* if-goto L1 */
	writeIf(elseLabel);
	++indexElseStatement;
	/* ')' */
	readWriteTerminalElement(SYMBOL, ")");
	/* '{' */
	readWriteTerminalElement(SYMBOL, "{");
	/* statements */
	/* VM code for executing s1 */
	compileStatements();
	/* '}' */
	readWriteTerminalElement(SYMBOL, "}");
	sprintf(ifLabel, "IF_LABEL_%d", indexIfStatement);
	/* goto L2 */
	writeGoto(ifLabel);
	++indexIfStatement;
	/* ('else' '{' statements '}')? */
	/* Get the position indicator of the input file */
	inputFilePosition = ftell(compilationEngineInput);
	readCompilationEngineInput();
	if(strcmp(currentToken, "else") == 0)
	{
		/* 'else' */
		writeTerminalElement(KEYWORD, "else");
		/* '{' */
		readWriteTerminalElement(SYMBOL, "{");
		/* label L1 */
		writeLabel(elseLabel);
		/* statements */
		/* VM code for executing s2 */
		compileStatements();
		/* '}' */
		readWriteTerminalElement(SYMBOL, "}");
	}
	/* If the current token is not the keyword 'else' we restore the position indicator of the input file */
	else
	{
		fseek(compilationEngineInput, inputFilePosition, SEEK_SET);
		writeLabel(elseLabel);
	}

	/* label L2 */
	writeLabel(ifLabel);

	writeNonTerminalElementEnd("ifStatement");
}

void compileWhile(void)
{
	/* whileStatement : 'while' '(' expression ')' '{' statements '}' */

	/* while (cond)	-> label L1
		s1		-> VM code for computing ~(cond)
				-> if-goto L2
				-> VM code for executing s1
				-> goto L1
				-> label L2
	*/

	static int indexWhileStatement = 0;
	char whileLabel[LENGTH_MAX];
	char whileEndLabel[LENGTH_MAX];
	
	writeNonTerminalElementBeginning("whileStatement");

	/* 'while' has already be read so we just print it */
	writeTerminalElement(KEYWORD, "while");
	sprintf(whileLabel, "WHILE_LABEL_%d", indexWhileStatement);
	/* label L1 */
	writeLabel(whileLabel);
	++indexWhileStatement;
	/* '(' */
	readWriteTerminalElement(SYMBOL, "(");
	/* expression */
	/* VM code for computing ~(cond) */
	compileExpression();
	writeArithmetic(NOT);
	sprintf(whileEndLabel, "WHILE_END_LABEL_%d", indexWhileStatement);
	/* if-goto L2 */
	writeIf(whileEndLabel);
	/* ')' */
	readWriteTerminalElement(SYMBOL, ")");
	/* '{' */
	readWriteTerminalElement(SYMBOL, "{");
	/* statements */
	/* VM code for executing s1 */
	compileStatements();
	/* goto L1 */
	writeGoto(whileLabel);
	/* '}' */
	readWriteTerminalElement(SYMBOL, "}");
	/* label L2 */
	writeLabel(whileEndLabel);

	writeNonTerminalElementEnd("whileStatement");
}

void compileDo(void)
{
	/* doStatement : 'do' subroutineCall ';' */
	/* subroutineCall : subroutineName '(' expressionList ')' | (className | varName) '.' subroutineName '(' expressionList ')' */
	/* expressionList : (expression (',' expression)*)? */

	/* If the subroutine is :
		- a method : push the object, and the expressionList and call the method with nbArg + 1 
			(INSIDE a class, call a method like : methodName OUTSIDE a class, call a method like : varName.methodName)
		- a function : push the expressionList and call the function with nbArg
			(ALWAYS call a function like : className.functionName)
	*/

	char name[LENGTH_MAX];

	writeNonTerminalElementBeginning("doStatement");

	/* 'do' has already be read so we just print it */
	writeTerminalElement(KEYWORD, "do");
	/* subroutineName OR (className | varName) */
	readWriteTerminalElement(IDENTIFIER, NULL);
	strcpy(name, currentToken);
	/* '(' expressionList ')' OR '.' subroutineName '(' expressionList ')' */
	readCompilationEngineInput();

	/* If the subroutineCall is a method call (inside of a class) */
	if(strcmp(currentToken, "(") == 0)
	{
		if(KindOf(name) == NONE)
			error("Unknow subroutine name here.");
		
		writeMethodCall(name);
	}
	/* Else, if the subroutine call is a function or a method (but outside of a class) */
	else
		writeSubroutineCall(name);
	/* ';' */
	readWriteTerminalElement(SYMBOL, ";");
	/* We pop the return value even if it is a void subroutine or if we juste don't want the return value */
	writePop(TEMP_SEGMENT, 0);

	writeNonTerminalElementEnd("doStatement");
}

void compileReturn(void)
{
	/* returnStatement : 'return' expression? ';' */

	/* If this is a void subroutine push 0 and return, else push the expression and return */

	long int inputFilePosition;
	
	writeNonTerminalElementBeginning("returnStatement");	

	/* 'return' has already be read so we just print it */
	writeTerminalElement(KEYWORD, "return");

	/* expression? ';' */
	/* Get the position indicator of the input file */
	inputFilePosition = ftell(compilationEngineInput);
	readCompilationEngineInput();
	/* If the subroutine return an expression */
	if(strcmp(currentToken, ";") != 0)
	{
		/* If the current token is not the symbol ';' we restore the position indicator of the input file */
		fseek(compilationEngineInput, inputFilePosition, SEEK_SET);
		/* expression */
		compileExpression();
		/* ';' */
		readWriteTerminalElement(SYMBOL, ";");

		writeReturn();
	}
	/* Else we have to return a value which is 0 */
	else
	{
		/* ';' */
		writeTerminalElement(SYMBOL, ";");

		writePush(CONST_SEGMENT, 0);
		writeReturn();
	}

	writeNonTerminalElementEnd("returnStatement");	
}

int isCurrentTokenOp(void)
{
	/* Return 1 if the current token is an op and 0 else */

	static const char *baseOp[NUMBER_OP] =
	{
		"+",
		"-",
		"*",
		"/",
		"&amp;",
		"|",
		"&lt;",
		"&gt;",
		"="
	};
	int indexOp;

	for(indexOp = 0; indexOp < NUMBER_OP; ++indexOp)
		if(strcmp(currentToken, baseOp[indexOp]) == 0)
			return 1;

	return 0;
}

void compileExpression(void)
{
	/* expression : term (op term)* */
	/* term : intergerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall | '(' expression ')' | unaryOp term */
	/* subroutineCall : subroutineName '(' expressionList ')' | (className | varName) '.' subroutineName '(' expressionList ')' */
	/* expressionList : (expression (',' expression)*)? */
	/* op : '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '=' */
	/* unaryOp : '-' | '~' */
	/* keywordConstant : 'true' | 'false' | 'null' | 'this' */

	/* expression(exp) :
		if exp is a number n then output "push n"
		if exp is a variable v then output "push v"
		if exp = (exp1 op exp2) then expression(exp1), expression(exp2), output "op"
		if exp = op(exp1) then expression(exp1), output "op" 
		if exp = f(exp1 ... expN) then expression(exp1), ..., expression(expN), output "call f"
	*/

	char op[LENGTH_MAX];
	long int inputFilePosition;

	writeNonTerminalElementBeginning("expression");

	/* term */
	compileTerm();
	/* (op term)* */
	/* Get the position indicator of the input file */
	inputFilePosition = ftell(compilationEngineInput);
	readCompilationEngineInput();
	while(isCurrentTokenOp())
	{
		/* op */
		writeTerminalElement(SYMBOL, NULL);
		strcpy(op, currentToken);
		/* term */
		compileTerm();
		/* output "op" */
		printOp(op);

		/* Get the position indicator of the input file */
		inputFilePosition = ftell(compilationEngineInput);
		readCompilationEngineInput();	
	}
	/* If the currentToken is not an operator (no more term), we restore the last position indicator of the input file */
	fseek(compilationEngineInput, inputFilePosition, SEEK_SET);

	writeNonTerminalElementEnd("expression");
}

int isCurrentTokenUnaryOp(void)
{
	/* Return 1 if the current token is an unary op and 0 else */

	if(strcmp(currentToken, "-") == 0 || strcmp(currentToken, "~") == 0)
		return 1;
	else
		return 0;
}

void compileTerm(void)
{
	/* term : intergerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall | '(' expression ')' | unaryOp term */
	/* subroutineCall : subroutineName '(' expressionList ')' | (className | varName) '.' subroutineName '(' expressionList ')' */
	/* expressionList : (expression (',' expression)*)? */
	/* op : '+' | '-' | '*' | '/' | '&' | '|' | '<' | '>' | '=' */
	/* unaryOp : '-' | '~' */
	/* keywordConstant : 'true' | 'false' | 'null' | 'this' */

	char name[LENGTH_MAX];
	char type[LENGTH_MAX];
	char op[LENGTH_MAX];
	long int inputFilePosition;

	writeNonTerminalElementBeginning("term");

	readCompilationEngineInput();
	strcpy(type, currentType);
	/* unaryOp term */
	if(isCurrentTokenUnaryOp())
	{
		/* unaryOp */
		writeTerminalElement(SYMBOL, NULL);
		strcpy(op, currentToken);
		/* term */
		compileTerm();
		/* output "op" */
		printUnaryOp(op);
	}
	/* '(' expression ')' */
	else if(strcmp(currentToken, "(") == 0)
	{
		/* '(' */
		writeTerminalElement(SYMBOL, "(");
		/* expression */
		compileExpression();
		/* ')' */
		readWriteTerminalElement(SYMBOL, ")");
	}
	/* intergerConstant | stringConstant | keywordConstant | varName | varName '[' expression ']' | subroutineCall */
	else
	{
		/* intergerConstant | stringConstant | keywordConstant | varName | subroutineName */
		writeTerminalElement(TOKEN_ERROR, NULL);
		strcpy(name, currentToken);
		/* Get the position indicator of the input file */
		inputFilePosition = ftell(compilationEngineInput);
		readCompilationEngineInput();
		/* varName '[' expression ']' */
		if(strcmp(currentToken, "[") == 0)
		{
			/* varName has already be print */
			if(KindOf(name) == NONE)
				error("Unknow variable name here.");
			/* '[' */
			writeTerminalElement(SYMBOL, "[");
			/* expression */
			compileExpression();
			writeArrayExpression(name);
			/* ']' */
			readWriteTerminalElement(SYMBOL, "]");
		}
		/* subroutineCall (a method call inside a class) */
		else if(strcmp(currentToken, "(") == 0)
		{
			int nbArgs;
			/* subroutineName has already be print */
			if(KindOf(name) == NONE)
				error("Unknow subroutine name here.");
			/* '(' */
			writeTerminalElement(SYMBOL, "(");
			/* expressionList */
			nbArgs = compileExpressionList();
			/* ')' */
			readWriteTerminalElement(SYMBOL, ")");

			writeCall(name, nbArgs);
		}
		/* subroutineCall (function, constructor, method outside a class) */
		else if(strcmp(currentToken, ".") == 0)
			writeSubroutineCall(name);
		/* intergerConstant | stringConstant | keywordConstant | varName */
		else
		{
			int kind;
			char subroutineName[LENGTH_MAX];

			/* intergerConstant | stringConstant | keywordConstant | varName has already be print */
			/* Restore the last position indicator of the input file because we already print intergerConstant | stringConstant | keywordConstant | varName */
			fseek(compilationEngineInput, inputFilePosition, SEEK_SET);

			strcpy(currentToken, name);
			kind = KindOf(currentToken);
			/* intergerConstant */
			if(isIntegerConstant(currentToken))
				/* Just push it */
				writePush(CONST_SEGMENT, atoi(currentToken));
			/* varName */
			else if(kind != NONE)
			{
				/* If we are in a method and the variable is an argument, we push the variable with index + 1 because the first arguement in a method is always the object */
				TypeOf(currentSubroutineName, subroutineName);
				if(strcmp(subroutineName, "method") == 0 && KindOf(currentToken) == ARG_KIND)
					writePush(getSegmentFromKind(kind), IndexOf(currentToken) + 1);
				/* Else we just push it */
				else
					writePush(getSegmentFromKind(kind), IndexOf(currentToken));
			}
			/* keywordConstant */
			else if(strcmp(currentToken, "true") == 0)
			{
				/* true -> -1 */
				writePush(CONST_SEGMENT, 1);
				writeArithmetic(NEG);
			}
			/* keywordConstant */
			else if(strcmp(currentToken, "false") == 0 || strcmp(currentToken, "null") == 0)
				/* false 	-> 0
				    null 	-> 0
				*/
				writePush(CONST_SEGMENT, 0);
			/* keywordConstant */
			else if(strcmp(currentToken, "this") == 0)
				/* this -> pointer 0 */
				writePush(POINTER_SEGMENT, 0);
			/* stringConstant */
			else if(strcmp(type, "stringConstant") == 0)
				writeStringConstant(currentToken);
			else
				error("Unknow variable name here.");
		}
	}

	writeNonTerminalElementEnd("term");
}

int compileExpressionList(void)
{
	/* expressionList : (expression (',' expression)*)? */

	long int inputFilePosition;
	int nbExpression;

	nbExpression = 0;

	writeNonTerminalElementBeginning("expressionList");

	/* Get the position indicator of the input file */
	inputFilePosition = ftell(compilationEngineInput);
	readCompilationEngineInput();
	/* If there is an expression */
	if(strcmp(currentToken, ")") != 0)
	{
		/* Restore the last position indicator of the input file because the current token is an expression */
		fseek(compilationEngineInput, inputFilePosition, SEEK_SET);
		/* expression */
		compileExpression();
		++nbExpression;
		/* other expression */
		/* Get the position indicator of the input file */
		inputFilePosition = ftell(compilationEngineInput);
		readCompilationEngineInput();
		while(strcmp(currentToken, ",") == 0)
		{
			/* ',' */
			writeTerminalElement(SYMBOL, ",");
			/* expression */
			compileExpression();
			++nbExpression;

			/* Get the position indicator of the input file */
			inputFilePosition = ftell(compilationEngineInput);
			readCompilationEngineInput();
		}
		/* Put the position indicator of the input file in the correct position because we read one more than we should (for other(s) expression(s)) */
		fseek(compilationEngineInput, inputFilePosition, SEEK_SET);
	}
	else
		/* If there is the current token is ')' (no expression), we restore the last position indicator of the input file */
		fseek(compilationEngineInput, inputFilePosition, SEEK_SET);

	writeNonTerminalElementEnd("expressionList");

	return nbExpression;
}

void createCompilationEngineOutputFileName(char *string)
{
	int length;

	length = strlen(string);
	string[length - 4] = 'x';
	string[length - 3] = 'm';
	string[length - 2] = 'l';
	string[length - 1] = '\0';
}

void writeCompilationEngineOutput(void)
{
	/* Each file starts with a class declaration */
	
	if(strcmp(currentToken, "class") == 0)
		compileClass();
	else
		error("Expecting a class declaration here.");
}

void readCompilationEngineInput(void)
{
	/* A line is like this : lineNumber <type> token <type> */


	char line[LENGTH_MAX];
	char number[LENGTH_MAX];
	int indexLine, indexType, indexToken;
	int isInTokenType;

	isInTokenType = 1;

	/* Read the line line in the current jack file */
	if(fgets(line, LENGTH_MAX, compilationEngineInput) != NULL)
		/* Put the last character of line to '\0' */
		line[strlen(line) - 1] = '\0';
	else
		return ;

	/* Get the line number */
	for(indexLine = 0; isdigit(line[indexLine]); ++indexLine)
		number[indexLine] = line[indexLine];

	number[indexLine] = '\0';
	lineNumber = atoi(number);

	/* We skip the space character and the '<' character */
	indexLine += 2;

	/* type> token <type> */
	for(indexType = 0, indexToken = 0; line[indexLine] != '<'; ++indexLine)
	{
		if(line[indexLine] == '>')
		{
			isInTokenType = 0;
			indexLine += 2;
		}

		if(isInTokenType)
		{
			currentType[indexType] = line[indexLine];
			++indexType;
		}
		else
		{
			currentToken[indexToken] = line[indexLine];
			++indexToken;
		}
	}

	currentToken[indexToken - 1] = '\0';
	currentType[indexType] = '\0';
}

void compilationEngine(void)
{
	char path[LENGTH_MAX];

	indentation = 0;

	strcpy(path, currentFileName);
	createTokenizerOutputFileName(path);
	compilationEngineInput = fopen(path, "r");

	strcpy(path, currentFileName);
	createCompilationEngineOutputFileName(path);
	compilationEngineOutput = fopen(path, "w+");

	readCompilationEngineInput();
	writeCompilationEngineOutput();

	fclose(compilationEngineInput);
	fclose(compilationEngineOutput);

	/*strcpy(path, currentFileName);
	createTokenizerOutputFileName(path);
	remove(path);*/
}

#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include "JackAnalyzer.h"

#include "JackTokenizer.h"
#include "CompilationEngine.h"
#include "SymbolTable.h"

/* Get the VM output file name */
void getVMOutputFileName(char *string)
{
	int length;

	length = strlen(string);
	string[length - 4] = 'v';
	string[length - 3] = 'm';
	string[length - 2] = '\0';
}

/* Translate the jack file currentFileName */
void translateJackFile(void)
{
	jackTokenizer();
	compilationEngine();
}

void checkDeclaration(const char *line)
{
	/* Checks if in line there is a declaraction like a class declaraction, a class variable declaration, or a subroutine declaration */

	char *result;

	result = strstr(line, "class");
	if(result != NULL)
	{
		DefineClassDec(result);
		return ;
	}

	result = strstr(line, "static");
	if(result != NULL)
	{
		DefineClassVarDec(result, STATIC_KIND);
		return ;
	}

	result = strstr(line, "field");
	if(result != NULL)
	{
		DefineClassVarDec(result, FIELD_KIND);
		return ;
	}

	result = strstr(line, "function");
	if(result != NULL)
	{
		DefineSubroutineDec(result, FUNCTION_SUB);
		return ;
	}

	result = strstr(line, "constructor");
	if(result != NULL)
	{
		DefineSubroutineDec(result, CONSTRUCTOR_SUB);
		return ;
	}

	result = strstr(line, "method");
	if(result != NULL)
		DefineSubroutineDec(result, METHOD_SUB);
}

/* Clean the file fileName, by removing comments, useless space character, and empty lines */
void cleanJackFile(const char *fileName)
{
	FILE *tokenizerInput;
	int isInAComment;
	int lineNumber;

	isInAComment = 0;
	lineNumber = 1;

	/* Open the input  of the tokenizer (which is the output of this function) */
	tokenizerInput = fopen(fileName, "w+");

	while(!feof(currentJackFile))
	{
		char buffer[LENGTH_MAX];
		char line[LENGTH_MAX];
		int indexBuffer, indexLine;
		int nbSpace;

		nbSpace = 0;

		if(fgets(buffer, LENGTH_MAX, currentJackFile) != NULL)
			buffer[strlen(buffer)] = '\0';
		else
		{
			line[0] = '\0';
			break;
		}

		/* Read the entire line, and ignore it if this is in a comment (multi 
		lines comment, or a simple line comment), else copy everything 
		except useless space character and control character */
		for(indexBuffer = 0, indexLine = 0; buffer[indexBuffer] != '\0'; ++indexBuffer)
		{	
			if(buffer[indexBuffer] == '/' && buffer[indexBuffer + 1] == '*')
			{
				isInAComment = 1;

				if(strstr(buffer, "*/") != NULL)
					isInAComment = 0;

				break;
			}
			if(buffer[indexBuffer] == '/' && buffer[indexBuffer + 1] == '/')
				break;
			else if(buffer[indexBuffer] == '*' && buffer[indexBuffer + 1] == '/')
			{
				isInAComment = 0;

				++indexBuffer;
				continue ;
			}

			if(!isInAComment)
			{
				if(buffer[indexBuffer] == ' ')
				{
					++nbSpace;

					if(nbSpace <= 1)
					{
						line[indexLine] = buffer[indexBuffer];
						++indexLine;
					}
				}
				else if(!iscntrl(buffer[indexBuffer]))
				{
					line[indexLine] = buffer[indexBuffer];
					++indexLine;

					nbSpace = 0;
				}
			}
		}

		line[indexLine] = '\0';

		/* Check if there is any declaration in the current line like a class declaration, a subroutine etc.*/
		checkDeclaration(line);

		/* Copy all the character of the clean line in the output (the tokenizer input) */
		/* If the line begin with a space we print only the line number */
		for(indexLine = 0; line[indexLine] != '\0'; ++indexLine)
		{
			if(indexLine == 0 && line[indexLine] == ' ')
			{
				fprintf(tokenizerInput, "%d ", lineNumber);
				continue ;
			}
			else if(indexLine == 0)
			{
				fprintf(tokenizerInput, "%d ", lineNumber);
				fputc(line[indexLine], tokenizerInput);
			}
			else
				fputc(line[indexLine], tokenizerInput);
		}

		if(line[0] != '\0')
			fputc('\n', tokenizerInput);

		++lineNumber;
	}

	fclose(tokenizerInput);
}

void prepareJackCompiler(void)
{
	/* Create the symbol table (class, subroutine) and clean all the files */
	char path[LENGTH_MAX];

	strcpy(path, currentFileName);
	createTokenizerOutputFileName(path);

	strcat(path, "_clean.xml");
	cleanJackFile(path);
}

void goThroughDirectory(const char *directoryName, void (*pointer)(void))
{
	DIR *directory;
	struct dirent *file;

	/* Go throught the directory */
	directory = opendir(directoryName);

	while((file = readdir(directory)) != NULL)
	{
		/* If the file name is a .jack */
		if(strstr(file->d_name, ".jack") != NULL)
		{
			char fileName[LENGTH_MAX];

			/* Get the name of the .jack file */
			sprintf(fileName, "%s/%s", directoryName, file->d_name);
			strcpy(currentFileName, fileName);
			currentJackFile = fopen(fileName, "r");

			getVMOutputFileName(fileName);
			vmFile = fopen(fileName, "w+");

			/* Call the function in parameter */
			(*pointer)();

			fclose(currentJackFile);
			fclose(vmFile);
		}
	}

	closedir(directory);
}

int main(int argc, char *argv[])
{
	char vmFilePath[LENGTH_MAX];

	ConstructorSymbolTable();
	symbolTable.currentScope = CLASS_SCOPE;
	DefineJackStandardLibrarySubroutine();

	/* If it is a directory */
	if(strchr(argv[1], '.') == NULL)
	{
		/* Create the symbol table (class, subroutine) and clean all the files */
		goThroughDirectory(argv[1], &prepareJackCompiler);
		/* Translate the .jack file */
		goThroughDirectory(argv[1], &translateJackFile);
	}
	/* Else it is a simple .jack file */
	else
	{
		strcpy(currentFileName, argv[1]);
		currentJackFile = fopen(currentFileName, "r");

		strcpy(vmFilePath, argv[1]);
		getVMOutputFileName(vmFilePath);
		vmFile = fopen(vmFilePath, "w+");

		/* Create the symbol table (class, subroutine) and clean all the files */
		prepareJackCompiler();
		/* Translate the .jack file */
		translateJackFile();

		fclose(currentJackFile);
		fclose(vmFile);
	}
	
	return 0;
}

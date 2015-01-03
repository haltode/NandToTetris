#include <stdio.h>
#include <string.h>
#include "vmTranslator.h"

#include "parser.h"
#include "codeWriter.h"

/* Translate the file path (.vm file) */
void translate(const char *path)
{
	currentVMFile = fopen(path, "r");

	/* While there is more commands in the current VM file */
	while(hasMoreCommands())
	{
		int currentCommandType;
		char currentArg1[LENGTH_MAX];
		int currentArg2;

		/* Read the current command */
		advance();

		/* If it is not the end of the .vm file or a comment */
		if(currentCommand[0] != '\0' && currentCommand[1] != '\0')
		{
			/* Get the current command type */
			currentCommandType = commandType();

			/* Test all commands types */
			switch(currentCommandType)
			{
				case C_ARITHMETIC :
					/* Get the name of the arithmetic command */
					arg1(currentArg1);

					/* Write the asm code corresponding to this command */
					writeArithmetic(currentArg1);
				break;
				case C_PUSH :
					/* Get the first and the second argument of the PUSH command */
					arg1(currentArg1);
					currentArg2 = arg2();

					/* Write the asm code correspondingn to a PUSH command with arguments */
					writePushPop(C_PUSH, currentArg1, currentArg2);
				break;
				case C_POP :
					/* Get the first and the second argument of the POP command */
					arg1(currentArg1);
					currentArg2 = arg2();

					/* Write the asm code correspondingn to a POP command with arguments */
					writePushPop(C_POP, currentArg1, currentArg2);
				break;
				case C_LABEL :
					/* Get the first argument of the LABEL command */
					arg1(currentArg1);

					/* Write the asm code correspondingn to a LABEL command with argument */
					writeLabel(currentArg1);
				break;
				case C_GOTO :
					/* Get the first argument of the GOTO command */
					arg1(currentArg1);

					/* Write the asm code correspondingn to a GOTO command with argument */
					writeGoto(currentArg1);
				break;
				case C_IF :
					/* Get the first argument of the IF-GOTO command */
					arg1(currentArg1);

					/* Write the asm code correspondingn to a IF-GOTO command with argument */
					writeIf(currentArg1);
				break;
				case C_FUNCTION :
					/* Get the first and the second argument of the FUNCTION command */
					arg1(currentArg1);
					currentArg2 = arg2();

					/* Write the asm code correspondingn to a FUNCTION command with arguments */
					writeFunction(currentArg1, currentArg2);
				break;
				case C_RETURN :
					/* Write the asm code correspondingn to a RETURN command */
					writeReturn();
				break;
				case C_CALL :
					/* Get the first and the second argument of the CALL command */
					arg1(currentArg1);
					currentArg2 = arg2();

					/* Write the asm code correspondingn to a CALL command with arguments */
					writeCall(currentArg1, currentArg2);
				break;
			}
		}
	}

	fclose(currentVMFile);
}

int main(int argc, char *argv[])
{
	char asmFilePath[LENGTH_MAX];

	/* If it is a directory with multiples files in it */
	if(strchr(argv[1], '.') == NULL)
	{
		DIR *directory;
		struct dirent *file;

		/* Create the name of the asm file name (output) */
		sprintf(asmFilePath, "%s/%s.asm", argv[1], argv[1]);
		asmFile = fopen(asmFilePath, "w+");

		//writeInit();

		/* Go throught the directory */
		directory = opendir(argv[1]);

		while((file = readdir(directory)) != NULL)
		{
			/* If the file name is a .vm */
			if(strstr(file->d_name, ".vm") != NULL)
			{
				char fileName[LENGTH_MAX];

				/* Get the name of the .vm file */
				sprintf(fileName, "%s/%s", argv[1], file->d_name);
				sprintf(currentFileName, "%s", file->d_name);

				currentFileName[strlen(currentFileName) - 3] = '\0';

				/* Translate it */
				translate(fileName);
			}
		}

		closedir(directory);
		fclose(asmFile);
	}
	/* Else it is a simple file */
	else
	{
		int vmFileLenght;

		/* Create the hack file name */
		strcpy(asmFilePath, argv[1]);
		vmFileLenght = strlen(argv[1]);

		asmFilePath[vmFileLenght - 2] = 'a';
		asmFilePath[vmFileLenght - 1] = 's';
		asmFilePath[vmFileLenght] = 'm';
		asmFilePath[vmFileLenght + 1] = '\0';

		asmFile = fopen(asmFilePath, "w+");

		//writeInit();

		/* Translate the file */
		translate(argv[1]);

		fclose(asmFile);
	}

	return 0;
}

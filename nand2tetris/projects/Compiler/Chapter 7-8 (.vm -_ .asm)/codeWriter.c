#include <stdio.h>
#include <string.h>
#include "codeWriter.h"

#include "parser.h"
#include "vmTranslator.h"

int counterBoolean = 0;
int counterFunction = 0;

/* Increment the SP */
void incrementSP(void)
{
	fprintf(asmFile, "@SP\n");
	fprintf(asmFile, "M=M+1\n");
}

/* Get the SP - 1 number on the stack */
void getSP(void)
{
	fprintf(asmFile, "@SP\n");
	fprintf(asmFile, "AM=M-1\n");
}

/* Put the D register on the stack */
void putOnStack(void)
{
	fprintf(asmFile, "@SP\n");
	fprintf(asmFile, "A=M\n");
	fprintf(asmFile, "M=D\n");
}

/* Get segment[index] and store it in the D register */
void getSegmentIndex(const char *segment, const int index)
{
	/* If it is the static segment, we can have the address by write Xxx.index with Xxx the filename */
	if(strcmp(segment, "static") == 0)
	{
		fprintf(asmFile, "@%s.%d\n", currentFileName, index);
		fprintf(asmFile, "D=A\n");
	}
	/* Else it is Argument, local, this, that, pointer, temp segment */
	else
	{
		/* Get the number index, and store it in the D register */
		fprintf(asmFile, "@%d\n", index);
		fprintf(asmFile, "D=A\n");
		/* Get the segment address, and ...*/
		fprintf(asmFile, "@%s\n", segment);

		/* If it is a pointer or a temp segment we want the address not the data */
		if(strcmp(segment, "3") == 0 || strcmp(segment, "5") == 0)
			/* Add to the D register the address */
			fprintf(asmFile, "D=D+A\n");
		/* Else we want the data not the address */
		else
			/* Add to the D register the data */
			fprintf(asmFile, "D=D+M\n");
	}
}

/* Write D into the TEMP segment to save it (temp segment -> 5..12) */
void saveInTempSegment(void)
{
	fprintf(asmFile, "@5\n");
	fprintf(asmFile, "M=D\n");
}

/* Put the D register value in the address store in temp segment */
void addressTempSegment(void)
{
	fprintf(asmFile, "@5\n");
	fprintf(asmFile, "A=M\n");
	fprintf(asmFile, "M=D\n");
}

/* Write the asm code for a ADD or SUB command */
void writeArithmeticAddSub(const int command)
{
	/* Get the SP - 1 number on the stack */
	getSP();
	/* If it is a ADD command, get the number */
	if(command == ADD)
		fprintf(asmFile, "D=M\n");
	/* Else if it is a SUB command, get the opposite of the number */
	else if(command == SUB)
		fprintf(asmFile, "D=-D\n");

	/* Get the SP - 2 number on the stack and store the result of D + M in the D register */
	getSP();
	fprintf(asmFile, "D=D+M\n");

	/* Put the result on the stack */
	putOnStack();
}

/* Write the asm code for a NEG or NOT command */
void writeArithmeticNegNot(const int command)
{
	/* Get the SP - 1 number on the stack */
	getSP();
	/* If it is a NEG command, get the opposite */
	if(command == NEG)
		fprintf(asmFile, "M=-M\n");
	/* Else if it is a NOT command, get the inverse */
	else if(command == NOT)
		fprintf(asmFile, "M=!M\n");
}

/* Write the asm code for a EQ or GT or LT command */
void writeArithmeticEqGtLt(const int command)
{
	/* Get the SP - 1 number on the stack and store the opposite in the D register */
	getSP();
	fprintf(asmFile, "D=-M\n");

	/* Get the SP - 2 number on the stack and store the result of D + M in the D register */
	getSP();
	fprintf(asmFile, "D=D+M\n");

	/* Goto the label TRUE if ... */
	fprintf(asmFile, "@TRUE_%d\n", counterBoolean);
	/* If it is a EQ command, and that the conditionnal jump is true (JEQ -> out = 0) */
	if(command == EQ)
		fprintf(asmFile, "D;JEQ\n");
	/* Else if it is a GT command, and that the conditionnal jump is true (JGT -> out > 0) */
	else if(command == GT)
		fprintf(asmFile, "D;JGT\n");
	/* Else if it is a LT command, and that the conditionnal jump is true (JLT -> out < 0) */
	else if(command == LT)
		fprintf(asmFile, "D;JLT\n");
	/* Create the label LOOP */
	fprintf(asmFile, "(LOOP_%d)\n", counterBoolean);
	/* Goto the label FALSE */
	fprintf(asmFile, "@FALSE_%d\n", counterBoolean);
	/* If it is a EQ command, and that the conditionnal jump is true (JNE -> out != 0) */
	if(command == EQ)
		fprintf(asmFile, "D;JNE\n");
	/* Else if it is a GT command, and that the conditionnal jump is true (JLE -> out <= 0) */
	else if(command == GT)
		fprintf(asmFile, "D;JLE\n");
	/* Else if it is a LT command, and that the conditionnal jump is true (JGE -> out >= 0) */
	else if(command == LT)
		fprintf(asmFile, "D;JGE\n");
	/* Goto the label END */
	fprintf(asmFile, "@END_%d\n", counterBoolean);
	fprintf(asmFile, "0;JMP\n");

	/* Create the label TRUE */
	fprintf(asmFile, "(TRUE_%d)\n", counterBoolean);
	/* Set the SP to -1 (true value) */
	fprintf(asmFile, "@SP\n");
	fprintf(asmFile, "A=M\n");
	fprintf(asmFile, "M=-1\n");
	/* Goto to the begining of the loop */
	fprintf(asmFile, "@LOOP_%d\n", counterBoolean);
	fprintf(asmFile, "0;JMP\n");

	/* Create the label FALSE s*/
	fprintf(asmFile, "(FALSE_%d)\n", counterBoolean);
	/* Set the SP to 0 (false value) */
	fprintf(asmFile, "@SP\n");
	fprintf(asmFile, "A=M\n");
	fprintf(asmFile, "M=0\n");

	/* Create the label END */
	fprintf(asmFile, "(END_%d)\n", counterBoolean);

	/* Increment the counterBoolean. It is use to create multiple TRUE, FALSE, END label in the same program. */
	++counterBoolean;
}

/* Write the asm code for an AND or OR command */
void writeArithmeticAndOr(const int command)
{
	/* Get the SP - 1 number on the stack, and store it in the D register */
	getSP();
	fprintf(asmFile, "D=M\n");
	/* Get the SP - 2 number on the stack */
	getSP();
	/* If it is an AND command */
	if(command == AND)
		/* Use the AND operator on D and M */
		fprintf(asmFile, "M=D&M\n");
	/* Else if it is an OR command */
	else if(command == OR)
		/* Use the OR operator on D and M */
		fprintf(asmFile, "M=D|M\n");
}

/* Write the asm code for an arithmetic command */
void writeArithmetic(const char *command)
{
	/* All arithmetics commands */
	static const arithmetic baseArithmeticCommand[NUMBER_ARITHMETIC_COMMAND] =
	{
		{"add", 	writeArithmeticAddSub, 	ADD},
		{"sub", 	writeArithmeticAddSub, 	SUB},
		{"neg", 	writeArithmeticNegNot, 	NEG},
		{"eq", 	writeArithmeticEqGtLt, 	EQ},
		{"gt", 	writeArithmeticEqGtLt, 	GT},
		{"lt", 	writeArithmeticEqGtLt, 	LT},
		{"and", 	writeArithmeticAndOr, 	AND},
		{"or", 	writeArithmeticAndOr, 	OR},
		{"not", 	writeArithmeticNegNot, 	NOT}
	};
	int indexCommand;

	/* Try all arithmetic commands */
	for(indexCommand = 0; indexCommand < NUMBER_ARITHMETIC_COMMAND; ++indexCommand)
	{
		if(strcmp(baseArithmeticCommand[indexCommand].name, command) == 0)
		{
			/* Call the corresponding function */
			baseArithmeticCommand[indexCommand].function(baseArithmeticCommand[indexCommand].commandType);
			/* Increment the SP */
			incrementSP();

			return ;
		}
	}
}

/* Write the asm code for a PUSH command */
void writePush(const char *segment, const int index)
{
	/* The constant segment is a particular one, so it is alone */

	/* Constant segment */
	if(strcmp(segment, "constant") == 0)
	{
		/* Put on the stack the number index */
		fprintf(asmFile, "@%d\n", index);
		fprintf(asmFile, "D=A\n");
		putOnStack();
	}
	/* Argument, local, static, this, that, pointer, temp segment */
	else
	{
		/* Get the SP, and store it in the D register */
		fprintf(asmFile, "@SP\n");
		fprintf(asmFile, "D=M\n");

		/* Write D into the TEMP segment to save it (temp segment -> 5..12) */
		saveInTempSegment();

		/* Get segment[index] and store it in the D register */
		getSegmentIndex(segment, index);

		/* Get the value at the address segment[index]*/
		fprintf(asmFile, "A=D\n");
		fprintf(asmFile, "D=M\n");

		/* Put the data of the D register (SP - 1) on the segment[index] address (the address was store before in temp segment) */
		addressTempSegment();
	}
}

/* Write the asm code for a POP command */
void writePop(const char *segment, const int index)
{
	/* Get segment[index] and store it in the D register */
	getSegmentIndex(segment, index);

	/* Write D into the TEMP segment to save it (temp segment -> 5..12) */
	saveInTempSegment();

	/* Get the SP - 1, and store it in the D register */
	getSP();
	fprintf(asmFile, "D=M\n");

	/* Put the data of the D register (SP - 1) on the segment[index] address (the address was store before in temp segment) */
	addressTempSegment();
}

/* Write the asm code for a PUSH or POP command (handle all 8 segments) */
void writePushPop(const int command, const char *segment, const int index)
{
	/* All 8 segments */
	static const pushPop basePushPopCommand[NUMBER_SEGMENT] =
	{
		{"constant", 	"constant"},
		{"local", 		"LCL"},
		{"argument", 	"ARG"},
		{"this", 		"THIS"},
		{"that", 		"THAT"},
		{"pointer", 	"3"},
		{"temp", 		"5"},
		{"static", 		"static"}
	};
	int indexCommand;

	/* Try all segments */
	for(indexCommand = 0; indexCommand < NUMBER_SEGMENT; ++indexCommand)
	{
		if(strcmp(segment, basePushPopCommand[indexCommand].name) == 0)
		{
			/* If it is a PUSH command */
			if(command == C_PUSH)
			{
				writePush(basePushPopCommand[indexCommand].segment, index);
				/* Increment the SP */
				incrementSP();
			}
			/* Else if it is a POP command */
			else if(command == C_POP)
				writePop(basePushPopCommand[indexCommand].segment, index);

			return ;
		}
	}
}

/* Write the basic asm code in the output file */
void writeInit(void)
{
	/* Init SP to 256 */
	fprintf(asmFile, "@256\n");
	fprintf(asmFile, "D=A\n");
	fprintf(asmFile, "@SP\n");
	fprintf(asmFile, "M=D\n");

	/* Call the main function -> Sys.init */
	writeCall("Sys.init", 0);
}

/* Write the asm code for a LABEL command */
void writeLabel(const char *label)
{
	/* Create the label */
	fprintf(asmFile, "(%s_%d)\n", label, counterFunction);
}

/* Write the asm code for a GOTO command */
void writeGoto(const char *label)
{
	/* Goto the label */
	fprintf(asmFile, "@%s_%d\n", label, counterFunction);
	fprintf(asmFile, "0;JMP\n");
}

/* Write the asm code for a IF-GOTO command */
void writeIf(const char *label)
{
	/* Get the SP - 1, and store it in the D register */
	getSP();
	fprintf(asmFile, "D=M\n");

	/* Goto label JGT is true (JNE -> out != 0) */
	fprintf(asmFile, "@%s_%d\n", label, counterFunction);
	fprintf(asmFile, "D;JNE\n");
}

/* Store the segment in parameter */
void storeSegment(const char *segment)
{
	/* If it is the return-address that we want to store */
	if(strcmp(segment, "return-address") == 0)
	{
		/* We want to push the address of RETURN_ADDRESS not the data in it */
		fprintf(asmFile, "@RETURN_ADDRESS_%d\n", counterFunction);
		fprintf(asmFile, "D=A\n");
	}
	/* Else it is all other segment */
	else
	{
		/* Put in the D register, the data of the segment */
		fprintf(asmFile, "@%s\n", segment);
		fprintf(asmFile, "D=M\n");
	}

	putOnStack();
	incrementSP();
}

/* Repositon ARG segment when calling a function */
void repositionARG(const int numArgs)
{
	/* ARG = SP - n - 5 */

	/* Put in the D register SP - numArgs - 5 */
	fprintf(asmFile, "@SP\n");
	fprintf(asmFile, "D=M\n");
	fprintf(asmFile, "@%d\n", numArgs);
	fprintf(asmFile, "D=D-A\n");
	fprintf(asmFile, "@5\n");
	fprintf(asmFile, "D=D-A\n");

	/* Put in the ARG segment the D register */
	fprintf(asmFile, "@ARG\n");
	fprintf(asmFile, "M=D\n");
}

/* Repositon LCL segment when calling a function */
void repositionLCL(void)
{
	/* LCL = SP */

	fprintf(asmFile, "@SP\n");
	fprintf(asmFile, "D=M\n");

	fprintf(asmFile, "@LCL\n");
	fprintf(asmFile, "M=D\n");
}

/* Write the asm code for a CALL command */
void writeCall(const char *functionName, const int numArgs)
{
	/* Call command ->
		push return-address	// Using the label declared below
		push LCL			// Save LCL of the calling function
		push ARG			// Save ARG of the calling function
		push THIS			// Save THIS of the calling function
		push THAT			// Save THAT of the calling function
		ARG = SP - n - 5		// Reposition ARG (n = number of args)
		LCL = SP			// Reposition LCL
		goto f			// Transfer control
		(return-address)		// Declare a lebel for the return-address
	*/

	/* push return-address on the stack */
	storeSegment("return-address");

	/* push LCL on the stack */
	storeSegment("LCL");
	/* push ARG on the stack */
	storeSegment("ARG");
	/* push THIS on the stack */
	storeSegment("THIS");
	/* push THAT on the stack */
	storeSegment("THAT");

	/* ARG = SP - n - 5 */
	repositionARG(numArgs);

	/* LCL = SP */
	repositionLCL();

	/* Goto f (goto the function) */
	fprintf(asmFile, "@%s\n", functionName);
	fprintf(asmFile, "0;JMP\n");

	/* (return-address) */
	/* Create the label for the return address */
	fprintf(asmFile, "(RETURN_ADDRESS_%d)\n", counterFunction);

	/* Increment the counter function */
	++counterFunction;
}

/* Restore the segment in parameter */
void restoreSegment(const char *segment)
{
	/* Get the address of FRAME - x (x correspond to a segment) (FRAME -> @5) */

	/* We can't juste do FRAME - 1..4 so we do 1 by 1 by decrementing @5 each time (FRAME -> @5) */

	/* Get the address of FRAME - 1 (FRAME -> @5) */
	fprintf(asmFile, "@5\n");
	fprintf(asmFile, "A=M-1\n");
	fprintf(asmFile, "D=M\n");
	/* Put the address in the segment */
	fprintf(asmFile, "@%s\n", segment);
	fprintf(asmFile, "M=D\n");
	/* Decrement FRAME (FRAME -> @5) */
	fprintf(asmFile, "@5\n");
	fprintf(asmFile, "M=M-1\n");
}

/* Write the asm code for a RETURN command */
void writeReturn(void)
{
	/* Return command ->
		FRAME = LCL			// FRAME is a temporary variable
		RET = *(FRAME - 5)		// Put the return-address in a temp. var
		*ARG = pop()			// Reposition the return value for the caller
		SP = ARG + 1			// Restore SP of the caller
		THAT 	= *(FRAME - 1)	// Restore THAT of the caller
		THIS 		= *(FRAME - 2)	// Restore THIS of the caller
		ARG 		= *(FRAME - 3)	// Restore ARG of the caller
		LCL 		= *(FRAME - 4)	// Restore LCL of the caller
		goto RET 				// Goto return-address (in the caller's code)
	*/

	/* FRAME here is @5 (part of the temp segment) */	
	/* RET here is @6 (part of the temp segment) */	

	/* FRAME = LCL (FRAME -> @5) */	
	fprintf(asmFile, "@LCL\n");
	fprintf(asmFile, "D=M\n");
	fprintf(asmFile, "@5\n");
	fprintf(asmFile, "M=D\n");

	/* RET = *(FRAME - 5) (RET -> @6 ; FRAME -> @5) */
	/* Get FRAME value (FRAME -> @5) */
	fprintf(asmFile, "@5\n");
	fprintf(asmFile, "D=M\n");
	/* Substract 5 to this value and get the address */
	fprintf(asmFile, "@5\n");
	fprintf(asmFile, "A=D-A\n");
	fprintf(asmFile, "D=M\n");
	/* Store the address in RET (RET -> @6) */
	fprintf(asmFile, "@6\n");
	fprintf(asmFile, "M=D\n");

	/* *ARG = pop() (pop is the last value on the stack) */
	getSP();
	fprintf(asmFile, "D=M\n");
	/* We want the address of ARG so we use A = M, M = D and not M = D */
	fprintf(asmFile, "@ARG\n");
	fprintf(asmFile, "A=M\n");
	fprintf(asmFile, "M=D\n");

	/* SP = ARG + 1 */
	fprintf(asmFile, "@ARG\n");
	fprintf(asmFile, "D=M\n");
	fprintf(asmFile, "@SP\n");
	fprintf(asmFile, "M=D+1\n");

	/* Restore THAT segment */
	restoreSegment("THAT");
	/* Restore THIS segment */
	restoreSegment("THIS");
	/* Restore ARG segment */
	restoreSegment("ARG");
	/* Restore LCL segment */
	restoreSegment("LCL");

	/* Goto RET (RET -> @6) */
	/* We don't want to jump to @6, we want to jump to the address in @6, so we put in the M register the address in @6 */
	fprintf(asmFile, "@6\n");
	fprintf(asmFile, "A=M\n");
	/* We didn't put an address in the A register so the jump will select the address in the M register */
	fprintf(asmFile, "0;JMP\n");
}

/* Write the asm code for a FUNCTION command */
void writeFunction(const char *functionName, const int numLocals)
{
	/* Function command -> 
		(f)				// Declare a label for the function entry
			repeat k times :	// k = number of local variables
			push 0		// Initialize all of them to 0
	*/

	/* If the function doesn't have any local variable, just declare the label */
	if(numLocals == 0)
		fprintf(asmFile, "(%s)\n", functionName);
  	else
  	{
  		/* Get the numLocals */
  		fprintf(asmFile, "@%d\n", numLocals);
  		fprintf(asmFile, "D=A\n");
  		/* Create the label for the function */
  		fprintf(asmFile, "(%s)\n", functionName);
  		/* Init the local variable of index D to 0 */
  		incrementSP();
  		fprintf(asmFile, "A=M-1\n");
  		fprintf(asmFile, "M=0\n");
  		/* While D > 0 (while there is still local variable to init), goto the function's label */
  		fprintf(asmFile, "@%s\n", functionName);
  		fprintf(asmFile, "D=D-1;JGT\n");
     	}
}

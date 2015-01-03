#ifndef CODE_WRITER_H
#define CODE_WRITER_H

#define NUMBER_ARITHMETIC_COMMAND 	9
#define NUMBER_SEGMENT 			8

enum arithmeticCommand
{
	ADD 	= 0,
	SUB 	= 1,
	NEG 	= 2,
	EQ 	= 3,
	GT 	= 4,
	LT 	= 5,
	AND 	= 6,
	OR 	= 7,
	NOT 	= 8
};

typedef struct arithmetic arithmetic;
struct arithmetic
{
	const char *name;
	void (*function)(const int command);
	const int commandType;
};

typedef struct pushPop pushPop;
struct pushPop
{
	const char *name;
	const char *segment;
};	

/* Increment the SP */
void incrementSP(void);
/* Get the SP - 1 number on the stack */
void getSP(void);
/* Put the D register on the stack */
void putOnStack(void);
/* Get segment[index] and store it in the D register */
void getSegmentIndex(const char *segment, const int index);
/* Write D into the TEMP segment to save it (temp segment -> 5..12) */
void saveInTempSegment(void);
/* Put the D register value in the address store in temp segment */
void addressTempSegment(void);


/* Write the asm code for a ADD or SUB command */
void writeArithmeticAddSub(const int command);
/* Write the asm code for a NEG or NOT command */
void writeArithmeticNegNot(const int command);
/* Write the asm code for a EQ or GT or LT command */
void writeArithmeticEqGtLt(const int command);
/* Write the asm code for an AND or OR command */
void writeArithmeticAndOr(const int command);
/* Write the asm code for an arithmetic command */
void writeArithmetic(const char *command);


/* Write the asm code for a PUSH command */
void writePush(const char *segment, const int index);
/* Write the asm code for a POP command */
void writePop(const char *segment, const int index);
/* Write the asm code for a PUSH or POP command (handle all 8 segments) */
void writePushPop(const int command, const char *segment, const int index);


/* Write the basic asm code in the output file */
void writeInit(void);


/* Write the asm code for a LABEL command */
void writeLabel(const char *label);
/* Write the asm code for a GOTO command */
void writeGoto(const char *label);
/* Write the asm code for a IF-GOTO command */
void writeIf(const char *label);


/* Store the segment in parameter */
void storeSegment(const char *segment);
/* Repositon ARG segment when calling a function */
void repositionARG(const int numArgs);
/* Repositon LCL segment when calling a function */
void repositionLCL(void);
/* Write the asm code for a CALL command */
void writeCall(const char *functionName, const int numArgs);


/* Restore the segment in parameter */
void restoreSegment(const char *segment);
/* Write the asm code for a RETURN command */
void writeReturn(void);


/* Write the asm code for a FUNCTION command */
void writeFunction(const char *functionName, const int numLocals);

#endif

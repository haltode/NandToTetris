// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)

// Ram[2] = 0
@2
M = 0

// counter = 0
@counter
M = 0

(LOOP)

    // D = counter
    @counter
    D = M
    
    // D = D - RAM[1]
    @1
    D = D - M

    // If D = 0, goto END
    @END
    D ; JEQ

    // counter = counter + 1
    @counter
    M = M + 1

    // D = RAM[0]
    @0
    D = M

    // D = D + RAM[2]
    @2
    D = D + M

    // RAM[2] = D
    @2
    M = D

    // Goto the beginning of the loop
    @LOOP
    0 ; JMP

(END)

    // Infinite loop
    @END
    0 ; JMP
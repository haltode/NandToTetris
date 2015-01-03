// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/04/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, the
// program clears the screen, i.e. writes "white" in every pixel.

// Put the first screen's pixel black if a key is pressed

(LOOP)

    // @KBD point to the keyboard, return the ASCII
    // number of the key pressed or 0 if any key has
    // been pressed
    @KBD
    D = M

    // If D > 0 (if a key has been pressed), print the
    // black pixel
    @PRINT
    D ; JGT

    // Else we clear the screen
    @CLEAR
    0 ; JMP

(PRINT)

    // Put the first screen's pixel to 1 (black)
    @SCREEN
    M = 1

    // Goto LOOP
    @LOOP
    0 ; JMP

(CLEAR)

    // Put the first screen's pixel to 0 (white)
    @SCREEN
    M = 0

    // Goto LOOP
    @LOOP
    0 ; JMP
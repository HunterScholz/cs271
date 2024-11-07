// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

(SETUP)
    @SCREEN
    D=A
    @0
    M=D

(KEY)
    @KBD
    D=M
    @BLACK
    D;JGT
    @WHITE
    D;JEQ

(BLACK)
    @0
    A=M
    M=-1
    @0
    D=M+1
    @KBD
    D=A-D
    @0
    M=M+1
    A=M
    @EXIT
    0;JMP

(WHITE)
    @0
    A=M
    M=0
    @0
    D=M+1
    @KBD
    D=A-D
    @0
    M=M+1
    A=M
    @EXIT
    0;JMP

(EXIT)
    @0
    D=M
    @KEY
    D;JGE
    @SETUP
    D;JLT

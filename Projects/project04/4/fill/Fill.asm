// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Fill.asm

// Runs an infinite loop that listens to the keyboard input. 
// When a key is pressed (any key), the program blackens the screen,
// i.e. writes "black" in every pixel. When no key is pressed, 
// the screen should be cleared.

(RESTART)
@SCREEN //point to start of screen
D=A
@0
M=D //initialize R0 to point to start of screen

(KBDCHECK)
@KBD //point to keyboard
D=M //read keyboard
@BLACK
D;JGT //if key pressed, jump to BLACK
@WHITE
D;JEQ //if no key pressed, jump to WHITE

@KBDCHECK //repeat check
0;JMP

(BLACK)
@1 //color address
M=-1 //set color to BLACK
@CHANGE
0;JMP //jump to CHANGE

(WHITE)
@1 //color address
M=0 //set color to WHITE
@CHANGE
0;JMP //jump to CHANGE

(CHANGE)
@1 //check color address
D=M

@0
A=M //point to current pixel
M=D //set pixel to color

@0
D=M+1 //increment pixel address
@KBD
D=A-D //KBD - SCREEN = a 512*256 = 131072/16=8192 which is the amount of registers to write to the screen 24576 - 16384 = 8192

@0
M=M+1 //increment pixel address
A=M

@CHANGE
D;JGT //if not at end of screen, repeat

@RESTART //if at end of screen, restart
0;JMP
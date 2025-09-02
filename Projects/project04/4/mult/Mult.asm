// This file is part of www.nand2tetris.org
// and the book "The Elements of Computing Systems"
// by Nisan and Schocken, MIT Press.
// File name: projects/4/Mult.asm

// Multiplies R0 and R1 and stores the result in R2.
// (R0, R1, R2 refer to RAM[0], RAM[1], and RAM[2], respectively.)
// The algorithm is based on repetitive addition.

@2 //jump to R2
M=0 //set R2 to 0
@0 //jump to R0
D=M
@END
D;JEQ //checks if R0 is 0, if so jump to END
@1 //jump to R1
D=M
@END //checks if R1 is 0, if so jump to END
D;JEQ
(LOOP)
@0 //jump to R0
D=M
@2 //jump to R2
M=M+D //set R2 to R0
@1 //jump to R1
D=M-1 //decrement R1 by 1
M=D //store new value of R1
@LOOP //if R1 is not 0, repeat loop
D;JGT

(END)
@END //infinite loop
0;JMP
// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1

// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1

//eq
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
@TRUE_1
D;JEQ
@SP
A=M-1
M=0
@END_2
0;JMP
(TRUE_1)
@SP
A=M-1
M=-1
(END_2)

// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1

// push constant 16
@16
D=A
@SP
A=M
M=D
@SP
M=M+1

//eq
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
@TRUE_3
D;JEQ
@SP
A=M-1
M=0
@END_4
0;JMP
(TRUE_3)
@SP
A=M-1
M=-1
(END_4)

// push constant 16
@16
D=A
@SP
A=M
M=D
@SP
M=M+1

// push constant 17
@17
D=A
@SP
A=M
M=D
@SP
M=M+1

//eq
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
@TRUE_5
D;JEQ
@SP
A=M-1
M=0
@END_6
0;JMP
(TRUE_5)
@SP
A=M-1
M=-1
(END_6)

// push constant 892
@892
D=A
@SP
A=M
M=D
@SP
M=M+1

// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1

//lt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
@TRUE_7
D;JLT
@SP
A=M-1
M=0
@END_8
0;JMP
(TRUE_7)
@SP
A=M-1
M=-1
(END_8)

// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1

// push constant 892
@892
D=A
@SP
A=M
M=D
@SP
M=M+1

//lt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
@TRUE_9
D;JLT
@SP
A=M-1
M=0
@END_10
0;JMP
(TRUE_9)
@SP
A=M-1
M=-1
(END_10)

// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1

// push constant 891
@891
D=A
@SP
A=M
M=D
@SP
M=M+1

//lt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
@TRUE_11
D;JLT
@SP
A=M-1
M=0
@END_12
0;JMP
(TRUE_11)
@SP
A=M-1
M=-1
(END_12)

// push constant 32767
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1

// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1

//gt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
@TRUE_13
D;JGT
@SP
A=M-1
M=0
@END_14
0;JMP
(TRUE_13)
@SP
A=M-1
M=-1
(END_14)

// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1

// push constant 32767
@32767
D=A
@SP
A=M
M=D
@SP
M=M+1

//gt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
@TRUE_15
D;JGT
@SP
A=M-1
M=0
@END_16
0;JMP
(TRUE_15)
@SP
A=M-1
M=-1
(END_16)

// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1

// push constant 32766
@32766
D=A
@SP
A=M
M=D
@SP
M=M+1

//gt
@SP
AM=M-1
D=M
@SP
A=M-1
D=M-D
@TRUE_17
D;JGT
@SP
A=M-1
M=0
@END_18
0;JMP
(TRUE_17)
@SP
A=M-1
M=-1
(END_18)

// push constant 57
@57
D=A
@SP
A=M
M=D
@SP
M=M+1

// push constant 31
@31
D=A
@SP
A=M
M=D
@SP
M=M+1

// push constant 53
@53
D=A
@SP
A=M
M=D
@SP
M=M+1

//add
@SP
AM=M-1
D=M
@SP
A=M-1
M=M+D

// push constant 112
@112
D=A
@SP
A=M
M=D
@SP
M=M+1

//sub
@SP
AM=M-1
D=M
@SP
A=M-1
M=M-D

//neg
@SP
A=M-1
M=-M

//and
@SP
AM=M-1
D=M
@SP
A=M-1
M=M&D

// push constant 82
@82
D=A
@SP
A=M
M=D
@SP
M=M+1

//or
@SP
AM=M-1
D=M
@SP
A=M-1
M=M|D

//not
@SP
A=M-1
M=!M


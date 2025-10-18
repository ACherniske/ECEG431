#include "codewriter.h"
#include <stdexcept>
#include <iostream>

CodeWriter::CodeWriter(const std::string& outputFileName) 
    : labelCounter(0), callCounter(0), currentFunction("") {
    outputFile.open(outputFileName);
    if (!outputFile.is_open()) {
        throw std::runtime_error("Could not open output file: " + outputFileName);
    }
}

CodeWriter::~CodeWriter() {
    close();
}

void CodeWriter::setFileName(const std::string& fileName) {
    size_t lastSlash = fileName.find_last_of("/\\");
    size_t lastDot = fileName.find_last_of('.');

    if (lastSlash != std::string::npos) {
        if (lastDot != std::string::npos && lastDot > lastSlash) {
            currentFileName = fileName.substr(lastSlash + 1, lastDot - lastSlash - 1);
        } else {
            currentFileName = fileName.substr(lastSlash + 1);
        }
    } else {
        if (lastDot != std::string::npos) {
            currentFileName = fileName.substr(0, lastDot);
        } else {
            currentFileName = fileName;
        }
    }
}

std::string CodeWriter::generateLabel(const std::string& prefix) { //generate unique label ex. TRUE_1, END_2
    return prefix + "_" + std::to_string(++labelCounter);
}

void CodeWriter::writeArithmetic(const std::string& command) {
    /**
     * Writes the assembly code that is the translation of the given arithmetic command.
     * Valid commands are: add, sub, neg, eq, gt, lt, and, or, not
     * @param command the arithmetic command to translate
     */
    outputFile << "//" << command << std::endl;

    if (command == "add") {
        outputFile << "@SP\n"
                   << "AM=M-1\n" //SP = SP - 1, A = SP
                   << "D=M\n"
                   << "@SP\n"
                   << "A=M-1\n" //A = SP - 1
                   << "M=M+D\n"; //add top two stack values
    } else if (command == "sub") {
        outputFile << "@SP\n"
                   << "AM=M-1\n"
                   << "D=M\n"
                   << "@SP\n"
                   << "A=M-1\n"
                   << "M=M-D\n";
    } else if (command == "neg") {
        outputFile << "@SP\n"
                   << "A=M-1\n"
                   << "M=-M\n";
    } else if (command == "and") {
        outputFile << "@SP\n"
                   << "AM=M-1\n"
                   << "D=M\n"
                   << "@SP\n"
                   << "A=M-1\n"
                   << "M=M&D\n";
    } else if (command == "or") {
        outputFile << "@SP\n"
                   << "AM=M-1\n"
                   << "D=M\n"
                   << "@SP\n"
                   << "A=M-1\n"
                   << "M=M|D\n";
    } else if (command == "not") {
        outputFile << "@SP\n"
                   << "A=M-1\n"
                   << "M=!M\n";
    } else if (command == "eq") {
        writeComparison("JEQ");
    } else if (command == "gt") {
        writeComparison("JGT");
    } else if (command == "lt") {
        writeComparison("JLT");
    } else {
        throw std::runtime_error("Unknown arithmetic command: " + command);
    }

    outputFile << std::endl;
}

void CodeWriter::writeComparison(const std::string& jumpType) {
    /**
     * Writes the assembly code that is the translation of the given comparison command.
     * @param jumpType the type of jump to perform (JEQ, JGT, JLT)
     */
    std::string labelTrue = generateLabel("TRUE");
    std::string labelEnd = generateLabel("END");

    outputFile << "@SP\n"
               << "AM=M-1\n"
               << "D=M\n"
               << "@SP\n"
               << "A=M-1\n"
               << "D=M-D\n"
               << "@" << labelTrue << "\n"
               << "D;" << jumpType << "\n"
               << "@SP\n"
               << "A=M-1\n"
               << "M=0\n"
               << "@" << labelEnd << "\n"
               << "0;JMP\n"
               << "(" << labelTrue << ")\n"
               << "@SP\n"
               << "A=M-1\n"
               << "M=-1\n"
               << "(" << labelEnd << ")\n";
}

void CodeWriter::writePushPop(const std::string& command, const std::string& segment, int index) {
    /**
     * Writes the assembly code that is the translation of the given command, 
     * where command is either C_PUSH or C_POP.
     * @param command the command type ("push" or "pop")
     * @param segment the memory segment to operate on
     * @param index the index within the segment
     */
    if (command == "push") {
        outputFile << "// push " << segment << " " << index << std::endl;

        if (segment == "constant") {
            outputFile << "@" << index << "\n"
                       << "D=A\n"
                       << "@SP\n"
                       << "A=M\n"
                       << "M=D\n"
                       << "@SP\n"
                       << "M=M+1\n";
        } else if (segment == "local") {
            writePushSegment("LCL", index);
        } else if (segment == "argument") {
            writePushSegment("ARG", index);
        } else if (segment == "this") {
            writePushSegment("THIS", index);
        } else if (segment == "that") {
            writePushSegment("THAT", index);
        } else if (segment == "temp") {
            outputFile << "@" << (5 + index) << "\n"
                       << "D=M\n"
                       << "@SP\n"
                       << "A=M\n"
                       << "M=D\n"
                       << "@SP\n"
                       << "M=M+1\n";
        } else if (segment == "static") {
            outputFile << "@" << currentFileName << "." << index << "\n"
                       << "D=M\n"
                       << "@SP\n"
                       << "A=M\n"
                       << "M=D\n"
                       << "@SP\n"
                       << "M=M+1\n";
        } else if (segment == "pointer") {
            if (index == 0) {
                outputFile << "@THIS\n";
            } else {
                outputFile << "@THAT\n";
            }
            outputFile << "D=M\n"
                       << "@SP\n"
                       << "A=M\n"
                       << "M=D\n"
                       << "@SP\n"
                       << "M=M+1\n";
        } else {
            throw std::runtime_error("Unknown segment for push: " + segment);
        }
    } else if (command == "pop") {
        outputFile << "// pop " << segment << " " << index << std::endl;

        if (segment == "local") {
            writePopSegment("LCL", index);
        } else if (segment == "argument") {
            writePopSegment("ARG", index);
        } else if (segment == "this") {
            writePopSegment("THIS", index);
        } else if (segment == "that") {
            writePopSegment("THAT", index);
        } else if (segment == "temp") {
            outputFile << "@SP\n"
                       << "AM=M-1\n"
                       << "D=M\n"
                       << "@" << (5 + index) << "\n"
                       << "M=D\n";
        } else if (segment == "static") {
            outputFile << "@SP\n"
                       << "AM=M-1\n"
                       << "D=M\n"
                       << "@" << currentFileName << "." << index << "\n"
                       << "M=D\n";
        } else if (segment == "pointer") {
            outputFile << "@SP\n"
                       << "AM=M-1\n"
                       << "D=M\n";
            if (index == 0) {
                outputFile << "@THIS\n";
            } else {
                outputFile << "@THAT\n";
            }
            outputFile << "M=D\n";
        } else {
            throw std::runtime_error("Unknown segment for pop: " + segment);
        }
    }

    outputFile << std::endl;
}

void CodeWriter::writePushSegment(const std::string& segment, int index) {
    /**
     * Helper function to write push commands for segments that use base pointers.
     * @param segment the base segment pointer (LCL, ARG, THIS, THAT)
     * @param index the index within the segment
     */
    outputFile << "@" << segment << "\n"
               << "D=M\n"
               << "@" << index << "\n"
               << "A=D+A\n"
               << "D=M\n"
               << "@SP\n"
               << "A=M\n"
               << "M=D\n"
               << "@SP\n"
               << "M=M+1\n";
}

void CodeWriter::writePopSegment(const std::string& segment, int index) {
    /**
     * Helper function to write pop commands for segments that use base pointers.
     * @param segment the base segment pointer (LCL, ARG, THIS, THAT)
     * @param index the index within the segment
     */
    outputFile << "@" << segment << "\n"
               << "D=M\n"
               << "@" << index << "\n"
               << "D=D+A\n"
               << "@R13\n"
               << "M=D\n"
               << "@SP\n"
               << "AM=M-1\n"
               << "D=M\n"
               << "@R13\n"
               << "A=M\n"
               << "M=D\n";
}

//chapter 8 methods

void CodeWriter::writeInit() {
    /**
     * Writes the assembly code that effects the VM initialization,
     * also called bootstrap code. This code must be placed at the
     * beginning of the output file.
     */
    outputFile << "// Bootstrap code\n"
               << "@256\n"
               << "D=A\n"
               << "@SP\n"
               << "M=D\n";
    
    writeCall("Sys.init", 0); //call Sys.init with 0 args
}

void CodeWriter::writeLabel(const std::string& label) {
    /**
     * Writes the assembly code that is the translation of the given label command.
     * The label is function-scoped and uses the format functionName$label.
     * Example: (SimpleFunction$LOOP)
     * @param label the label to declare
     */
    outputFile << "// label " << label << std::endl;
    outputFile << "(" << currentFunction << "$" << label << ")\n"; //functionName$label ex. SimpleFunction$LOOP
    outputFile << std::endl;
}

void CodeWriter::writeGoto(const std::string& label) {
    /**
     * Writes the assembly code that is the translation of the given goto command.
     * Unconditional jump to a function-scoped label using the format functionName$label.
     * Example: @SimpleFunction$LOOP 0;JMP
     * @param label the label to go to
     */
    outputFile << "// goto " << label << std::endl;
    outputFile << "@" << currentFunction << "$" << label << "\n" //functionName$label
               << "0;JMP\n";
    outputFile << std::endl;
}

void CodeWriter::writeIf(const std::string& label) {
    /**
     * Writes the assembly code that is the translation of the given if-goto command.
     * Conditional jump to a function-scoped label using the format functionName$label.
     * Example: @SimpleFunction$LOOP D;JNE
     * @param label the label to go to if top stack value != 0
     */
    outputFile << "// if-goto " << label << std::endl;
    outputFile << "@SP\n"
               << "AM=M-1\n"
               << "D=M\n"
               << "@" << currentFunction << "$" << label << "\n" //functionName$label
               << "D;JNE\n";
    outputFile << std::endl;
}

void CodeWriter::writeCall(const std::string& functionName, int numArgs) {
    /**
     * Writes the assembly code that is the translation of the given call command.
     * 
     * process: 
     * push return-address; //unique label for return address
     * push LCL; //save LCL of caller for later restoration
     * push ARG; //save ARG of caller for later restoration
     * push THIS; //save THIS of caller for later restoration
     * push THAT; //save THAT of caller for later restoration
     * ARG = SP-n-5; //reposition ARG for callee
     * LCL = SP;  //reposition LCL for callee
     * goto functionName; (return-address) //transfer control to callee
     * 
     * @param functionName the name of the function to call ex. Sys.init
     * @param numArgs the number of arguments to pass to the function
     */
    std::string returnLabel = "RETURN_" + std::to_string(++callCounter); //unique return label
    
    outputFile << "// call " << functionName << " " << numArgs << std::endl;
    
    // Push return address to stack
    outputFile << "@" << returnLabel << "\n"
               << "D=A\n"
               << "@SP\n"
               << "A=M\n"
               << "M=D\n"
               << "@SP\n"
               << "M=M+1\n";
    
    // Push LCL to save caller's LCL for later restoration
    outputFile << "@LCL\n"
               << "D=M\n"
               << "@SP\n"
               << "A=M\n"
               << "M=D\n"
               << "@SP\n"
               << "M=M+1\n";
    
    // Push ARG
    outputFile << "@ARG\n"
               << "D=M\n"
               << "@SP\n"
               << "A=M\n"
               << "M=D\n"
               << "@SP\n"
               << "M=M+1\n";
    
    // Push THIS
    outputFile << "@THIS\n"
               << "D=M\n"
               << "@SP\n"
               << "A=M\n"
               << "M=D\n"
               << "@SP\n"
               << "M=M+1\n";
    
    // Push THAT
    outputFile << "@THAT\n"
               << "D=M\n"
               << "@SP\n"
               << "A=M\n"
               << "M=D\n"
               << "@SP\n"
               << "M=M+1\n";
    
    // ARG = SP - n - 5
    outputFile << "@SP\n" //get current SP
               << "D=M\n" //D = SP
               << "@" << (numArgs + 5) << "\n" // subtract (numArgs + 5) from SP: SP - numArgs - 5
               << "D=D-A\n" // D = SP - numArgs - 5
               << "@ARG\n" //set ARG 
               << "M=D\n"; //ARG = SP - n - 5 ARG now points to base of args for callee
    
    // LCL = SP
    outputFile << "@SP\n" //reposition LCL for callee
               << "D=M\n" //D = SP
               << "@LCL\n" //set LCL
               << "M=D\n"; //LCL = SP
    
    // goto functionName
    outputFile << "@" << functionName << "\n"
               << "0;JMP\n";
    
    // (return address)
    outputFile << "(" << returnLabel << ")\n";
    outputFile << std::endl;
}

void CodeWriter::writeReturn() {
    /**
     * Writes the assembly code that is the translation of the given return command.
     * 
     * process:
     * FRAME = LCL; //FRAME is a temporary variable
     * RET = *(FRAME-5); //get return address
     * *ARG = pop(); //reposition return value for caller
     * SP = ARG + 1; //restore SP of caller
     * THAT = *(FRAME-1); //restore THAT of caller
     * THIS = *(FRAME-2); //restore THIS of caller
     * ARG = *(FRAME-3); //restore ARG of caller
     * LCL = *(FRAME-4); //restore LCL of caller
     * goto RET; //goto return address
     */
    outputFile << "// return\n";
    
    // FRAME = LCL (using R13 as FRAME)
    outputFile << "@LCL\n"
               << "D=M\n"
               << "@R13\n"
               << "M=D\n";
    
    // RET = *(FRAME-5) (using R14 as RET)
    outputFile << "@5\n"
               << "A=D-A\n"
               << "D=M\n"
               << "@R14\n"
               << "M=D\n";
    
    // *ARG = pop()
    outputFile << "@SP\n"
               << "AM=M-1\n"
               << "D=M\n"
               << "@ARG\n"
               << "A=M\n"
               << "M=D\n";
    
    // SP = ARG + 1
    outputFile << "@ARG\n"
               << "D=M+1\n"
               << "@SP\n"
               << "M=D\n";
    
    // THAT = *(FRAME-1)
    outputFile << "@R13\n"
               << "AM=M-1\n"
               << "D=M\n"
               << "@THAT\n"
               << "M=D\n";
    
    // THIS = *(FRAME-2)
    outputFile << "@R13\n"
               << "AM=M-1\n"
               << "D=M\n"
               << "@THIS\n"
               << "M=D\n";
    
    // ARG = *(FRAME-3)
    outputFile << "@R13\n"
               << "AM=M-1\n"
               << "D=M\n"
               << "@ARG\n"
               << "M=D\n";
    
    // LCL = *(FRAME-4)
    outputFile << "@R13\n"
               << "AM=M-1\n"
               << "D=M\n"
               << "@LCL\n"
               << "M=D\n";
    
    // goto RET
    outputFile << "@R14\n"
               << "A=M\n"
               << "0;JMP\n";
    outputFile << std::endl;
}

void CodeWriter::writeFunction(const std::string& functionName, int numLocals) {
    /**
     * Writes the assembly code that is the translation of the given function command.
     * 
     * process:
     * (functionName) //declare function label
     * repeat numLocals times:
     *     push 0 //initialize local variables to 0
     * 
     * @param functionName the name of the function
     * @param numLocals the number of local variables to initialize
     */
    currentFunction = functionName;
    
    outputFile << "// function " << functionName << " " << numLocals << std::endl;
    outputFile << "(" << functionName << ")\n";
    
    //initialize local variables to 0 by pushing 0 onto stack numLocals times
    for (int i = 0; i < numLocals; i++) {
        outputFile << "@SP\n" //push 0
                   << "A=M\n" //get address at SP
                   << "M=0\n" //set that address to 0
                   << "@SP\n" //increment SP
                   << "M=M+1\n"; //SP = SP + 1
    }
    outputFile << std::endl;
}

void CodeWriter::close() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
}
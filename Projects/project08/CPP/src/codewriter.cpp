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

std::string CodeWriter::generateLabel(const std::string& prefix) {
    return prefix + "_" + std::to_string(++labelCounter);
}

void CodeWriter::writeArithmetic(const std::string& command) {
    outputFile << "//" << command << std::endl;

    if (command == "add") {
        outputFile << "@SP\n"
                   << "AM=M-1\n"
                   << "D=M\n"
                   << "@SP\n"
                   << "A=M-1\n"
                   << "M=M+D\n";
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
    outputFile << "// Bootstrap code\n"
               << "@256\n"
               << "D=A\n"
               << "@SP\n"
               << "M=D\n";
    
    writeCall("Sys.init", 0);
}

void CodeWriter::writeLabel(const std::string& label) {
    outputFile << "// label " << label << std::endl;
    outputFile << "(" << currentFunction << "$" << label << ")\n";
    outputFile << std::endl;
}

void CodeWriter::writeGoto(const std::string& label) {
    outputFile << "// goto " << label << std::endl;
    outputFile << "@" << currentFunction << "$" << label << "\n"
               << "0;JMP\n";
    outputFile << std::endl;
}

void CodeWriter::writeIf(const std::string& label) {
    outputFile << "// if-goto " << label << std::endl;
    outputFile << "@SP\n"
               << "AM=M-1\n"
               << "D=M\n"
               << "@" << currentFunction << "$" << label << "\n"
               << "D;JNE\n";
    outputFile << std::endl;
}

void CodeWriter::writeCall(const std::string& functionName, int numArgs) {
    std::string returnLabel = "RETURN_" + std::to_string(++callCounter);
    
    outputFile << "// call " << functionName << " " << numArgs << std::endl;
    
    // Push return address
    outputFile << "@" << returnLabel << "\n"
               << "D=A\n"
               << "@SP\n"
               << "A=M\n"
               << "M=D\n"
               << "@SP\n"
               << "M=M+1\n";
    
    // Push LCL
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
    outputFile << "@SP\n"
               << "D=M\n"
               << "@" << (numArgs + 5) << "\n"
               << "D=D-A\n"
               << "@ARG\n"
               << "M=D\n";
    
    // LCL = SP
    outputFile << "@SP\n"
               << "D=M\n"
               << "@LCL\n"
               << "M=D\n";
    
    // goto functionName
    outputFile << "@" << functionName << "\n"
               << "0;JMP\n";
    
    // (return address)
    outputFile << "(" << returnLabel << ")\n";
    outputFile << std::endl;
}

void CodeWriter::writeReturn() {
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
    currentFunction = functionName;
    
    outputFile << "// function " << functionName << " " << numLocals << std::endl;
    outputFile << "(" << functionName << ")\n";
    
    //initialize local variables to 0
    for (int i = 0; i < numLocals; i++) {
        outputFile << "@SP\n"
                   << "A=M\n"
                   << "M=0\n"
                   << "@SP\n"
                   << "M=M+1\n";
    }
    outputFile << std::endl;
}

void CodeWriter::close() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
}
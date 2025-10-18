#include "vm7codewriter.h"
#include <stdexcept>
#include <iostream>

CodeWriter::CodeWriter(const std::string& outputFileName) :labelCounter(0) {
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

std::string CodeWriter::generateLabel(const std::string& prefix) { //ex. TRUE_1, END_2, etc.
    return prefix + "_" + std::to_string(++labelCounter);
}

void CodeWriter::writeArithmetic(const std::string& command) {
    /**
     * Writes the assembly code for arithmetic commands.
     * @param command The arithmetic command to translate (e.g., "add", "sub", "neg", "eq", "gt", "lt", "and", "or", "not").
     */
    outputFile << "//" << command << std::endl;

    if (command == "add") {
        outputFile << "@SP\n" 
                   << "AM=M-1\n" //SP = SP - 1; A = SP
                   << "D=M\n" 
                   << "@SP\n"
                   << "A=M-1\n" //A = SP - 1
                   << "M=M+D\n"; //* M[SP-1] = M[SP-1] + D */
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
     * Writes the assembly code for comparison commands (eq, gt, lt).
     * @param jumpType The jump condition (JEQ, JGT, JLT).
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
     * Writes the assembly code for push and pop commands.
     * lines of form "push segment index" or "pop segment index".
     * @param command The command type ("push" or "pop").
     * @param segment The memory segment to operate on. ex. "local", "argument", "this", "that", "constant", "static", "temp", "pointer".
     * @param index The index within the segment.
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
     * Writes the assembly code to push from a segment.
     * lines of form "push segment index".
     * @param segment The base segment pointer (LCL, ARG, THIS, THAT).
     * @param index The index within the segment.
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
     * Writes the assembly code to pop to a segment.
     * lines of form "pop segment index".
     * @param segment The base segment pointer (LCL, ARG, THIS, THAT).
     * @param index The index within the segment.
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

void CodeWriter::close() {
    if (outputFile.is_open()) {
        outputFile.close();
    }
}
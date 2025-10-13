#include "VMWriter.h"
#include <stdexcept>

VMWriter::VMWriter(const std::string& outputFile) {
    output.open(outputFile);
    if (!output.is_open()) {
        throw std::runtime_error("Could not open file: " + outputFile);
    }
}

VMWriter::~VMWriter() {
    if (output.is_open()) {
        output.close();
    }
}

std::string VMWriter::segmentToString(Segment segment) {
    switch (segment) {
        case Segment::CONST: return "constant";
        case Segment::ARG: return "argument";
        case Segment::LOCAL: return "local";
        case Segment::STATIC: return "static";
        case Segment::THIS: return "this";
        case Segment::THAT: return "that";
        case Segment::POINTER: return "pointer";
        case Segment::TEMP: return "temp";
        default: throw std::runtime_error("Invalid segment");
    }
    return "";
}

std::string VMWriter::commandToString(Command command) {
    switch (command) {
        case Command::ADD: return "add";
        case Command::SUB: return "sub";
        case Command::NEG: return "neg";
        case Command::EQ: return "eq";
        case Command::GT: return "gt";
        case Command::LT: return "lt";
        case Command::AND: return "and";
        case Command::OR: return "or";
        case Command::NOT: return "not";
        default: throw std::runtime_error("Invalid command");
    }
    return "";
}

void VMWriter::writePush(Segment segment, int index) {
    output << "push " << segmentToString(segment) << " " << index << "\n";
}

void VMWriter::writePop(Segment segment, int index) {
    output << "pop " << segmentToString(segment) << " " << index << "\n";
}

void VMWriter::writeArithmetic(Command command) {
    output << commandToString(command) << "\n";
}

void VMWriter::writeLabel(const std::string& label) {
    output << "label " << label << "\n";
}

void VMWriter::writeGoto(const std::string& label) {
    output << "goto " << label << "\n";
}

void VMWriter::writeIf(const std::string& label) {
    output << "if-goto " << label << "\n";
}

void VMWriter::writeCall(const std::string& name, int nArgs) {
    output << "call " << name << " " << nArgs << "\n";
}

void VMWriter::writeFunction(const std::string& name, int nLocals) {
    output << "function " << name << " " << nLocals << "\n";
}

void VMWriter::writeReturn() {
    output << "return\n";
}
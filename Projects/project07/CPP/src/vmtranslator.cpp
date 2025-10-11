#include "vm7translator.h"
#include <iostream>

VMTranslator::VMTranslator(const std::string& inputFile, const std::string& outputFile, bool verbose)
    : parser(inputFile), codeWriter(outputFile), verbose(verbose) {
    codeWriter.setFileName(inputFile);
}

void VMTranslator::verboseOutput(const std::string& message) {
    if (verbose) {
        std::cout << message << std::endl;
    }
}

void VMTranslator::translate() {
    verboseOutput("VM Translator starting...");
    verboseOutput("Parser: Loaded " + std::to_string(parser.getLines().size()) + " valid commands");

    int lineNum = 0;
    while (parser.hasMoreCommands()) {
        parser.advance();
        lineNum++;

        CommandType type = parser.commandType();

        if (type == CommandType::C_ARITHMETIC) {
            std::string command = parser.arg1();
            verboseOutput("Parser: Processing line " + std::to_string(lineNum) + ": " + command);
            verboseOutput("CodeWriter: Translating arithmetic '" + command + "'");
            codeWriter.writeArithmetic(command);
        } else if (type == CommandType::C_PUSH) {
            std::string segment = parser.arg1();
            int index = parser.arg2();
            verboseOutput("Parser: Processing line " + std::to_string(lineNum) + ": push " + segment + " " + std::to_string(index));
            verboseOutput("CodeWriter: Translating push " + segment + " " + std::to_string(index));
            codeWriter.writePushPop("push", segment, index);
        } else if (type == CommandType::C_POP) {
            std::string segment = parser.arg1();
            int index = parser.arg2();
            verboseOutput("Parser: Processing line " + std::to_string(lineNum) + ": pop " + segment + " " + std::to_string(index));
            verboseOutput("CodeWriter: Translating pop " + segment + " " + std::to_string(index));
            codeWriter.writePushPop("pop", segment, index);
        } else {
            throw std::runtime_error("Unknown command type at line " + std::to_string(lineNum));
        }
    }

    verboseOutput("Translation Complete!");
}
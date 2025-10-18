#include "vmtranslator.h"
#include <iostream>

VMTranslator::VMTranslator(std::string inputFile, std::string outputFile, bool verbose)
    : parser(std::move(inputFile)), codeWriter(std::move(outputFile)), verbose(verbose) {
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
        } 
        else if (type == CommandType::C_PUSH) {
            std::string segment = parser.arg1();
            int index = parser.arg2();
            verboseOutput("Parser: Processing line " + std::to_string(lineNum) + ": push " + segment + " " + std::to_string(index));
            verboseOutput("CodeWriter: Translating push " + segment + " " + std::to_string(index));
            codeWriter.writePushPop("push", segment, index);
        } 
        else if (type == CommandType::C_POP) {
            std::string segment = parser.arg1();
            int index = parser.arg2();
            verboseOutput("Parser: Processing line " + std::to_string(lineNum) + ": pop " + segment + " " + std::to_string(index));
            verboseOutput("CodeWriter: Translating pop " + segment + " " + std::to_string(index));
            codeWriter.writePushPop("pop", segment, index);
        }
        else if (type == CommandType::C_LABEL) {
            std::string label = parser.arg1();
            verboseOutput("Parser: Processing line " + std::to_string(lineNum) + ": label " + label);
            verboseOutput("CodeWriter: Translating label " + label);
            codeWriter.writeLabel(label);
        }
        else if (type == CommandType::C_GOTO) {
            std::string label = parser.arg1();
            verboseOutput("Parser: Processing line " + std::to_string(lineNum) + ": goto " + label);
            verboseOutput("CodeWriter: Translating goto " + label);
            codeWriter.writeGoto(label);
        }
        else if (type == CommandType::C_IF) {
            std::string label = parser.arg1();
            verboseOutput("Parser: Processing line " + std::to_string(lineNum) + ": if-goto " + label);
            verboseOutput("CodeWriter: Translating if-goto " + label);
            codeWriter.writeIf(label);
        }
        else if (type == CommandType::C_FUNCTION) {
            std::string functionName = parser.arg1();
            int numLocals = parser.arg2();
            verboseOutput("Parser: Processing line " + std::to_string(lineNum) + ": function " + functionName + " " + std::to_string(numLocals));
            verboseOutput("CodeWriter: Translating function " + functionName + " " + std::to_string(numLocals));
            codeWriter.writeFunction(functionName, numLocals);
        }
        else if (type == CommandType::C_CALL) {
            std::string functionName = parser.arg1();
            int numArgs = parser.arg2();
            verboseOutput("Parser: Processing line " + std::to_string(lineNum) + ": call " + functionName + " " + std::to_string(numArgs));
            verboseOutput("CodeWriter: Translating call " + functionName + " " + std::to_string(numArgs));
            codeWriter.writeCall(functionName, numArgs);
        }
        else if (type == CommandType::C_RETURN) {
            verboseOutput("Parser: Processing line " + std::to_string(lineNum) + ": return");
            verboseOutput("CodeWriter: Translating return");
            codeWriter.writeReturn();
        }
        else {
            throw std::runtime_error("Unknown command type at line " + std::to_string(lineNum));
        }
    }

    verboseOutput("Translation Complete!");
}
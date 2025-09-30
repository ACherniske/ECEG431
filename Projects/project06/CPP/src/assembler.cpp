#include "assembler.h"
#include <iostream>
#include <fstream>
#include <bitset>
#include <algorithm>
#include <cctype>

Assembler::Assembler(const std::string& inputFile, bool verbose)
    : parser(inputFile), verbose(verbose) {}

bool Assembler::isNumber(const std::string& str) {
    return !str.empty() && std::all_of(str.begin(), str.end(), ::isdigit);
}

std::string Assembler::toBinary(int value, int bits) {
    return std::bitset<16>(value).to_string().substr(16 - bits);
}

void Assembler::verboseOutput(const std::string& message) {
    if (verbose) {
        std::cout << message << std::endl;
    }
}

void Assembler::firstPass() {
    verboseOutput("Step 1: First pass = scanning for labels...");

    int pc = 0;
    parser.reset();

    while (parser.hasMoreCommands()) {
        parser.advance();

        if (parser.commandType() == InstructionType::L_INSTRUCTION) {
            std::string symbol = parser.symbol();
            symbolTable.addEntry(symbol, pc);
            verboseOutput("Found label: " + symbol + " at address " + std::to_string(pc));
        } else {
            pc++;
        }
    }
    verboseOutput("First pass complete. Found " + std::to_string(pc) + " instructions");
}

void Assembler::secondPass(const std::string& outputFile) {
    verboseOutput("Step 2: Second pass = translating instructions...");

    std::ofstream output(outputFile);
    if (!output.is_open()) {
        throw std::runtime_error("Could not open output file: " + outputFile);
    }

    parser.reset();
    int pc = 0;

    while (parser.hasMoreCommands()) {
        parser.advance();

        InstructionType type = parser.commandType();

        if (type == InstructionType::L_INSTRUCTION) {
            continue; // Labels are ignored in the second pass
        }

        std::string binaryInstruction;

        if (type == InstructionType::A_INSTRUCTION) {
            std::string symbol = parser.symbol();
            int value;

            if (isNumber(symbol)) {
                value = std::stoi(symbol);
            } else {
                if (symbolTable.contains(symbol)) {
                    value = symbolTable.getAddress(symbol);
                } else {
                    //new variable
                    value = symbolTable.getNextVariableAddress();
                    symbolTable.addEntry(symbol, value);
                    symbolTable.incrementNextVariableAddress();
                    verboseOutput("New variable: " + symbol + " assigned to address " + std::to_string(value));
                }
            }

            binaryInstruction = "0" + toBinary(value, 15);
            verboseOutput("A-instruction: @" + symbol + " -> " + binaryInstruction);
        } else if (type == InstructionType::C_INSTRUCTION) {
            std::string destMnemonic = parser.dest();
            std::string compMnemonic = parser.comp();
            std::string jumpMnemonic = parser.jump();

            verboseOutput("C-instruction: dest=" + destMnemonic + ", comp=" + compMnemonic + ", jump=" + jumpMnemonic);

            std::string destBits = code.dest(destMnemonic);
            std::string compBits = code.comp(compMnemonic);
            std::string jumpBits = code.jump(jumpMnemonic);

            binaryInstruction = "111" + compBits + destBits + jumpBits;
            verboseOutput("C-instruction: " + destMnemonic + "=" + compMnemonic + ";" + jumpMnemonic + " -> " + binaryInstruction);            
        }

        output << binaryInstruction << std::endl;
        pc++;
    }

    output.close();
    verboseOutput("Assembly complete!");
    verboseOutput("Generated " + std::to_string(pc) + " machine code instructions.");
}

void Assembler::assemble(const std::string& outputFile) {
    firstPass();
    secondPass(outputFile);
}

void Assembler::printSymbolTable() const {
    symbolTable.printTable();
}
#include "parser.h"
#include <iostream>
#include <algorithm>
#include <cctype> 

Parser::Parser(const std::string& filename) : currentLine(0) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        throw std::runtime_error("Could not open file: " + filename);
    }

    std::string line;
    while (std::getline(file, line)) {
        line = trim(removeComments(line));
        if (!line.empty()) {
            lines.push_back(line);
        }
    }
    file.close();
}

std::string Parser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return "";

    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, (last - first + 1));
}

std::string Parser::removeComments(const std::string& line) {
    size_t commentPos = line.find("//");
    if (commentPos != std::string::npos) {
        return line.substr(0, commentPos);
    }
    return line;
}

bool Parser::hasMoreCommands() {
    return currentLine < lines.size();
}

void Parser::advance() {
    if (hasMoreCommands()) {
        currentInstruction = lines[currentLine++];
    } else {
        throw std::runtime_error("No more commands to advance to.");
    }
}

InstructionType Parser::commandType() {
    if (currentInstruction.empty()) return InstructionType::A_INSTRUCTION; // Default case

    if (currentInstruction[0] == '@') {
        return InstructionType::A_INSTRUCTION;
    } else if (currentInstruction[0] == '(' && currentInstruction.back() == ')') {
        return InstructionType::L_INSTRUCTION;
    } else {
        return InstructionType::C_INSTRUCTION;
    }
}

std::string Parser::symbol() {
    InstructionType type = commandType();
    if (type == InstructionType::A_INSTRUCTION) {
        return currentInstruction.substr(1); //Remove @
    } else if (type == InstructionType::L_INSTRUCTION) {
        return currentInstruction.substr(1, currentInstruction.length() -2); //Remove ( and )
    }
    return "";
}

std::string Parser::dest() {
    if (commandType() != InstructionType::C_INSTRUCTION) return "";

        size_t equalPos = currentInstruction.find('=');
        if (equalPos != std::string::npos) {
            return currentInstruction.substr(0, equalPos);
        }
    return "";
}

std::string Parser::comp() {
    if (commandType() != InstructionType::C_INSTRUCTION) return "";

    std::string line = currentInstruction;

    //remove dest part if exists
    size_t equalPos = line.find('=');
    if (equalPos != std::string::npos) {
        line = line.substr(equalPos + 1);
    }

    //remove jump part if exists
    size_t semicolonPos = line.find(';');
    if (semicolonPos != std::string::npos) {
        line = line.substr(0, semicolonPos);
    }
    return line;
}

std::string Parser::jump() {
    if (commandType() != InstructionType::C_INSTRUCTION) return "";

    size_t semicolonPos = currentInstruction.find(';');
    if (semicolonPos != std::string::npos) {
        return currentInstruction.substr(semicolonPos + 1);
    }
    return "";
}

void Parser::reset() {
    currentLine = 0;
    currentInstruction = "";
}
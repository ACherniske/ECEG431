#include "vmparser.h"
#include <iostream>
#include <sstream>
#include <algorithm>
#include <cctype>

Parser::Parser(const std::string& filename): currentLine(0) {
    std::ifstream file(filename);
    if (!file.is_open()) throw std::runtime_error("Could not open file: " + filename);

    std::string line;
    while (std::getline(file, line)) {
        line = trim(removeComments(line));
        if (!line.empty()) lines.push_back(line);
    }
}

std::string Parser::trim(const std::string& str) {
    size_t first = str.find_first_not_of(" \t\r\n");
    if (first == std::string::npos) return ""; //all whitespace

    size_t last = str.find_last_not_of(" \t\r\n");
    return str.substr(first, last - first + 1); //trimmed string
}

std::string Parser::removeComments(const std::string& line) {
    size_t commentPos = line.find("//");
    if (commentPos != std::string::npos) { //found comment
        return line.substr(0, commentPos); //return part before comment
    }
    return line;
}

bool Parser::hasMoreCommands() {
    return currentLine < lines.size();
}

void Parser::advance() {
    if (hasMoreCommands()) {
        currentCommand = lines[currentLine++];
    } else {
        throw std::runtime_error("No more commands to advance to.");
    }
}

//add new command types
CommandType Parser::commandType() {
    std::istringstream iss(currentCommand);
    std::string cmd;
    iss >> cmd; //get first word of command

    if (cmd == "add" || cmd == "sub" || cmd == "neg" ||
        cmd == "eq" || cmd == "gt" || cmd == "lt" ||
        cmd == "and" || cmd == "or" || cmd == "not") {
        return CommandType::C_ARITHMETIC;
    } else if (cmd == "push") {
        return CommandType::C_PUSH;
    } else if (cmd == "pop") {
        return CommandType::C_POP;
    } else if (cmd == "label") {
        return CommandType::C_LABEL;
    } else if (cmd == "goto") {
        return CommandType::C_GOTO;
    } else if (cmd == "if-goto") {
        return CommandType::C_IF;
    } else if (cmd == "function") {
        return CommandType::C_FUNCTION;
    } else if (cmd == "call") {
        return CommandType::C_CALL;
    } else if (cmd == "return") {
        return CommandType::C_RETURN;
    }
    return CommandType::C_UNKNOWN;
}

std::string Parser::arg1() {
    CommandType type = commandType();

    std::istringstream iss(currentCommand);
    std::string first, second;
    iss >> first >> second; //ex. push constant 10

    //add new command types
    if (type == CommandType::C_ARITHMETIC) { //for arithmetic, return command itself
        return first;
    } else if (type == CommandType::C_PUSH || type == CommandType::C_POP ||
               type == CommandType::C_LABEL || type == CommandType::C_GOTO ||
               type == CommandType::C_IF || type == CommandType::C_FUNCTION ||
               type == CommandType::C_CALL) { //for these, return first arg
        return second;
    }

    return "";
}

int Parser::arg2() {
    /**
     * Only called if the command is C_PUSH, C_POP, C_FUNCTION, or C_CALL
     * Returns the second argument as an integer
     * Push and Pop: index ex. push constant 10, pop local 0
     * Function and Call: number of args/locals ex. function SimpleFunction 2 (2 is number of locals), call Sys.init 0
     */
    CommandType type = commandType();

    //add new command types
    if (type == CommandType::C_PUSH || type == CommandType::C_POP || //push pop arg2 is index
        type == CommandType::C_FUNCTION || type == CommandType::C_CALL) { //function call arg2 is number of args/locals
        std::istringstream iss(currentCommand);
        std::string first, second, third;
        iss >> first >> second >> third;
        return std::stoi(third);
    }

    return -1;
}

void Parser::reset() {
    currentLine = 0;
    currentCommand.clear();
}
#ifndef PARSER_H
#define PARSER_H

#include <string>
#include <vector>
#include <fstream>

enum class InstructionType {
    A_INSTRUCTION,
    C_INSTRUCTION,
    L_INSTRUCTION // (label)
};

class Parser {
    private:
        std::vector<std::string> lines;
        size_t currentLine;
        std::string currentInstruction;

        std::string trim(const std::string &str);
        std::string removeComments(const std::string& line);

    public:
        Parser(const std::string& str);

        bool hasMoreCommands();
        void advance();
        InstructionType commandType();
        std::string symbol(); // For A_INSTRUCTION and L_INSTRUCTION
        std::string dest();   // For C_INSTRUCTION
        std::string comp();   // For C_INSTRUCTION
        std::string jump();   // For C_INSTRUCTION

        void reset(); //reset to beginning
        const std::vector<std::string>& getLines() const { return lines; }

};

#endif // PARSER_H
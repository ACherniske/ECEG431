#ifndef VMPARSER_H
#define VMPARSER_H

#include <string>
#include <vector>
#include <fstream>

enum class CommandType {
    C_ARITHMETIC,
    C_PUSH,
    C_POP,
    C_LABEL,
    C_GOTO,
    C_IF,
    C_FUNCTION,
    C_RETURN,
    C_CALL,
    C_UNKNOWN
};

class Parser {
    private:
        std::vector<std::string> lines;
        size_t currentLine;
        std::string currentCommand;

        std::string trim(const std::string& str);
        std::string removeComments(const std::string& line);

    public:
        Parser(const std::string& filename);

        bool hasMoreCommands();
        void advance();
        CommandType commandType();
        std::string arg1();
        int arg2();

        void reset();
        const std::vector<std::string>& getLines() const { return lines; }
};

#endif // VMPARSER_H
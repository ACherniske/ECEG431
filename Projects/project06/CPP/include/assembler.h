#ifndef ASSEMBLER_H
#define ASSEMBLER_H

#include <string>
#include "parser.h"
#include "code.h"
#include "symboltable.h"

class Assembler {
    private:
        Parser parser;
        Code code;
        SymbolTable symbolTable;

        bool verbose;

        bool isNumber(const std::string& str);
        std::string toBinary(int value, int bits = 15);
        void firstPass();
        void secondPass(const std::string& outputFile);
        void verboseOutput(const std::string& message);

    public:
        Assembler(const std::string& inputFile, bool verbose = false);

        void assemble(const std::string& outputFile);
        void printSymbolTable() const;
};

#endif // ASSEMBLER_H
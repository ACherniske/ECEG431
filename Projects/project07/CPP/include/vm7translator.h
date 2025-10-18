#ifndef VMTRANSLATOR_H
#define VMTRANSLATOR_H

#include <string>
#include "vm7parser.h"
#include "vm7codewriter.h"

class VMTranslator {
    private:
        Parser parser;
        CodeWriter codeWriter;
        bool verbose;

        void verboseOutput(const std::string& message);

    public:
        VMTranslator(std::string inputFile, std::string outputFile, bool verbose = false);

        void translate();
};

#endif // VMTRANSLATOR_H
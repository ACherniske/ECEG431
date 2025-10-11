#ifndef VMTRANSLATOR_H
#define VMTRANSLATOR_H

#include <string>
#include "vmparser.h"
#include "codewriter.h"

class VMTranslator {
    private:
        Parser parser;
        CodeWriter codeWriter;
        bool verbose;

        void verboseOutput(const std::string& message);

    public:
        VMTranslator(const std::string& inputFile, const std::string& outputFile, bool verbose = false);

        void translate();
};

#endif // VMTRANSLATOR_H
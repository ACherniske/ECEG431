#ifndef CODEWRITER_H
#define CODEWRITER_H

#include <string>
#include <fstream>

class CodeWriter {
    private:
        std::ofstream outputFile;
        std::string currentFileName;
        std::string currentFunction;
        int labelCounter;
        int callCounter;

        std::string generateLabel(const std::string& prefix);
        void writeComparison(const std::string& jumpType);
        void writePushSegment(const std::string& segment, int index);
        void writePopSegment(const std::string& segment, int index);

    public:
        CodeWriter(const std::string& outputFileName);
        ~CodeWriter();

        void setFileName(const std::string& fileName);
        void writeArithmetic(const std::string& command);
        void writePushPop(const std::string& command, const std::string& segment, int index);
        
        //chapter 8 additions
        void writeInit();
        void writeLabel(const std::string& label);
        void writeGoto(const std::string& label);
        void writeIf(const std::string& label);
        void writeCall(const std::string& functionName, int numArgs);
        void writeReturn();
        void writeFunction(const std::string& functionName, int numLocals);
        
        void close();
};

#endif // CODEWRITER_H
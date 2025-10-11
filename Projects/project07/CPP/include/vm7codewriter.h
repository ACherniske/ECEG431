#ifndef CODEWRITER_H
#define CODEWRITER_H

#include <string>
#include <fstream>

class CodeWriter {
    private:
        std::ofstream outputFile;
        std::string currentFileName;
        int labelCounter;

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
        void close();
};

#endif // CODEWRITER_H
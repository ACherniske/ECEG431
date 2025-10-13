#ifndef VM_WRITER_H
#define VM_WRITER_H

#include <string>
#include <fstream>

enum class Segment {
    CONST,
    ARG,
    LOCAL,
    STATIC,
    THIS,
    THAT,
    POINTER,
    TEMP
};

enum class Command {
    ADD,
    SUB,
    NEG,
    EQ,
    GT,
    LT,
    AND,
    OR,
    NOT
};

class VMWriter {
    private:
        std::ofstream output;

        std::string segmentToString(Segment segment); //helper to convert enums to strings
        std::string commandToString(Command command); //helper to convert enums to strings

    public:
        VMWriter(const std::string& outputFile);
        ~VMWriter();

        void writePush(Segment segment, int index);
        void writePop(Segment segment, int index);
        void writeArithmetic(Command command);
        void writeLabel(const std::string& label);
        void writeGoto(const std::string& label);
        void writeIf(const std::string& label);
        void writeCall(const std::string& name, int nArgs);
        void writeFunction(const std::string& name, int nLocals);
        void writeReturn();
};

#endif // VM_WRITER_H
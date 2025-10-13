#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "JackTokenizer.h"
#include "JackSymbolTable.h"
#include "VMWriter.h"
#include <string>

class CompilationEngine {
private:
    JackTokenizer& tokenizer;
    SymbolTable symbolTable;
    VMWriter vmWriter;
    std::string className;
    std::string currentFunction;
    int labelCounter;
    
    void eat(const std::string& expected);
    void eatKeyword(Keyword kw);
    void eatSymbol(char sym);
    
    std::string generateLabel(const std::string& prefix);
    Segment kindToSegment(SegmentKind kind);
    
public:
    CompilationEngine(JackTokenizer& tok, const std::string& outputFile);
    
    void compileClass();
    void compileClassVarDec();
    void compileSubroutine();
    void compileParameterList();
    void compileVarDec();
    void compileStatements();
    void compileDo();
    void compileLet();
    void compileWhile();
    void compileReturn();
    void compileIf();
    void compileExpression();
    void compileTerm();
    int compileExpressionList();
};

#endif // COMPILATION_ENGINE_H
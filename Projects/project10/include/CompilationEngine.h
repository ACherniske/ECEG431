#ifndef COMPILATION_ENGINE_H
#define COMPILATION_ENGINE_H

#include "JackTokenizer.h"
#include <fstream>
#include <string>

class CompilationEngine {
private:
    JackTokenizer& tokenizer; //reference to tokenizer
    std::ofstream output;
    int indentLevel; //for pretty printing XML
    
    void writeIndent();
    void writeOpenTag(const std::string& tag);
    void writeCloseTag(const std::string& tag);
    void writeTerminal();
    void eat(const std::string& expected); //consume expected token
    void eatKeyword(Keyword kw); //consume expected keyword
    void eatSymbol(char sym); //consume expected symbol
    std::string escapeXml(const std::string& str);
    
public:
    CompilationEngine(JackTokenizer& tok, const std::string& outputFile); //constructor
    ~CompilationEngine(); //destructor to close file
    
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
    void compileExpressionList();
};

#endif // COMPILATION_ENGINE_H
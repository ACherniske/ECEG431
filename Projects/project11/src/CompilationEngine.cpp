#include "JackCompilationEngine.h"
#include <stdexcept>

CompilationEngine::CompilationEngine(JackTokenizer& tok, const std::string& outputFile)
    : tokenizer(tok), vmWriter(outputFile), labelCounter(0) {}

void CompilationEngine::eat(const std::string& expected) {
    if (tokenizer.getCurrentToken() != expected) {
        throw std::runtime_error("Expected '" + expected + "' but got '" + tokenizer.getCurrentToken() + "'");
    }
    tokenizer.advance();
}

void CompilationEngine::eatKeyword(Keyword kw) {
    if (tokenizer.tokenType() != TokenType::KEYWORD || tokenizer.keyword() != kw) {
        throw std::runtime_error("Expected keyword");
    }
    tokenizer.advance();
}

void CompilationEngine::eatSymbol(char sym) {
    if (tokenizer.tokenType() != TokenType::SYMBOL || tokenizer.symbol() != sym) {
        throw std::runtime_error(std::string("Expected symbol '") + sym + "'");
    }
    tokenizer.advance();
}

std::string CompilationEngine::generateLabel(const std::string& prefix) {
    return prefix + std::to_string(labelCounter++);
}

Segment CompilationEngine::kindToSegment(SegmentKind kind) {
    switch (kind) {
        case SegmentKind::STATIC: return Segment::STATIC;
        case SegmentKind::FIELD: return Segment::THIS;
        case SegmentKind::ARG: return Segment::ARG;
        case SegmentKind::VAR: return Segment::LOCAL;
    }
    return Segment::LOCAL;
}

void CompilationEngine::compileClass() {
    eatKeyword(Keyword::CLASS);
    className = tokenizer.identifier();
    tokenizer.advance();
    eatSymbol('{');
    
    while (tokenizer.getCurrentToken() == "static" || 
           tokenizer.getCurrentToken() == "field") {
        compileClassVarDec();
    }
    
    while (tokenizer.getCurrentToken() == "constructor" || 
           tokenizer.getCurrentToken() == "function" || 
           tokenizer.getCurrentToken() == "method") {
        compileSubroutine();
    }
    
    eatSymbol('}');
}

void CompilationEngine::compileClassVarDec() {
    std::string keyword = tokenizer.getCurrentToken();
    SegmentKind kind = (keyword == "static") ? SegmentKind::STATIC : SegmentKind::FIELD;
    tokenizer.advance();
    
    std::string type = tokenizer.getCurrentToken();
    tokenizer.advance();
    
    std::string varName = tokenizer.identifier();
    symbolTable.define(varName, type, kind);
    tokenizer.advance();
    
    while (tokenizer.getCurrentToken() == ",") {
        eatSymbol(',');
        varName = tokenizer.identifier();
        symbolTable.define(varName, type, kind);
        tokenizer.advance();
    }
    
    eatSymbol(';');
}

void CompilationEngine::compileSubroutine() {
    symbolTable.startSubroutine();
    
    std::string subroutineType = tokenizer.getCurrentToken();
    tokenizer.advance();
    
    std::string returnType = tokenizer.getCurrentToken();
    tokenizer.advance();
    
    std::string subroutineName = tokenizer.identifier();
    currentFunction = className + "." + subroutineName;
    tokenizer.advance();
    
    if (subroutineType == "method") {
        symbolTable.define("this", className, SegmentKind::ARG);
    }
    
    eatSymbol('(');
    compileParameterList();
    eatSymbol(')');
    
    eatSymbol('{');
    
    while (tokenizer.getCurrentToken() == "var") {
        compileVarDec();
    }
    
    int nLocals = symbolTable.varCount(SegmentKind::VAR);
    vmWriter.writeFunction(currentFunction, nLocals);
    
    if (subroutineType == "constructor") {
        int nFields = symbolTable.varCount(SegmentKind::FIELD);
        vmWriter.writePush(Segment::CONST, nFields);
        vmWriter.writeCall("Memory.alloc", 1);
        vmWriter.writePop(Segment::POINTER, 0);
    }
    else if (subroutineType == "method") {
        vmWriter.writePush(Segment::ARG, 0);
        vmWriter.writePop(Segment::POINTER, 0);
    }
    
    compileStatements();
    eatSymbol('}');
}

void CompilationEngine::compileParameterList() {
    if (tokenizer.getCurrentToken() != ")") {
        std::string type = tokenizer.getCurrentToken();
        tokenizer.advance();
        
        std::string varName = tokenizer.identifier();
        symbolTable.define(varName, type, SegmentKind::ARG);
        tokenizer.advance();
        
        while (tokenizer.getCurrentToken() == ",") {
            eatSymbol(',');
            type = tokenizer.getCurrentToken();
            tokenizer.advance();
            
            varName = tokenizer.identifier();
            symbolTable.define(varName, type, SegmentKind::ARG);
            tokenizer.advance();
        }
    }
}

void CompilationEngine::compileVarDec() {
    eatKeyword(Keyword::VAR);
    
    std::string type = tokenizer.getCurrentToken();
    tokenizer.advance();
    
    std::string varName = tokenizer.identifier();
    symbolTable.define(varName, type, SegmentKind::VAR);
    tokenizer.advance();
    
    while (tokenizer.getCurrentToken() == ",") {
        eatSymbol(',');
        varName = tokenizer.identifier();
        symbolTable.define(varName, type, SegmentKind::VAR);
        tokenizer.advance();
    }
    
    eatSymbol(';');
}

void CompilationEngine::compileStatements() {
    while (true) {
        if (tokenizer.getCurrentToken() == "let") {
            compileLet();
        } else if (tokenizer.getCurrentToken() == "if") {
            compileIf();
        } else if (tokenizer.getCurrentToken() == "while") {
            compileWhile();
        } else if (tokenizer.getCurrentToken() == "do") {
            compileDo();
        } else if (tokenizer.getCurrentToken() == "return") {
            compileReturn();
        } else {
            break;
        }
    }
}

void CompilationEngine::compileDo() {
    eatKeyword(Keyword::DO);
    
    std::string name = tokenizer.identifier();
    tokenizer.advance();
    
    int nArgs = 0;
    std::string functionName;
    
    if (tokenizer.getCurrentToken() == ".") {
        eatSymbol('.');
        std::string subroutineName = tokenizer.identifier();
        tokenizer.advance();
        
        if (symbolTable.exists(name)) {
            std::string type = symbolTable.typeOf(name);
            functionName = type + "." + subroutineName;
            
            Segment seg = kindToSegment(symbolTable.kindOf(name));
            int index = symbolTable.indexOf(name);
            vmWriter.writePush(seg, index);
            nArgs = 1;
        } else {
            functionName = name + "." + subroutineName;
        }
    } else {
        functionName = className + "." + name;
        vmWriter.writePush(Segment::POINTER, 0);
        nArgs = 1;
    }
    
    eatSymbol('(');
    nArgs += compileExpressionList();
    eatSymbol(')');
    
    vmWriter.writeCall(functionName, nArgs);
    vmWriter.writePop(Segment::TEMP, 0);
    
    eatSymbol(';');
}

void CompilationEngine::compileLet() {
    eatKeyword(Keyword::LET);
    
    std::string varName = tokenizer.identifier();
    tokenizer.advance();
    
    bool isArray = false;
    if (tokenizer.getCurrentToken() == "[") {
        isArray = true;
        Segment seg = kindToSegment(symbolTable.kindOf(varName));
        int index = symbolTable.indexOf(varName);
        vmWriter.writePush(seg, index);
        
        eatSymbol('[');
        compileExpression();
        eatSymbol(']');
        
        vmWriter.writeArithmetic(Command::ADD);
    }
    
    eatSymbol('=');
    compileExpression();
    
    if (isArray) {
        vmWriter.writePop(Segment::TEMP, 0);
        vmWriter.writePop(Segment::POINTER, 1);
        vmWriter.writePush(Segment::TEMP, 0);
        vmWriter.writePop(Segment::THAT, 0);
    } else {
        Segment seg = kindToSegment(symbolTable.kindOf(varName));
        int index = symbolTable.indexOf(varName);
        vmWriter.writePop(seg, index);
    }
    
    eatSymbol(';');
}

void CompilationEngine::compileWhile() {
    std::string labelWhile = generateLabel("WHILE_EXP");
    std::string labelEnd = generateLabel("WHILE_END");
    
    vmWriter.writeLabel(labelWhile);
    
    eatKeyword(Keyword::WHILE);
    eatSymbol('(');
    compileExpression();
    eatSymbol(')');
    
    vmWriter.writeArithmetic(Command::NOT);
    vmWriter.writeIf(labelEnd);
    
    eatSymbol('{');
    compileStatements();
    eatSymbol('}');
    
    vmWriter.writeGoto(labelWhile);
    vmWriter.writeLabel(labelEnd);
}

void CompilationEngine::compileReturn() {
    eatKeyword(Keyword::RETURN);
    
    if (tokenizer.getCurrentToken() != ";") {
        compileExpression();
    } else {
        vmWriter.writePush(Segment::CONST, 0);
    }
    
    vmWriter.writeReturn();
    eatSymbol(';');
}

void CompilationEngine::compileIf() {
    std::string labelTrue = generateLabel("IF_TRUE");
    std::string labelFalse = generateLabel("IF_FALSE");
    std::string labelEnd = generateLabel("IF_END");
    
    eatKeyword(Keyword::IF);
    eatSymbol('(');
    compileExpression();
    eatSymbol(')');
    
    vmWriter.writeIf(labelTrue);
    vmWriter.writeGoto(labelFalse);
    
    vmWriter.writeLabel(labelTrue);
    eatSymbol('{');
    compileStatements();
    eatSymbol('}');
    
    if (tokenizer.getCurrentToken() == "else") {
        vmWriter.writeGoto(labelEnd);
        
        vmWriter.writeLabel(labelFalse);
        eatKeyword(Keyword::ELSE);
        eatSymbol('{');
        compileStatements();
        eatSymbol('}');
        
        vmWriter.writeLabel(labelEnd);
    } else {
        vmWriter.writeLabel(labelFalse);
    }
}

void CompilationEngine::compileExpression() {
    compileTerm();
    
    while (tokenizer.tokenType() == TokenType::SYMBOL) {
        char op = tokenizer.symbol();
        
        if (op == '+' || op == '-' || op == '*' || op == '/' || 
            op == '&' || op == '|' || op == '<' || op == '>' || op == '=') {
            
            tokenizer.advance();
            compileTerm();
            
            switch (op) {
                case '+': vmWriter.writeArithmetic(Command::ADD); break;
                case '-': vmWriter.writeArithmetic(Command::SUB); break;
                case '*': vmWriter.writeCall("Math.multiply", 2); break;
                case '/': vmWriter.writeCall("Math.divide", 2); break;
                case '&': vmWriter.writeArithmetic(Command::AND); break;
                case '|': vmWriter.writeArithmetic(Command::OR); break;
                case '<': vmWriter.writeArithmetic(Command::LT); break;
                case '>': vmWriter.writeArithmetic(Command::GT); break;
                case '=': vmWriter.writeArithmetic(Command::EQ); break;
            }
        } else {
            break;
        }
    }
}

void CompilationEngine::compileTerm() {
    if (tokenizer.tokenType() == TokenType::INT_CONST) {
        int value = tokenizer.intVal();
        vmWriter.writePush(Segment::CONST, value);
        tokenizer.advance();
    } 
    else if (tokenizer.tokenType() == TokenType::STRING_CONST) {
        std::string str = tokenizer.stringVal();
        vmWriter.writePush(Segment::CONST, str.length());
        vmWriter.writeCall("String.new", 1);
        
        for (char c : str) {
            vmWriter.writePush(Segment::CONST, c);
            vmWriter.writeCall("String.appendChar", 2);
        }
        tokenizer.advance();
    } 
    else if (tokenizer.tokenType() == TokenType::KEYWORD) {
        Keyword kw = tokenizer.keyword();
        
        if (kw == Keyword::TRUE) {
            vmWriter.writePush(Segment::CONST, 0);
            vmWriter.writeArithmetic(Command::NOT);
        } else if (kw == Keyword::FALSE || kw == Keyword::NULL_) {
            vmWriter.writePush(Segment::CONST, 0);
        } else if (kw == Keyword::THIS) {
            vmWriter.writePush(Segment::POINTER, 0);
        }
        tokenizer.advance();
    } 
    else if (tokenizer.getCurrentToken() == "(") {
        eatSymbol('(');
        compileExpression();
        eatSymbol(')');
    } 
    else if (tokenizer.getCurrentToken() == "-" || tokenizer.getCurrentToken() == "~") {
        char op = tokenizer.symbol();
        tokenizer.advance();
        compileTerm();
        
        if (op == '-') {
            vmWriter.writeArithmetic(Command::NEG);
        } else {
            vmWriter.writeArithmetic(Command::NOT);
        }
    } 
    else {
        std::string name = tokenizer.identifier();
        tokenizer.advance();
        
        if (tokenizer.getCurrentToken() == "[") {
            Segment seg = kindToSegment(symbolTable.kindOf(name));
            int index = symbolTable.indexOf(name);
            vmWriter.writePush(seg, index);
            
            eatSymbol('[');
            compileExpression();
            eatSymbol(']');
            
            vmWriter.writeArithmetic(Command::ADD);
            vmWriter.writePop(Segment::POINTER, 1);
            vmWriter.writePush(Segment::THAT, 0);
        } 
        else if (tokenizer.getCurrentToken() == "(" || tokenizer.getCurrentToken() == ".") {
            int nArgs = 0;
            std::string functionName;
            
            if (tokenizer.getCurrentToken() == ".") {
                eatSymbol('.');
                std::string subroutineName = tokenizer.identifier();
                tokenizer.advance();
                
                if (symbolTable.exists(name)) {
                    std::string type = symbolTable.typeOf(name);
                    functionName = type + "." + subroutineName;
                    
                    Segment seg = kindToSegment(symbolTable.kindOf(name));
                    int index = symbolTable.indexOf(name);
                    vmWriter.writePush(seg, index);
                    nArgs = 1;
                } else {
                    functionName = name + "." + subroutineName;
                }
            } else {
                functionName = className + "." + name;
                vmWriter.writePush(Segment::POINTER, 0);
                nArgs = 1;
            }
            
            eatSymbol('(');
            nArgs += compileExpressionList();
            eatSymbol(')');
            
            vmWriter.writeCall(functionName, nArgs);
        } 
        else {
            Segment seg = kindToSegment(symbolTable.kindOf(name));
            int index = symbolTable.indexOf(name);
            vmWriter.writePush(seg, index);
        }
    }
}

int CompilationEngine::compileExpressionList() {
    int nArgs = 0;
    
    if (tokenizer.getCurrentToken() != ")") {
        compileExpression();
        nArgs++;
        
        while (tokenizer.getCurrentToken() == ",") {
            eatSymbol(',');
            compileExpression();
            nArgs++;
        }
    }
    
    return nArgs;
}
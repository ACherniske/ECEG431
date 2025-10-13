#include "CompilationEngine.h"
#include <stdexcept>
#include <unordered_set>

CompilationEngine::CompilationEngine(JackTokenizer& tok,
const std::string& outputFile)
    : tokenizer(tok), indentLevel(0) {
        output.open(outputFile);
        if (!output.is_open()) {
            throw std::runtime_error("Cannot create output file: " + outputFile);
        }
}

CompilationEngine::~CompilationEngine() {
    if (output.is_open()) {
        output.close();
    }
}

void CompilationEngine::writeIndent() {
    for (int i = 0; i < indentLevel; ++i) {
        output << "  ";
    }
}

void CompilationEngine::writeOpenTag(const std::string& tag) {
    writeIndent();
    output << "<" << tag << ">\n";
    indentLevel++;
}

void CompilationEngine::writeCloseTag(const std::string& tag) {
    indentLevel--;
    writeIndent();
    output << "</" << tag << ">\n";
}

std::string CompilationEngine::escapeXml(const std::string& str) {
    std::string result;
    for (char c : str) {
        switch (c) {
            case '<': result += "&lt;"; break;
            case '>': result += "&gt;"; break;
            case '&': result += "&amp;"; break;
            case '"': result += "&quot;"; break;
            case '\'': result += "&apos;"; break;
            default: result += c; break;
        }
    }
    return result;
}

void CompilationEngine::writeTerminal() {
    writeIndent();
    
    switch (tokenizer.tokenType()) {
        case TokenType::KEYWORD:
            output << "<keyword> " << tokenizer.getCurrentToken() 
                   << " </keyword>\n";
            break;
        case TokenType::SYMBOL:
            output << "<symbol> " << escapeXml(tokenizer.getCurrentToken()) 
                   << " </symbol>\n";
            break;
        case TokenType::IDENTIFIER:
            output << "<identifier> " << tokenizer.identifier() 
                   << " </identifier>\n";
            break;
        case TokenType::INT_CONST:
            output << "<integerConstant> " << tokenizer.intVal() 
                   << " </integerConstant>\n";
            break;
        case TokenType::STRING_CONST:
            output << "<stringConstant> " << tokenizer.stringVal() 
                   << " </stringConstant>\n";
            break;
    }
    
    tokenizer.advance();
}

void CompilationEngine::eat(const std::string& expected) { //consume expected token
    if (tokenizer.getCurrentToken() != expected) {
        throw std::runtime_error("Expected '" + expected + "' but got '" + tokenizer.getCurrentToken() + "'");
    }
    writeTerminal();
}

void CompilationEngine::eatKeyword(Keyword kw) { //consume expected keyword
    if (tokenizer.tokenType() != TokenType::KEYWORD || tokenizer.keyword() != kw) {
        throw std::runtime_error("Expected keyword");
    }
    writeTerminal();
}

void CompilationEngine::eatSymbol(char sym) { //consume expected symbol
    if (tokenizer.tokenType() != TokenType::SYMBOL || tokenizer.symbol() != sym) {
        throw std::runtime_error(std::string("Expected symbol '") + sym + "'");
    }
    writeTerminal();
}

void CompilationEngine::compileClass() {
    writeOpenTag("class");
    
    eatKeyword(Keyword::CLASS);
    writeTerminal(); //className
    eatSymbol('{');
    
    // classVarDec*
    while (tokenizer.getCurrentToken() == "static" || 
           tokenizer.getCurrentToken() == "field") {
        compileClassVarDec();
    }
    
    // subroutineDec*
    while (tokenizer.getCurrentToken() == "constructor" || 
           tokenizer.getCurrentToken() == "function" || 
           tokenizer.getCurrentToken() == "method") {
        compileSubroutine();
    }
    
    eatSymbol('}');
    
    writeCloseTag("class");
}

void CompilationEngine::compileClassVarDec() { // 'static' | 'field' type varName (',' varName)* ';'
    writeOpenTag("classVarDec");

    writeTerminal(); // 'static' | 'field'
    writeTerminal(); // type
    writeTerminal(); // varName

    while (tokenizer.getCurrentToken() == ",") {
        eatSymbol(',');
        writeTerminal(); // varName
    }

    eatSymbol(';');

    writeCloseTag("classVarDec");
}

void CompilationEngine::compileSubroutine() { // ('constructor' | 'function' | 'method') ('void' | type) subroutineName '(' parameterList ')' subroutineBody
    writeOpenTag("subroutineDec");

    writeTerminal(); // 'constructor' | 'function' | 'method'
    writeTerminal(); // 'void' | type
    writeTerminal(); // subroutineName
    eatSymbol('(');
    compileParameterList();
    eatSymbol(')');

    //subroutineBody
    writeOpenTag("subroutineBody");
    eatSymbol('{');

    //varDec*
    while (tokenizer.getCurrentToken() == "var") {
        compileVarDec();
    }

    compileStatements();

    eatSymbol('}');
    writeCloseTag("subroutineBody");

    writeCloseTag("subroutineDec");
}

void CompilationEngine::compileParameterList() { // (type varName (',' type varName)*)?
    writeOpenTag("parameterList");

    if (tokenizer.getCurrentToken() != ")") { //not empty
        writeTerminal(); // type
        writeTerminal(); // varName

        while (tokenizer.getCurrentToken() == ",") {
            eatSymbol(',');
            writeTerminal(); // type
            writeTerminal(); // varName
        }
    }

    writeCloseTag("parameterList");
}

void CompilationEngine::compileVarDec() { // 'var' type varName (',' varName)* ';'
    writeOpenTag("varDec");

    eatKeyword(Keyword::VAR);
    writeTerminal(); // type
    writeTerminal(); // varName

    while (tokenizer.getCurrentToken() == ",") {
        eatSymbol(',');
        writeTerminal(); // varName
    }

    eatSymbol(';');

    writeCloseTag("varDec");
}

void CompilationEngine::compileStatements() { // statement*
    writeOpenTag("statements");

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
            break; //no more statements
        }
    }

    writeCloseTag("statements");
}

void CompilationEngine::compileDo() { // 'do' subroutineCall ';'
    writeOpenTag("doStatement");

    eatKeyword(Keyword::DO);
    writeTerminal(); // subroutineCall (subroutineName | (className | varName) '.' subroutineName '(' expressionList ')')

    if (tokenizer.getCurrentToken() == ".") {
        eatSymbol('.');
        writeTerminal(); // subroutineName
    }

    eatSymbol('(');
    compileExpressionList();
    eatSymbol(')');

    eatSymbol(';');

    writeCloseTag("doStatement");
}

void CompilationEngine::compileLet() { // 'let' varName ('[' expression ']')? '=' expression ';'
    writeOpenTag("letStatement");

    eatKeyword(Keyword::LET);
    writeTerminal(); // varName

    if (tokenizer.getCurrentToken() == "[") {
        eatSymbol('[');
        compileExpression();
        eatSymbol(']');
    }

    eatSymbol('=');
    compileExpression();
    eatSymbol(';');

    writeCloseTag("letStatement");
}

void CompilationEngine::compileWhile() { // 'while' '(' expression ')' '{' statements '}'
    writeOpenTag("whileStatement");

    eatKeyword(Keyword::WHILE);
    eatSymbol('(');
    compileExpression();
    eatSymbol(')');
    eatSymbol('{');
    compileStatements();
    eatSymbol('}');

    writeCloseTag("whileStatement");
}

void CompilationEngine::compileReturn() { // 'return' expression? ';'
    writeOpenTag("returnStatement");

    eatKeyword(Keyword::RETURN);

    if (tokenizer.getCurrentToken() != ";") { //not void return
        compileExpression();
    }

    eatSymbol(';');

    writeCloseTag("returnStatement");
}

void CompilationEngine::compileIf() { // 'if' '(' expression ')' '{' statements '}' ('else' '{' statements '}')?
    writeOpenTag("ifStatement");

    eatKeyword(Keyword::IF);
    eatSymbol('(');
    compileExpression();
    eatSymbol(')');
    eatSymbol('{');
    compileStatements();
    eatSymbol('}');

    if (tokenizer.getCurrentToken() == "else") {
        eatKeyword(Keyword::ELSE);
        eatSymbol('{');
        compileStatements();
        eatSymbol('}');
    }

    writeCloseTag("ifStatement");
}

void CompilationEngine::compileExpression() { // term (op term)*
    writeOpenTag("expression");

    compileTerm();

    static const std::unordered_set<char> ops = {'+', '-', '*', '/', '&', '|', '<', '>', '='};

    while (tokenizer.tokenType() == TokenType::SYMBOL && 
           ops.count(tokenizer.symbol())) {
        writeTerminal(); //op
        compileTerm();
    }

    writeCloseTag("expression");
}

void CompilationEngine::compileTerm() { //see grammar for term
    writeOpenTag("term");
    
    if (tokenizer.tokenType() == TokenType::INT_CONST) { //integerConstant
        writeTerminal();
    } else if (tokenizer.tokenType() == TokenType::STRING_CONST) { //stringConstant
        writeTerminal();
    } else if (tokenizer.tokenType() == TokenType::KEYWORD) { //keywordConstant
        // true | false | null | this
        writeTerminal();
    } else if (tokenizer.getCurrentToken() == "(") { // '(' expression ')'
        eatSymbol('(');
        compileExpression();
        eatSymbol(')');
    } else if (tokenizer.getCurrentToken() == "-" || 
               tokenizer.getCurrentToken() == "~") { //unaryOp term
        writeTerminal();
        compileTerm();
    } else {
        //identifier
        writeTerminal();
        
        if (tokenizer.getCurrentToken() == "[") {
            eatSymbol('[');
            compileExpression();
            eatSymbol(']');
        } else if (tokenizer.getCurrentToken() == "(") {
            eatSymbol('(');
            compileExpressionList();
            eatSymbol(')');
        } else if (tokenizer.getCurrentToken() == ".") {
            eatSymbol('.');
            writeTerminal(); //subroutineName
            eatSymbol('(');
            compileExpressionList();
            eatSymbol(')');
        }
    }
    
    writeCloseTag("term");
}

void CompilationEngine::compileExpressionList() { // (expression (',' expression)*)?
    writeOpenTag("expressionList");

    if (tokenizer.getCurrentToken() != ")") { //not empty
        compileExpression();

        while (tokenizer.getCurrentToken() == ",") {
            eatSymbol(',');
            compileExpression();
        }
    }

    writeCloseTag("expressionList");
}
#include "JackTokenizer.h"
#include <cctype>
#include <stdexcept>

const std::unordered_set<std::string> JackTokenizer::keywords = {
    "class", "method", "function", "constructor", "int", "boolean", "char", "void",
    "var", "static", "field", "let", "do", "if", "else", "while", "return",
    "true", "false", "null", "this"
};

const std::unordered_set<char> JackTokenizer::symbols = {
    '{', '}', '(', ')', '[', ']', '.', ',', ';', '+', '-', '*', '/', '&', '|', '<', '>', '=', '~'
};

JackTokenizer::JackTokenizer(const std::string& filename)
    : currentPos(0), hasMoreTokens_(true) {  //start with true
    input.open(filename);
    if (!input.is_open()) {
        throw std::runtime_error("Failed to open file: " + filename);
    }
    if (!std::getline(input, currentLine)) {
        hasMoreTokens_ = false;  //empty file
    }
}

JackTokenizer::~JackTokenizer() { //destructor
    if (input.is_open()) {
        input.close();
    }
}

void JackTokenizer::skipWhitespaceAndComments() {
    while (true) {
        //skip whitespace in current line
        while (currentPos < currentLine.length()
        && std::isspace(currentLine[currentPos])) { //isspace checks for space, tab, newline, etc.
            currentPos++;
        }

        //check for comments
        if (currentPos < currentLine.length() - 1) {
            //single-line comment
            if (currentLine[currentPos] == '/' && currentLine[currentPos + 1] == '/') {
                currentPos = currentLine.length(); //move to end of line
            }
            //multi-line comment start
            else if (currentLine[currentPos] == '/' && currentLine[currentPos + 1] == '*') {
                currentPos += 2; //skip /*
                while (true) {
                    if (currentPos >= currentLine.length()) {
                        if (!std::getline(input, currentLine)) {
                            return;
                        }
                        currentPos = 0;
                    }
                    if (currentPos < currentLine.length() - 1 && currentLine[currentPos] == '*' && currentLine[currentPos + 1] == '/') {
                        currentPos += 2; //skip */
                        break;
                    }
                    currentPos++;
                }
                continue; //recheck for whitespace/comments
            }
        }

        //need new line
        if (currentPos >= currentLine.length()) {
            if (!std::getline(input, currentLine)) {
                return;
            }
            currentPos = 0;
            continue;
        }
        break;
    }
}

bool JackTokenizer::hasMoreTokens() {
    return hasMoreTokens_;
}

void JackTokenizer::readNextToken() {
    skipWhitespaceAndComments();

    if (currentPos >= currentLine.length()) {
        hasMoreTokens_ = false;
        return;
    }

    char c = currentLine[currentPos];

    //string constant
    if (c == '"') {
        currentType = TokenType::STRING_CONST;
        currentToken = "";
        currentPos++;

        while (currentPos < currentLine.length() 
        && currentLine[currentPos] != '"') {
            currentToken += currentLine[currentPos];
            currentPos++;
        }
        currentPos++; //skip closing "
    }
    //symbol
    else if (isSymbol(c)) {
        currentType = TokenType::SYMBOL;
        currentToken = std::string(1, c);
        currentPos++;
    }
    //integer constant
    else if (isDigit(c)) {
        currentType = TokenType::INT_CONST;
        currentToken = "";
        while (currentPos < currentLine.length() 
        && isDigit(currentLine[currentPos])) {
            currentToken += currentLine[currentPos];
            currentPos++;
        }
    }
    //identifier or keyword
    else if (isAlpha(c) || c == '_') {
        currentToken = "";
        while (currentPos < currentLine.length() && 
                    (isAlpha(currentLine[currentPos]) || 
                        isDigit(currentLine[currentPos]) || 
                        currentLine[currentPos] == '_')) {
                    currentToken += currentLine[currentPos];
                    currentPos++;
                }

            if (isKeyword(currentToken)) {
                currentType = TokenType::KEYWORD;
            } else {
                currentType = TokenType::IDENTIFIER;
            }
    }

    hasMoreTokens_ = true;
}

void JackTokenizer::advance() {
    readNextToken();
}

TokenType JackTokenizer::tokenType() {
    return currentType;
}

Keyword JackTokenizer::keyword() {
    if (currentToken == "class") return Keyword::CLASS;
    if (currentToken == "method") return Keyword::METHOD;
    if (currentToken == "function") return Keyword::FUNCTION;
    if (currentToken == "constructor") return Keyword::CONSTRUCTOR;
    if (currentToken == "int") return Keyword::INT;
    if (currentToken == "boolean") return Keyword::BOOLEAN;
    if (currentToken == "char") return Keyword::CHAR;
    if (currentToken == "void") return Keyword::VOID;
    if (currentToken == "var") return Keyword::VAR;
    if (currentToken == "static") return Keyword::STATIC;
    if (currentToken == "field") return Keyword::FIELD;
    if (currentToken == "let") return Keyword::LET;
    if (currentToken == "do") return Keyword::DO;
    if (currentToken == "if") return Keyword::IF;
    if (currentToken == "else") return Keyword::ELSE;
    if (currentToken == "while") return Keyword::WHILE;
    if (currentToken == "return") return Keyword::RETURN;
    if (currentToken == "true") return Keyword::TRUE;
    if (currentToken == "false") return Keyword::FALSE;
    if (currentToken == "null") return Keyword::NULL_;
    if (currentToken == "this") return Keyword::THIS;
    throw std::runtime_error("Not a keyword: " + currentToken);
}

char JackTokenizer::symbol() {
    return currentToken[0];
}

std::string JackTokenizer::identifier() {
    return currentToken;
}


int JackTokenizer::intVal() {
    return std::stoi(currentToken);
}

std::string JackTokenizer::stringVal() {
    return currentToken;
}

std::string JackTokenizer::getCurrentToken() {
    return currentToken;
}

bool JackTokenizer::isKeyword(const std::string& word) {
    return keywords.find(word) != keywords.end();
}

bool JackTokenizer::isSymbol(char c) {
    return symbols.find(c) != symbols.end();
}

bool JackTokenizer::isDigit(char c) {
    return c >= '0' && c <= '9';
}

bool JackTokenizer::isAlpha(char c) {
    return (c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z');
}

void JackTokenizer::writeTokensToXml(const std::string& outputFile) { //write all tokens to XML file
    std::ofstream out(outputFile);
    if (!out.is_open()) {
        throw std::runtime_error("Cannot create output file: " + outputFile);
    }
    
    out << "<tokens>\n";
    
    while (true) {
        skipWhitespaceAndComments();
        
        //check if we're at the end
        if (currentPos >= currentLine.length() && input.eof()) {
            break;
        }
        
        readNextToken();
        
        switch (currentType) {
            case TokenType::KEYWORD:
                out << "<keyword> " << currentToken << " </keyword>\n";
                break;
            case TokenType::SYMBOL: {
                std::string escaped = currentToken;
                if (escaped == "<") escaped = "&lt;";
                else if (escaped == ">") escaped = "&gt;";
                else if (escaped == "&") escaped = "&amp;";
                else if (escaped == "\"") escaped = "&quot;";
                out << "<symbol> " << escaped << " </symbol>\n";
                break;
            }
            case TokenType::IDENTIFIER:
                out << "<identifier> " << currentToken << " </identifier>\n";
                break;
            case TokenType::INT_CONST:
                out << "<integerConstant> " << intVal() << " </integerConstant>\n";
                break;
            case TokenType::STRING_CONST:
                out << "<stringConstant> " << stringVal() << " </stringConstant>\n";
                break;
        }
    }
    
    out << "</tokens>\n";
    out.close();
}
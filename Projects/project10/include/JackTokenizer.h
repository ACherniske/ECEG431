#ifndef JACK_TOKENIZER_H
#define JACK_TOKENIZER_H

#include <string>
#include <fstream>
#include <unordered_set>

enum class TokenType { //possible types of tokens pg 203
    KEYWORD,
    SYMBOL,
    IDENTIFIER,
    INT_CONST,
    STRING_CONST
};

enum class Keyword {
    CLASS, METHOD, FUNCTION, CONSTRUCTOR, INT, BOOLEAN, CHAR, VOID,
    VAR, STATIC, FIELD, LET, DO, IF, ELSE, WHILE, RETURN, TRUE, FALSE, NULL_, THIS //NULL is a reserved word in C++
};

class JackTokenizer {
    private:
        std::ifstream input;
        std::string currentToken;
        TokenType currentType;
        std::string currentLine;
        size_t currentPos;
        bool hasMoreTokens_; //flag to indicate if there are more tokens

        static const std::unordered_set<std::string> keywords;
        static const std::unordered_set<char> symbols;

        void skipWhitespaceAndComments();
        void readNextToken();
        bool isKeyword(const std::string& word);
        bool isSymbol(char c);
        bool isDigit(char c);
        bool isAlpha(char c);

    public:
        JackTokenizer(const std::string& filename);
        ~JackTokenizer(); //destructor to close file

        bool hasMoreTokens();
        void advance();
        TokenType tokenType();
        Keyword keyword();
        char symbol();
        std::string identifier();
        int intVal();
        std::string stringVal();
        std::string getCurrentToken();
        void writeTokensToXml(const std::string& outputFile);
};

#endif // JACK_TOKENIZER_H
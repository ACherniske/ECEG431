#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

#include <string>
#include <unordered_map>

enum class SegmentKind {
    STATIC,    // class-level static variables
    FIELD,     // class-level field variables (this)
    ARG,       // subroutine arguments
    VAR        // subroutine local variables
};

struct Symbol {
    std::string type;
    SegmentKind kind;
    int index;
};

class SymbolTable {
private:
    std::unordered_map<std::string, Symbol> classScope;
    std::unordered_map<std::string, Symbol> subroutineScope;
    int staticCount;
    int fieldCount;
    int argCount;
    int varCount_; //changed to varCount_ to avoid naming conflict

public:
    SymbolTable();
    
    void startSubroutine(); //resets the subroutine scope
    void define(const std::string& name, const std::string& type, SegmentKind kind); //adds a new variable
    int varCount(SegmentKind kind); //returns the number of variables of the given kind
    SegmentKind kindOf(const std::string& name); //returns the kind of the named variable
    std::string typeOf(const std::string& name); //returns the type of the named variable
    int indexOf(const std::string& name); //returns the index of the named variable
    bool exists(const std::string& name); //checks if the named variable exists
};

#endif // SYMBOL_TABLE_H
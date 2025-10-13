#include "JackSymbolTable.h"
#include <stdexcept>

SymbolTable::SymbolTable() //initialize counts to zero
    : staticCount(0), fieldCount(0), argCount(0), varCount_(0) {}

void SymbolTable::startSubroutine() { //reset subroutine scope and counts
    subroutineScope.clear();
    argCount = 0;
    varCount_ = 0;
}

void SymbolTable::define(const std::string& name, const std::string& type, SegmentKind kind) { //add new variable
    Symbol symbol;
    symbol.type = type;
    symbol.kind = kind;
    
    switch (kind) {
        case SegmentKind::STATIC:
            symbol.index = staticCount++;
            classScope[name] = symbol;
            break;
        case SegmentKind::FIELD:
            symbol.index = fieldCount++;
            classScope[name] = symbol;
            break;
        case SegmentKind::ARG:
            symbol.index = argCount++;
            subroutineScope[name] = symbol;
            break;
        case SegmentKind::VAR:
            symbol.index = varCount_++;
            subroutineScope[name] = symbol;
            break;
    }
}

int SymbolTable::varCount(SegmentKind kind) { //return count of variables of given kind
    switch (kind) {
        case SegmentKind::STATIC: return staticCount;
        case SegmentKind::FIELD: return fieldCount;
        case SegmentKind::ARG: return argCount;
        case SegmentKind::VAR: return varCount_;
    }
    return 0;
}

SegmentKind SymbolTable::kindOf(const std::string& name) { //return kind of named variable
    if (subroutineScope.find(name) != subroutineScope.end()) {
        return subroutineScope[name].kind;
    }
    if (classScope.find(name) != classScope.end()) {
        return classScope[name].kind;
    }
    throw std::runtime_error("Variable not found: " + name);
}

std::string SymbolTable::typeOf(const std::string& name) { //return type of named variable
    if (subroutineScope.find(name) != subroutineScope.end()) {
        return subroutineScope[name].type;
    }
    if (classScope.find(name) != classScope.end()) {
        return classScope[name].type;
    }
    throw std::runtime_error("Variable not found: " + name);
}

int SymbolTable::indexOf(const std::string& name) { //return index of named variable
    if (subroutineScope.find(name) != subroutineScope.end()) {
        return subroutineScope[name].index;
    }
    if (classScope.find(name) != classScope.end()) {
        return classScope[name].index;
    }
    throw std::runtime_error("Variable not found: " + name);
}

bool SymbolTable::exists(const std::string& name) { //check if named variable exists
    return subroutineScope.find(name) != subroutineScope.end() ||
           classScope.find(name) != classScope.end();
}
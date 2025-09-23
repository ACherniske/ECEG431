#include "symboltable.h"
#include <iostream>

SymbolTable::SymbolTable() : nextVariableAddress(16) {
    //init prefedined symbols
    table["SP"] = 0;
    table["LCL"] = 1;
    table["ARG"] = 2;
    table["THIS"] = 3;
    table["THAT"] = 4;
    table["SCREEN"] = 16384;
    table["KBD"] = 24576;

    // R0 to R15
    for (int i = 0; i <= 15; ++i) {
        table["R" + std::to_string(i)] = i;
    }
}

void SymbolTable::addEntry(const std::string& symbol, int address) {
    table[symbol] = address;
}

bool SymbolTable::contains(const std::string& symbol) {
    return table.find(symbol) != table.end();
}

int SymbolTable::getAddress(const std::string& symbol) {
    auto it = table.find(symbol);
    if (it != table.end()) {
        return it->second;
    }
    throw std::runtime_error("Symbol not found: " + symbol);
}

void SymbolTable::printTable() const {
    std::cout << "Symbol Table:" << std::endl;
    for (const auto& pair : table) {
        std::cout << pair.first << " -> " << pair.second << std::endl;
    }
}
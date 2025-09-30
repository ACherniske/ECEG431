#ifndef SYMBOLTABLE_H
#define SYMBOLTABLE_H

#include <string>
#include <unordered_map>

class SymbolTable {
    private:
        std::unordered_map<std::string, int> table;
        int nextVariableAddress;

    public:
        SymbolTable();
        
        void addEntry(const std::string& symbol, int address);
        bool contains(const std::string& symbol);
        int getAddress(const std::string& symbol);
        int getNextVariableAddress() { return nextVariableAddress; }
        void incrementNextVariableAddress() { nextVariableAddress++; }

        void printTable() const;
};

#endif // SYMBOLTABLE_H
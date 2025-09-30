#ifndef CODE_H
#define CODE_H

#include <string>
#include <unordered_map>

class Code {
    private:
        std::unordered_map<std::string, std::string> destTable;
        std::unordered_map<std::string, std::string> compTable;
        std::unordered_map<std::string, std::string> jumpTable;
        
        void initializeTables();
        
    public:
        Code();
        
        std::string dest(const std::string& mnemonic);
        std::string comp(const std::string& mnemonic);
        std::string jump(const std::string& mnemonic);
    };

#endif // CODE_H
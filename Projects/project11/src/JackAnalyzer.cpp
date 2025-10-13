#include "JackTokenizer.h"
#include "JackCompilationEngine.h"
#include <iostream>
#include <filesystem>
#include <vector>

namespace fs = std::filesystem;

std::vector<std::string> getJackFiles(const std::string& path) {
    std::vector<std::string> files;
    
    if (fs::is_directory(path)) {
        for (const auto& entry : fs::directory_iterator(path)) {
            if (entry.path().extension() == ".jack") {
                files.push_back(entry.path().string());
            }
        }
    } else if (fs::exists(path) && fs::path(path).extension() == ".jack") {
        files.push_back(path);
    }
    
    return files;
}

std::string getOutputFilename(const std::string& inputFile) {
    fs::path p(inputFile);
    p.replace_extension(".vm");
    return p.string();
}

int main(int argc, char* argv[]) {
    if (argc != 2) {
        std::cerr << "Usage: JackCompiler <input.jack | directory>\n";
        return 1;
    }
    
    std::string input = argv[1];
    std::vector<std::string> jackFiles = getJackFiles(input);
    
    if (jackFiles.empty()) {
        std::cerr << "No .jack files found\n";
        return 1;
    }
    
    for (const auto& file : jackFiles) {
        try {
            std::cout << "Compiling " << file << "...\n";
            
            JackTokenizer tokenizer(file);
            std::string outputFile = getOutputFilename(file);
            CompilationEngine engine(tokenizer, outputFile);
            
            tokenizer.advance(); //get first token
            engine.compileClass();
            
            std::cout << "Created " << outputFile << "\n";
        } catch (const std::exception& e) {
            std::cerr << "Error processing " << file << ": " 
                      << e.what() << "\n";
            return 1;
        }
    }
    
    std::cout << "Compilation completed successfully!\n";
    return 0;
}
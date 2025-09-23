#include <iostream>
#include <string>
#include <cstring>
#include <filesystem>
#include "assembler.h"

void showHelp(const char* programName) {
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS] [FILE]" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << " -f, --file FILE | Specify input .asm file" << std::endl;
    std::cout << " -v, --verbose   | Enable Verbose Output" << std::endl;
    std::cout << " -h, --help      | Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << " Files can also be provided as positional arguments" << std::endl;
}

int main(int argc, char* argv[]) {
    bool verbose = false;
    bool showHelpFlag = false;
    std::string inputFile;
    
    // Parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-f" || arg == "--file") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                inputFile = argv[++i];
            } else {
                std::cerr << "ERROR: -f/--file requires a file argument" << std::endl;
                return 1;
            }
        } else if (arg.substr(0, 7) == "--file=") {
            inputFile = arg.substr(7);
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-h" || arg == "--help") {
            showHelpFlag = true;
        } else if (arg[0] == '-') {
            std::cerr << "ERROR: Unknown option " << arg << std::endl;
            showHelp(argv[0]);
            return 1;
        } else {
            if (inputFile.empty()) {
                inputFile = arg;
            } else {
                std::cerr << "ERROR: Multiple files specified. Use only one file." << std::endl;
                return 1;
            }
        }
    }
    
    // Show help if requested
    if (showHelpFlag) {
        showHelp(argv[0]);
        return 0;
    }
    
    // Check if required file argument is provided
    if (inputFile.empty()) {
        std::cerr << "ERROR: Input file is required. Specify with -f/--file or as a positional argument" << std::endl;
        return 1;
    }
    
    // Check if file exists
    if (!std::filesystem::exists(inputFile)) {
        std::cerr << "ERROR: File '" << inputFile << "' does not exist" << std::endl;
        return 1;
    }
    
    // Check if file is a .asm
    if (inputFile.substr(inputFile.find_last_of('.') + 1) != "asm") {
        std::cerr << "ERROR: File must have .asm extension" << std::endl;
        return 1;
    }
    
    // Create output file name (.hack)
    std::string outputFile = inputFile;
    size_t lastDot = outputFile.find_last_of('.');
    if (lastDot != std::string::npos) {
        outputFile = outputFile.substr(0, lastDot) + ".hack";
    } else {
        outputFile += ".hack";
    }
    
    if (verbose) {
        std::cerr << "Assembling " << inputFile << " --> " << outputFile << std::endl;
    }
    
    try {
        Assembler assembler(inputFile, verbose);
        assembler.assemble(outputFile);
        
        std::cout << "Assembly successful! Generated " << outputFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
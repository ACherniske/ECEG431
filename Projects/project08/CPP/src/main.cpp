#include <iostream>
#include <string>
#include <vector>
#include <filesystem>
#include "vmtranslator.h"
#include "codewriter.h"
#include "vmparser.h"

void showHelp(const char* programName) {
    std::cout << std::endl;
    std::cout << "Usage: " << programName << " [OPTIONS] [FILE|DIRECTORY]" << std::endl;
    std::cout << std::endl;
    std::cout << "OPTIONS:" << std::endl;
    std::cout << " -f, --file FILE/DIR | Specify input .vm file or directory" << std::endl;
    std::cout << " -v, --verbose       | Enable Verbose Output" << std::endl;
    std::cout << " -h, --help          | Show this help message" << std::endl;
    std::cout << std::endl;
    std::cout << " Files/directories can also be provided as positional arguments" << std::endl;
}

int main(int argc, char* argv[]) {
    bool verbose = false;
    bool showHelpFlag = false;
    std::string inputPath;
    
    //parse command line arguments
    for (int i = 1; i < argc; i++) {
        std::string arg = argv[i];
        
        if (arg == "-f" || arg == "--file") {
            if (i + 1 < argc && argv[i + 1][0] != '-') {
                inputPath = argv[++i];
            } else {
                std::cerr << "ERROR: -f/--file requires a file argument" << std::endl;
                return 1;
            }
        } else if (arg.substr(0, 7) == "--file=") {
            inputPath = arg.substr(7);
        } else if (arg == "-v" || arg == "--verbose") {
            verbose = true;
        } else if (arg == "-h" || arg == "--help") {
            showHelpFlag = true;
        } else if (arg == "-n" || arg == "-y") {
            //ignore -n and -y options for compatibility with autograder
            continue;
        } else if (arg[0] == '-') {
            std::cerr << "ERROR: Unknown option " << arg << std::endl;
            showHelp(argv[0]);
            return 1;
        } else {
            if (inputPath.empty()) {
                inputPath = arg;
            } else {
                std::cerr << "ERROR: Multiple paths specified. Use only one file or directory." << std::endl;
                return 1;
            }
        }
    }
    
    //show help if requested
    if (showHelpFlag) {
        showHelp(argv[0]);
        return 0;
    }
    
    //check if required path argument is provided
    if (inputPath.empty()) {
        std::cerr << "ERROR: Input file or directory is required. Specify with -f/--file or as a positional argument" << std::endl;
        return 1;
    }

    //check if path exists
    if (!std::filesystem::exists(inputPath)) {
        std::cerr << "ERROR: Path '" << inputPath << "' does not exist" << std::endl;
        return 1;
    }

    std::vector<std::string> vmFiles;
    std::string outputFile;
    
    //handle directory or single file
    if (std::filesystem::is_directory(inputPath)) {
        //directory: collect all .vm files
        for (const auto& entry : std::filesystem::directory_iterator(inputPath)) {
            if (entry.path().extension() == ".vm") {
                vmFiles.push_back(entry.path().string());
            }
        }
        
        if (vmFiles.empty()) {
            std::cerr << "ERROR: No .vm files found in directory '" << inputPath << "'" << std::endl;
            return 1;
        }
        
        //output file is directory name + .asm
        std::string dirName = std::filesystem::path(inputPath).filename().string();
        outputFile = inputPath;
        if (outputFile.back() != '/' && outputFile.back() != '\\') {
            outputFile += "/";
        }
        outputFile += dirName + ".asm";
        
        if (verbose) {
            std::cerr << "Directory mode: Found " << vmFiles.size() << " .vm files" << std::endl;
        }
    } else {
        //single file mode
        if (std::filesystem::path(inputPath).extension() != ".vm") {
            std::cerr << "ERROR: Input file must have .vm extension" << std::endl;
            return 1;
        }
        
        vmFiles.push_back(inputPath);
        
        //create output file name (.asm)
        outputFile = inputPath;
        size_t lastDot = outputFile.find_last_of('.');
        if (lastDot != std::string::npos) {
            outputFile = outputFile.substr(0, lastDot) + ".asm";
        } else {
            outputFile += ".asm";
        }
    }
    
    if (verbose) {
        std::cerr << "Translating to " << outputFile << std::endl;
    }
    
    try {
        //create CodeWriter
        CodeWriter codeWriter(outputFile);
        
        //write bootstrap code (for directory mode or if Sys.vm exists)
        bool needsBootstrap = vmFiles.size() > 1;
        if (!needsBootstrap) {
            //check if single file is Sys.vm or contains Sys.init
            std::string fileName = std::filesystem::path(vmFiles[0]).filename().string();
            if (fileName == "Sys.vm") {
                needsBootstrap = true;
            }
        }
        
        if (needsBootstrap) {
            if (verbose) {
                std::cerr << "Writing bootstrap code..." << std::endl;
            }
            codeWriter.writeInit();
        }
        
        //translate each .vm file
        for (const auto& vmFile : vmFiles) {
            if (verbose) {
                std::cerr << "Translating " << vmFile << "..." << std::endl;
            }
            
            Parser parser(vmFile);
            codeWriter.setFileName(vmFile);
            
            int lineNum = 0;
            while (parser.hasMoreCommands()) {
                parser.advance();
                lineNum++;

                CommandType type = parser.commandType();

                if (type == CommandType::C_ARITHMETIC) {
                    std::string command = parser.arg1();
                    if (verbose) {
                        std::cout << "  Line " << lineNum << ": " << command << std::endl;
                    }
                    codeWriter.writeArithmetic(command);
                } 
                else if (type == CommandType::C_PUSH) {
                    std::string segment = parser.arg1();
                    int index = parser.arg2();
                    if (verbose) {
                        std::cout << "  Line " << lineNum << ": push " << segment << " " << index << std::endl;
                    }
                    codeWriter.writePushPop("push", segment, index);
                } 
                else if (type == CommandType::C_POP) {
                    std::string segment = parser.arg1();
                    int index = parser.arg2();
                    if (verbose) {
                        std::cout << "  Line " << lineNum << ": pop " << segment << " " << index << std::endl;
                    }
                    codeWriter.writePushPop("pop", segment, index);
                }
                else if (type == CommandType::C_LABEL) {
                    std::string label = parser.arg1();
                    if (verbose) {
                        std::cout << "  Line " << lineNum << ": label " << label << std::endl;
                    }
                    codeWriter.writeLabel(label);
                }
                else if (type == CommandType::C_GOTO) {
                    std::string label = parser.arg1();
                    if (verbose) {
                        std::cout << "  Line " << lineNum << ": goto " << label << std::endl;
                    }
                    codeWriter.writeGoto(label);
                }
                else if (type == CommandType::C_IF) {
                    std::string label = parser.arg1();
                    if (verbose) {
                        std::cout << "  Line " << lineNum << ": if-goto " << label << std::endl;
                    }
                    codeWriter.writeIf(label);
                }
                else if (type == CommandType::C_FUNCTION) {
                    std::string functionName = parser.arg1();
                    int numLocals = parser.arg2();
                    if (verbose) {
                        std::cout << "  Line " << lineNum << ": function " << functionName << " " << numLocals << std::endl;
                    }
                    codeWriter.writeFunction(functionName, numLocals);
                }
                else if (type == CommandType::C_CALL) {
                    std::string functionName = parser.arg1();
                    int numArgs = parser.arg2();
                    if (verbose) {
                        std::cout << "  Line " << lineNum << ": call " << functionName << " " << numArgs << std::endl;
                    }
                    codeWriter.writeCall(functionName, numArgs);
                }
                else if (type == CommandType::C_RETURN) {
                    if (verbose) {
                        std::cout << "  Line " << lineNum << ": return" << std::endl;
                    }
                    codeWriter.writeReturn();
                }
                else {
                    throw std::runtime_error("Unknown command type at line " + std::to_string(lineNum) + " in file " + vmFile);
                }
            }
        }
        
        codeWriter.close();
        
        std::cout << "Successfully translated to " << outputFile << std::endl;
        
    } catch (const std::exception& e) {
        std::cerr << "ERROR: " << e.what() << std::endl;
        return 1;
    }
    
    return 0;
}
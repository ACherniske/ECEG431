#include <iostream>
#include <cassert>
#include <string>
#include <vector>
#include "parser.h"
#include "code.h"
#include "symboltable.h"
#include "assembler.h"

void test_code_module(){
    std::cout << "Testing Code class..." << std::endl;

    Code code;

    //test dest code
    assert(code.dest("") == "000");
    assert(code.dest("M") == "001");
    assert(code.dest("D") == "010");
    assert(code.dest("MD") == "011");
    assert(code.dest("A") == "100");
    assert(code.dest("AM") == "101");
    assert(code.dest("AD") == "110");
    assert(code.dest("AMD") == "111");

    //test comp code
    assert(code.comp("0") == "0101010");
    assert(code.comp("1") == "0111111");
    assert(code.comp("D") == "0001100");
    assert(code.comp("A") == "0110000");
    assert(code.comp("M") == "1110000");
    assert(code.comp("D+A") == "0000010");
    assert(code.comp("D+M") == "1000010");

    //test jump code
    assert(code.jump("") == "000");
    assert(code.jump("JGT") == "001");
    assert(code.jump("JEQ") == "010");
    assert(code.jump("JGE") == "011");
    assert(code.jump("JLT") == "100");
    assert(code.jump("JNE") == "101");
    assert(code.jump("JLE") == "110");
    assert(code.jump("JMP") == "111");

    std::cout << "Code class tests passed!" << std::endl;
}

void test_symbol_table_module() {
    std::cout << "Testing SymbolTable module..." << std::endl;

    SymbolTable symbolTable;

    // Test predefined symbols
    assert(symbolTable.contains("SP"));
    assert(symbolTable.getAddress("SP") == 0);
    assert(symbolTable.contains("LCL"));
    assert(symbolTable.getAddress("LCL") == 1);
    assert(symbolTable.contains("ARG"));
    assert(symbolTable.getAddress("ARG") == 2);
    assert(symbolTable.contains("THIS"));
    assert(symbolTable.getAddress("THIS") == 3);
    assert(symbolTable.contains("THAT"));
    assert(symbolTable.getAddress("THAT") == 4);
    assert(symbolTable.contains("R0"));
    assert(symbolTable.getAddress("R0") == 0);
    assert(symbolTable.contains("R15"));
    assert(symbolTable.getAddress("R15") == 15);
    assert(symbolTable.contains("SCREEN"));
    assert(symbolTable.getAddress("SCREEN") == 16384);
    assert(symbolTable.contains("KBD"));
    assert(symbolTable.getAddress("KBD") == 24576);
    
    assert(!symbolTable.contains("LOOP"));
    symbolTable.addEntry("LOOP", 10);
    assert(symbolTable.contains("LOOP"));
    assert(symbolTable.getAddress("LOOP") == 10);

    std::cout << "SymbolTable module tests passed!" << std::endl;
}

void test_parser_with_sample_file() {
    std::cout << "Testing Parser module..." << std::endl;

    //create a test file
    std::ofstream testFile("test_input.asm");
    testFile << "// This is a comment\n";
    testFile << "@2\n";
    testFile << "D=A\n";
    testFile << "@3\n";
    testFile << "D=D+A\n";
    testFile << "(LOOP)\n";
    testFile << "D;JGT\n";
    testFile.close();

    Parser parser("test_input.asm");

    //test first intruction: @2
    assert(parser.hasMoreCommands());
    parser.advance();
    assert(parser.commandType() == InstructionType::A_INSTRUCTION);
    assert(parser.symbol() == "2");

    //test second instruction: D=A
    assert(parser.hasMoreCommands());
    parser.advance();
    assert(parser.commandType() == InstructionType::C_INSTRUCTION);
    assert(parser.dest() == "D");
    assert(parser.comp() == "A");
    assert(parser.jump() == "");

    //test third instruction: @3
    assert(parser.hasMoreCommands());
    parser.advance();
    assert(parser.commandType() == InstructionType::A_INSTRUCTION);
    assert(parser.symbol() == "3");

    //test fourth instruction: D=D+A
    assert(parser.hasMoreCommands());
    parser.advance();
    assert(parser.commandType() == InstructionType::C_INSTRUCTION);
    assert(parser.dest() == "D");
    assert(parser.comp() == "D+A");
    assert(parser.jump() == "");
    
    //test fifth instruction: (LOOP)
    assert(parser.hasMoreCommands());
    parser.advance();
    assert(parser.commandType() == InstructionType::L_INSTRUCTION);
    assert(parser.symbol() == "LOOP");

    //test sixth instruction: D;JGT
    parser.advance();
    assert(parser.commandType() == InstructionType::C_INSTRUCTION);
    assert(parser.dest() == "");
    assert(parser.comp() == "D");
    assert(parser.jump() == "JGT");
    
    assert(!parser.hasMoreCommands());

    //cleanup
    std::remove("test_input.asm");

    std::cout << "Parser module tests passed!" << std::endl;
}

void test_full_assembly() {
    std::cout << "Testing full assembly process..." << std::endl;

    //create a test file
    std::ofstream testFile("test_program.asm");
    testFile << "// Simple test program\n";
    testFile << "@2\n";           // Should be: 0000000000000010
    testFile << "D=A\n";          // Should be: 1110110000010000
    testFile << "@sum\n";         // Should be: 0000000000010000 (variable at RAM[16])
    testFile << "M=D\n";          // Should be: 1110001100001000
    testFile << "(LOOP)\n";       // Label at instruction 4
    testFile << "@LOOP\n";        // Should be: 0000000000000100
    testFile << "0;JMP\n";        // Should be: 1110101010000111
    testFile.close();
    
    // Assemble the test program
    Assembler assembler("test_program.asm", false);
    assembler.assemble("test_program.hack");
    
    // Read and verify the output
    std::ifstream outputFile("test_program.hack");
    std::vector<std::string> expectedOutput = {
        "0000000000000010",  // @2
        "1110110000010000",  // D=A
        "0000000000010000",  // @sum (variable at 16)
        "1110001100001000",  // M=D
        "0000000000000100",  // @LOOP (label at 4)
        "1110101010000111"   // 0;JMP
    };
    
    std::string line;
    int lineNum = 0;
    while (std::getline(outputFile, line) && lineNum < expectedOutput.size()) {
        assert(line == expectedOutput[lineNum]);
        lineNum++;
    }
    assert(lineNum == expectedOutput.size());
    
    // Clean up
    std::remove("test_program.asm");
    std::remove("test_program.hack");
    
    std::cout << "Full assembly tests passed!" << std::endl;
}

int main() {
    try {
        test_code_module();
        test_symbol_table_module();
        test_parser_with_sample_file();
        test_full_assembly();

        std::remove("test_input.asm");
        std::remove("test_program.asm");
        std::remove("test_program.hack");

        std::cout << "All tests passed successfully!" << std::endl;

    } catch (const std::exception& e) {
        std::cerr << "Test failed: " << e.what() << std::endl;
        return 1;
    } catch (...) {
        std::cerr << "An unknown error occurred during testing." << std::endl;
        return 1;
    }
}
#include "Common/Parser.hpp"
#include "Common/Error.hpp"
#include <filesystem>
#include <fstream>
#include <iostream>
#include <sstream>
#include <string>

int main(int argc, char* argv[]) try {
    if (argc != 2) {
        throw slr_parser::FileError("Usage: " + std::string(argv[0]) + " <source-file>");
    }

    std::string filename = argv[1];
    if (!std::filesystem::exists(filename)) {
        throw slr_parser::FileError("File does not exist: " + filename);
    }

    std::ifstream file(filename);
    if (!file.is_open()) {
        throw slr_parser::FileError("Could not open file: " + filename);
    }

    std::stringstream buffer;
    buffer << file.rdbuf();
    std::string input = buffer.str();

    slr_parser::parse_program(input, std::cout);

    return 0;

} catch (const slr_parser::Error& e) {
    std::cerr << "Error: " << e.what() << '\n';
    return 1;
} catch (const std::exception& e) {
    std::cerr << "Unexpected error: " << e.what() << '\n';
    return 1;
}
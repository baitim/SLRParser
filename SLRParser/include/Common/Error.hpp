#pragma once

#include <stdexcept>
#include <string>
#include <format>

namespace slr_parser {

class Error : public std::runtime_error {
public:
    Error(const std::string& msg) : std::runtime_error(msg) {}
};

class FileError : public Error {
public:
    FileError(const std::string& filename)
        : Error(std::format("File error: {}", filename)) {}
    FileError(const std::string& msg, const std::string& filename)
        : Error(std::format("{}: {}", msg, filename)) {}
};

} // namespace slr_parser
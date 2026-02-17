#pragma once

#include <stdexcept>
#include <string>

namespace slr_parser {

class Error : public std::runtime_error {
public:
    Error(const std::string& msg) : std::runtime_error(msg) {}
};

class ParseError : public Error {
public:
    ParseError(const std::string& msg) : Error(msg) {}
};

class FileError : public Error {
public:
    FileError(const std::string& msg) : Error(msg) {}
};

} // namespace slr_parser
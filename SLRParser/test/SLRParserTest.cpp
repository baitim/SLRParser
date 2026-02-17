#include "Common/Parser.hpp"
#include <gtest/gtest.h>
#include <sstream>

TEST(SLRParserTest, EmptyProgram) {
    std::ostringstream out;
    bool result = slr_parser::parse_program("", out);
    EXPECT_TRUE(result);
}

TEST(SLRParserTest, SimpleVariable) {
    std::ostringstream out;
    bool result = slr_parser::parse_program("a;", out);
    EXPECT_TRUE(result);
}

TEST(SLRParserTest, ExpressionWithOperators) {
    std::ostringstream out;
    bool result = slr_parser::parse_program("a+b*c;", out);
    EXPECT_TRUE(result);
}

TEST(SLRParserTest, ExpressionWithParentheses) {
    std::ostringstream out;
    bool result = slr_parser::parse_program("(a+b)*c;\ng-45;", out);
    EXPECT_TRUE(result);
}

TEST(SLRParserTest, InvalidExpression) {
    std::ostringstream out;
    EXPECT_THROW(slr_parser::parse_program("a+", out), slr_parser::ParseError);
}

int main(int argc, char **argv) {
    ::testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}
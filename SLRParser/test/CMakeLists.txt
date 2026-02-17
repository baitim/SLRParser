find_package(GTest REQUIRED)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include)

add_executable(slr-parser-test SLRParserTest.cpp)
target_link_libraries(slr-parser-test
    PRIVATE
    Common
    GTest::GTest
)
add_dependencies(slr-parser-test slr-parser)


set(RUN_TESTS ./slr-parser-test --gtest_color=yes)
add_test(
    NAME slr_parser_test_target
    COMMAND ${RUN_TESTS}
)
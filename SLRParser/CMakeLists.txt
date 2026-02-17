find_package(FLEX REQUIRED)

set(INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/include)

# --------------- Common ---------------

set(COMMON_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/Common)

flex_target(lexer
    ${COMMON_LIB_DIR}/lexer.l
    ${CMAKE_CURRENT_BINARY_DIR}/lexer.cpp
)

add_library(Common STATIC
    ${COMMON_LIB_DIR}/Grammar.cpp
    ${COMMON_LIB_DIR}/Lexer.cpp
    ${COMMON_LIB_DIR}/LR0Items.cpp
    ${COMMON_LIB_DIR}/Parser.cpp
    ${FLEX_lexer_OUTPUTS}
)
target_include_directories(Common PUBLIC
    $<BUILD_INTERFACE:${INCLUDE_DIR}>
    $<INSTALL_INTERFACE:include>
)

# -------------- Execution -------------

set(SLRPARSER_LIB_DIR ${CMAKE_CURRENT_SOURCE_DIR}/lib/SLRParser)
add_executable(slr-parser ${SLRPARSER_LIB_DIR}/SLRParser.cpp)
target_link_libraries(slr-parser Common)

# ---------------- Test ----------------

if(ENABLE_TESTS)
    add_subdirectory(./test)
endif()

# -------------- Package ---------------

if(DEFINED ENV{CONAN_PACKAGE})
    include(GNUInstallDirs)

    set_target_properties(Common PROPERTIES EXPORT_NAME SLRParser)

    install(TARGETS Common EXPORT SLRParserTargets
        LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
        ARCHIVE DESTINATION ${CMAKE_INSTALL_LIBDIR}
        RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
        INCLUDES DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}
    )

    install(DIRECTORY ${INCLUDE_DIR}/ DESTINATION ${CMAKE_INSTALL_INCLUDEDIR})

    install(EXPORT SLRParserTargets
        FILE SLRParserConfig.cmake
        DESTINATION ${CMAKE_INSTALL_LIBDIR}/cmake/SLRParser
    )
endif()
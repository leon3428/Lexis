configure_file(LexisConfig.hpp.in ../LexisConfig.hpp)

add_executable(${PROJECT_NAME} 
    Lexis.cpp
    RegexSyntaxTree.cpp
    Dfa.cpp
    Minimizer.cpp
)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
)
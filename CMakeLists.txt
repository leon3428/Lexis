cmake_minimum_required(VERSION 3.10)

project(Lexis VERSION 0.1)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -DENABLE_DEBUG_MACRO")

configure_file(src/LexisConfig.h.in LexisConfig.h)

add_executable(${PROJECT_NAME} src/Lexis.cpp)

target_include_directories(${PROJECT_NAME} PUBLIC
    "${PROJECT_BINARY_DIR}"
)
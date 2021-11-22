cmake_minimum_required(VERSION 3.16)
project(mal)

set(CMAKE_CXX_STANDARD 17)

add_executable(step1_read_print impls/cpp/step1_read_print.cpp
        impls/cpp/Lexer.cpp impls/cpp/Reader.cpp impls/cpp/types.cpp impls/cpp/Error.cpp)

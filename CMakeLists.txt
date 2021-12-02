cmake_minimum_required(VERSION 3.16)
project(mal)

set(CMAKE_CXX_STANDARD 17)

add_executable(
        step7_file
        impls/cpp/src/step7_quote.cpp
        impls/cpp/src/Lexer.cpp
        impls/cpp/src/Reader.cpp
        impls/cpp/src/types.cpp
        impls/cpp/src/Error.cpp
        impls/cpp/src/rep.cpp
        impls/cpp/src/Environment.cpp
        impls/cpp/src/core_ns.cpp
        )

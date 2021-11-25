cmake_minimum_required(VERSION 3.16)
project(mal)

set(CMAKE_CXX_STANDARD 17)

add_executable(step4_if_fn_do impls/cpp/step4_if_fn_do.cpp
        impls/cpp/Lexer.cpp impls/cpp/Reader.cpp impls/cpp/types.cpp impls/cpp/Error.cpp impls/cpp/rep.cpp impls/cpp/Environment.cpp impls/cpp/Environment.h)

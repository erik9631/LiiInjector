cmake_minimum_required(VERSION 3.25)
project(LiiInjector)
set(CMAKE_CXX_STANDARD 17)
option(LII_INJECTOR_BUILD_TESTS "Build tests" OFF)

add_subdirectory(src)
if(LII_INJECTOR_BUILD_TESTS)
    add_subdirectory(libs/doctest)
    add_subdirectory(tests)
endif()
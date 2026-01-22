cmake_minimum_required(VERSION 3.10)
project(multithreading_ex C)

foreach(NUM RANGE 1 10)
    add_executable(ex${NUM} src/ex${NUM}.c)
endforeach()

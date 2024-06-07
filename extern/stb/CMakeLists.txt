cmake_minimum_required(VERSION 3.20)

project(stb)

file(GLOB SOURCES *.c *.h)
add_library(stb ${SOURCES})
target_include_directories(stb PUBLIC ${PROJECT_SOURCE_DIR})

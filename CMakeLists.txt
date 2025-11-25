cmake_minimum_required(VERSION 3.15)
project(zenith LANGUAGES C)

set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED ON)

add_compile_options(-Wall -Wextra -Wabi)

file(GLOB SRCS 
  src/*.c
  src/util/*.c
)

add_executable(zenith ${SRCS})

target_compile_definitions(zenith PRIVATE
  $<$<CONFIG:Debug>:ZN_DEBUG=1>
)

target_include_directories(zenith PRIVATE src)

target_precompile_headers(zenith PRIVATE src/pch.h)


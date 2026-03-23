cmake_minimum_required(VERSION 3.16)
project(Arcade CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_SOURCE_DIR}/lib)

add_executable(arcade
    src/core/main.cpp
    src/core/Core.cpp
    src/abstracts/AGameModule.cpp
)

target_include_directories(arcade PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(arcade PRIVATE dl)
target_compile_options(arcade PRIVATE -Wall -Wextra -Werror)

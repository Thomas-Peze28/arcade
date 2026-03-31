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

find_package(Curses REQUIRED)

add_library(arcade_ncurses SHARED
    src/graphics/ncurses/NCursesDisplay.cpp
    src/abstracts/ADisplayModule.cpp
)
target_include_directories(arcade_ncurses PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(arcade_ncurses PRIVATE -Wall -Wextra -Werror -fno-gnu-unique)
target_link_libraries(arcade_ncurses PRIVATE ${CURSES_LIBRARIES})
set_target_properties(arcade_ncurses PROPERTIES PREFIX "")

add_library(arcade_minesweeper SHARED
    src/games/minesweeper/Minesweeper.cpp
    src/abstracts/AGameModule.cpp
)
target_include_directories(arcade_minesweeper PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_compile_options(arcade_minesweeper PRIVATE -Wall -Wextra -Werror -fno-gnu-unique)
set_target_properties(arcade_minesweeper PROPERTIES PREFIX "")

cmake_minimum_required(VERSION 3.28)
project(tetris_core)

set(CMAKE_CXX_STANDARD 20)

#For the Curses library to load:
find_package(Curses REQUIRED)
include_directories(${CURSES_INCLUDE_DIR})

find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})

include_directories(
        "src/"
        "src/interfaces/"
        "src/tetris/"
        "src/curses/")

file(GLOB SOURCES
        "src/tetris/*.cpp"
)

add_library(TetrisCore
        ${SOURCES})

add_executable(TetraCurses
        "src/main.cpp"
        "src/curses/TetraCurses.cpp")

target_link_libraries(TetraCurses
        PRIVATE
        TetrisCore
        ${CURSES_LIBRARIES}
        ${Boost_LIBRARIES})

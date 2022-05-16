cmake_minimum_required(VERSION 3.14)

project(webserv LANGUAGES CXX VERSION 0.1)

set(SOURCE_DIR srcs)
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

file(GLOB_RECURSE SOURCES ${PROJECT_SOURCE_DIR}/${SOURCE_DIR}/*.cpp ${PROJECT_SOURCE_DIR}/${SOURCE_DIR}/**/*.cpp)

add_executable(surf ${SOURCES})

target_link_libraries(surf pthread)

target_include_directories(surf PUBLIC includes)

add_subdirectory(tests)
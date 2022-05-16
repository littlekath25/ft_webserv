cmake_minimum_required(VERSION 3.17)
project(tests LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED True)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall -Werror -Wextra")

# to use libcurl the following must be installed
# 1. sudo apt-get install libssl-dev
# 2. ./configure --with-openssl --prefix=$PWD
# 3. make
# 4. make install

include(FetchContent)
FetchContent_Declare(
    googletest
    URL https://github.com/google/googletest/archive/609281088cfefc76f9d0ce82e1ff6c30cc3591e5.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

file(GLOB_RECURSE TEST_SOURCES unittests/*.cpp)
file(GLOB_RECURSE MAIN_SOURCES ../srcs/*.cpp)
list(FILTER MAIN_SOURCES EXCLUDE REGEX ../srcs/main.cpp)

enable_testing()

add_executable(tests unittests/utils.cpp ${TEST_SOURCES} ${MAIN_SOURCES})
target_link_libraries(tests gtest_main curl)
target_include_directories(tests PUBLIC ../includes PUBLIC unittests)

add_definitions(-DTESTRUN)

include(GoogleTest)
gtest_discover_tests(tests)

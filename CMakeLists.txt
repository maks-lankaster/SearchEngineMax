cmake_minimum_required(VERSION 3.22)
project(SearchEngineMax)

set(CMAKE_CXX_STANDARD 17)

set (CMAKE_EXE_LINKER_FLAGS ${CMAKE_EXE_LINKER_FLAGS} "-static")

add_executable(SearchEngineMax main.cpp
        src/SearchServer.cpp
        src/ConverterJSON.cpp
        src/InvertedIndex.cpp
        include/SearchServer.h
        include/ConverterJSON.h
        include/InvertedIndex.h
        include/Exceptions.h)

# FETCH nlohmann json library

include(FetchContent)

FetchContent_Declare(json URL https://github.com/nlohmann/json/releases/download/v3.11.2/json.tar.xz)

FetchContent_MakeAvailable(json)

target_link_libraries(SearchEngineMax PRIVATE nlohmann_json::nlohmann_json)


#set(JSON_BuildTests OFF CACHE INTERNAL "")

#add_subdirectory(nlohmann_json)

target_include_directories(SearchEngineMax
        PRIVATE
        src
        include
        tests)

#target_link_libraries(SearchEngineMax PRIVATE nlohmann_json::nlohmann_json)

#GOOGLE TEST STUFF
include(FetchContent)
FetchContent_Declare(
        googletest
        URL https://github.com/google/googletest/archive/5376968f6948923e2411081fd9372e71a59d8e77.zip
)

include_directories(${gtest_SOURCE_DIR}/include ${gtest_SOURCE_DIR})

set(gtest_disable_pthreads on)

set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(googletest)

enable_testing()

target_link_libraries(SearchEngineMax PRIVATE gtest gtest_main)

include(GoogleTest)

gtest_discover_tests(SearchEngineMax)


#GOOGLE TEST EXECUTABLE

add_executable(RunTests tests/TestInvertedIndex.cpp
        tests/TestSearchServer.cpp
        src/SearchServer.cpp
        src/ConverterJSON.cpp
        src/InvertedIndex.cpp
        include/SearchServer.h
        include/ConverterJSON.h
        include/InvertedIndex.h
        include/Exceptions.h)

target_include_directories(RunTests
        PRIVATE
        src
        include
        tests)

target_link_libraries(RunTests PRIVATE gtest gtest_main)

target_link_libraries(RunTests PRIVATE nlohmann_json::nlohmann_json)
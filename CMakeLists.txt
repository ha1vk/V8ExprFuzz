cmake_minimum_required(VERSION 3.11)

project(V8ExprFuzz C CXX)

find_package(PkgConfig REQUIRED)

if (NOT CMAKE_BUILD_TYPE AND NOT CMAKE_CONFIGURATION_TYPES)
    message(STATUS "No build type selected, default to Release")
    set(CMAKE_BUILD_TYPE "Release")
endif()

#set(CMAKE_CXX_FLAGS "-g -fsanitize=address -fno-omit-frame-pointer")
#set(CMAKE_C_FLAGS "-g -fsanitize=address -fno-omit-frame-pointer")
#SET (CMAKE_C_COMPILER clang)
#SET (CMAKE_CXX_COMPILER clang++)

if (CMAKE_BUILD_TYPE MATCHES "Release")
    set(CMAKE_CXX_FLAGS "-O3")
    set(CMAKE_C_FLAGS "-O3")
elseif (CMAKE_BUILD_TYPE MATCHES "ONLYDebug")
    set(CMAKE_CXX_FLAGS "-O0 -DDEBUG -g")
    set(CMAKE_C_FLAGS "-O0 -DDEBUG -g")
elseif (CMAKE_BUILD_TYPE MATCHES "FUZZ")
set(CMAKE_CXX_FLAGS "-O0 -DDEBUG -g -fsanitize=address,fuzzer -fno-omit-frame-pointer")
set(CMAKE_C_FLAGS "-O0 -DDEBUG -g -fsanitize=address,fuzzer -fno-omit-frame-pointer")
elseif (CMAKE_BUILD_TYPE MATCHES "Debug")
    set(CMAKE_CXX_FLAGS "-O0 -DDEBUG -g -fsanitize=address -fno-omit-frame-pointer")
    set(CMAKE_C_FLAGS "-O0 -DDEBUG -g -fsanitize=address -fno-omit-frame-pointer")
endif()

set(V8ExprFuzz_PATH .)

pkg_check_modules(LB_LIBS REQUIRED)

set(SOURCE  generator/context.cpp
            generator/expr.cpp
            generator/random.cpp
            generator/value.cpp
            generator/function.cpp
            generator/config.cpp
            generator/generator.cpp
    )

add_executable(V8ExprFuzz ${SOURCE} main.cpp)
add_executable(test ${SOURCE} test.cpp)
add_library(Generator ${SOURCE})
add_executable(afl-v8-expr afl/afl-fuzz.c)

include_directories(${V8ExprFuzz_PATH}/generator/include ${LB_LIBS_INCLUDE_DIRS})
target_link_options(${PROJECT_NAME} PRIVATE ${LB_LIBS_LDFLAGS})
target_link_libraries(${PROJECT_NAME} ${LB_LIBS_LIBRARIES})
target_link_libraries(afl-v8-expr PUBLIC Generator dl)

set_target_properties(${PROJECT_NAME} PROPERTIES
        CXX_STANDARD 11
        C_STANDARD 99
        )

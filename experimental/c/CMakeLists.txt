cmake_minimum_required(VERSION 2.8)

project(condor C)
set(CMAKE_C_COMPILER gcc)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(INCLUDES ${CMAKE_SOURCE_DIR}/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "build/")

macro(set_c_flag FLAG)
    if(${ARGC} GREATER 1)
        set(CMAKE_C_FLAGS_${ARGV1} "${CMAKE_C_FLAGS_${ARGV1}} ${FLAG}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
    endif()
endmacro()

set_c_flag("-Wall")

set(CMAKE_BUILD_TYPE Debug)

set(SOURCE_LIST
	${SOURCE_DIR}/condor/lexer/lexer.c
	${SOURCE_DIR}/condor/mem/allocate.c
	${SOURCE_DIR}/condor/syntax/syntax.c
	${SOURCE_DIR}/condor/token/token.c
	${SOURCE_DIR}/condor/ast/ast.c
	${SOURCE_DIR}/condor/ast/scope.c
	${SOURCE_DIR}/condor/number/number.c
	${SOURCE_DIR}/utils/clock.c
	${SOURCE_DIR}/condor/codegen/codegen.c
)

add_library(CondorLib STATIC ${SOURCE_LIST})
target_include_directories(CondorLib PUBLIC ${SOURCE_DIR})
include_directories(${INCLUDES})
add_executable(condor ${CMAKE_SOURCE_DIR}/main.c ${INCLUDES}/Condor.h)
target_link_libraries(condor CondorLib)
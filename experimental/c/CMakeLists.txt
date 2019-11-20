cmake_minimum_required(VERSION 2.8)

project(condor C)
set(CMAKE_C_COMPILER gcc)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(INCLUDES ${CMAKE_SOURCE_DIR}/include)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "build")

macro(set_c_flag FLAG)
    if(${ARGC} GREATER 1)
        set(CMAKE_C_FLAGS_${ARGV1} "${CMAKE_C_FLAGS_${ARGV1}} ${FLAG}")
    else()
        set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${FLAG}")
    endif()
endmacro()

set_c_flag("-Wall")
set_c_flag("-std=gnu11")

if(CMAKE_BUILD_TYPE MATCHES Debug)
	set_c_flag("-DDEBUG=1")
elseif(CMAKE_BUILD_TYPE MATCHES Release)
	set_c_flag("-DDEBUG=0")
endif()

if(DEBUG_NAMESPACE)
	set_c_flag('-DDEBUG_NAMESPACE=\"${DEBUG_NAMESPACE}\"')
endif()

if(DEBUG_SYNTAX)
	set_c_flag('-DDEBUG_SYNTAX=1')
endif()

if(DEBUG_TRACK)
	set_c_flag('-DDEBUG_TRACK=1')
endif()

if(RUN_TESTS)
	set_c_flag('-DTEST=1')
endif()

if(EXPAND_AST)
	set_c_flag(-DEXPAND_AST=1)
endif()

set(SOURCE_LIST
	${SOURCE_DIR}/condor/lexer/lexer.c
	${SOURCE_DIR}/condor/mem/allocate.c
	${SOURCE_DIR}/condor/syntax/syntax.c
	${SOURCE_DIR}/condor/token/token.c
	${SOURCE_DIR}/condor/ast/ast.c
	${SOURCE_DIR}/condor/ast/astlist.c
	${SOURCE_DIR}/condor/ast/scope.c
	${SOURCE_DIR}/condor/number/number.c
	${SOURCE_DIR}/condor/runner/runner.c
	${SOURCE_DIR}/condor/runner/runner-math.c
	${SOURCE_DIR}/utils/clock.c
	${SOURCE_DIR}/condor/semantic/semantic.c
	${SOURCE_DIR}/condor/semantic/typechecker.c
	${SOURCE_DIR}/utils/string/string.c
	${SOURCE_DIR}/utils/file/file.c
)

add_library(CondorLib STATIC ${SOURCE_LIST})
target_include_directories(CondorLib PUBLIC ${SOURCE_DIR})
include_directories(${INCLUDES})
add_executable(condor ${CMAKE_SOURCE_DIR}/main.c ${INCLUDES}/Condor.h)
target_link_libraries(condor CondorLib)

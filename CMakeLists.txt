cmake_minimum_required(VERSION 2.8)

project(condor C CXX)

set(SOURCE_DIR ${CMAKE_SOURCE_DIR}/src)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)
set(INCLUDES ${CMAKE_SOURCE_DIR}/include)
set(BUILD_DIR ${CMAKE_SOURCE_DIR}/build)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "build/")

macro(set_cpp_flag FLAG)
    if(${ARGC} GREATER 1)
        set(CMAKE_CXX_FLAGS_${ARGV1} "${CMAKE_CXX_FLAGS_${ARGV1}} ${FLAG}")
    else()
        set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${FLAG}")
    endif()
endmacro()

set_cpp_flag("--std=c++11")

set(SOURCE_LIST
	${SOURCE_DIR}/api.cc
	${SOURCE_DIR}/condor/assert.cc
	${SOURCE_DIR}/condor/clock.cc
	${SOURCE_DIR}/condor/flags.cc
	${SOURCE_DIR}/condor/ast/context.cc
	${SOURCE_DIR}/condor/ast/node.cc
	${SOURCE_DIR}/condor/ast/scope.cc
	${SOURCE_DIR}/condor/error/error.cc
	${SOURCE_DIR}/condor/execute/execute.cc
	${SOURCE_DIR}/condor/execute/stack.cc
	${SOURCE_DIR}/condor/mem/allocate.cc
	${SOURCE_DIR}/condor/mem/gc.cc
	${SOURCE_DIR}/condor/mem/isolate.cc
	${SOURCE_DIR}/condor/parser/parser.cc
	${SOURCE_DIR}/condor/scanner/scanner.cc
	${SOURCE_DIR}/condor/semantics/binary.cc
	${SOURCE_DIR}/condor/semantics/internal.cc
	${SOURCE_DIR}/condor/semantics/semantics.cc
	${SOURCE_DIR}/condor/token/token.cc
	${SOURCE_DIR}/condor/types/app/app.cc
	${SOURCE_DIR}/condor/types/clock/clock.cc
	${SOURCE_DIR}/condor/types/fs/fs.cc
	${SOURCE_DIR}/condor/types/path/path.cc
	${SOURCE_DIR}/condor/types/script/script.cc
	${SOURCE_DIR}/condor/types/strings/string.cc
)

add_library(CondorLib STATIC ${SOURCE_LIST})
target_include_directories(CondorLib PUBLIC
	${SOURCE_DIR}
)
# include_directories(${INCLUDES})
add_executable(condor ${SOURCE_DIR}/main.cc ${INCLUDES}/Condor.h)
target_link_libraries(condor CondorLib)


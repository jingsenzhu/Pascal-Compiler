# set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")

project(spc)

cmake_minimum_required(VERSION 3.10)
set(CMAKE_CXX_STANDARD 14)
set(CMAKE_BUILD_TYPE Debug)

if(EXISTS "${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp" AND
        EXISTS "${CMAKE_CURRENT_BINARY_DIR}/parser.cpp" AND
        EXISTS "${CMAKE_CURRENT_BINARY_DIR}/parser.hpp")
        message(STATUS "Found scanner and parser exists")
        set(FLEX_spc_lexer_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp")
        set(BISON_spc_parser_OUTPUTS "${CMAKE_CURRENT_BINARY_DIR}/parser.cpp;${CMAKE_CURRENT_BINARY_DIR}/parser.hpp")
else()
	find_package(BISON REQUIRED)
	find_package(FLEX REQUIRED)

	BISON_TARGET(spc_parser
			${CMAKE_SOURCE_DIR}/src/parser.y
			${CMAKE_CURRENT_BINARY_DIR}/parser.cpp
			COMPILE_FLAGS "--defines=${CMAKE_CURRENT_BINARY_DIR}/parser.hpp --verbose")
	FLEX_TARGET(spc_lexer
			${CMAKE_SOURCE_DIR}/src/scanner.l
			${CMAKE_CURRENT_BINARY_DIR}/scanner.cpp)
	ADD_FLEX_BISON_DEPENDENCY(spc_lexer spc_parser)
endif()

find_package(LLVM REQUIRED CONFIG)

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig.cmake in: ${LLVM_DIR}")

include_directories(${CMAKE_SOURCE_DIR}/src/ ${CMAKE_BINARY_DIR} ${LLVM_INCLUDE_DIRS})
add_definitions(${LLVM_DEFINITIONS})
#add_executable(simple-tool tool.cpp)


file(GLOB SOURCE_FILES
        "src/*.cpp"
        "src/*.hpp"
        "src/ast/*.hpp"
        "src/codegen/*.hpp"
        "src/codegen/*.cpp"
        "src/utils/*.hpp"
        "src/utils/*.cpp"
        )

add_executable(
        ${CMAKE_PROJECT_NAME}
        ${SOURCE_FILES}
        ${FLEX_spc_lexer_OUTPUTS}
        ${BISON_spc_parser_OUTPUTS}
        src/utils/ast.hpp src/codegen/program.cpp src/codegen/decl.cpp src/codegen/type.cpp src/codegen/expr.cpp src/codegen/identifier.cpp src/codegen/stmt.cpp)
target_include_directories(${CMAKE_PROJECT_NAME}
        PRIVATE
        .
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_SOURCE_DIR}/src/
        ${LLVM_INCLUDE_DIRS})
set_property(TARGET ${CMAKE_PROJECT_NAME}
        PROPERTY CXX_STANDARD 14)

llvm_map_components_to_libnames(llvm_libs all)
target_link_libraries(${CMAKE_PROJECT_NAME} ${llvm_libs})
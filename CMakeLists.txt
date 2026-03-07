cmake_minimum_required(VERSION 3.15)

set(CMAKE_CXX_STANDARD 23)

project(deviant)

add_executable(deviant)

# We incorporate the CMake features provided by LLVM:
if($ENV{LLVM_ROOT})
    set(LLVM_ROOT $ENV{LLVM_ROOT})
endif()

find_package(LLVM PATHS "${LLVM_ROOT}/lib/cmake/llvm" "$ENV{ProgramFiles}/lib/cmake/llvm" "C:/LLVM/lib/cmake/llvm")

if(LLVM_FOUND)
    set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${LLVM_DIR}")
else()
    message(FATAL_ERROR "LLVM installation not found. Set LLVM_ROOT to the installation root path.")
endif()

# Find the libraries that correspond to the LLVM components
# that we wish to use
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link against LLVM libraries
target_link_libraries(deviant ${llvm_libs})

include(LLVMConfig)
message(STATUS "Found LLVM Package Version:${LLVM_PACKAGE_VERSION}")
message(STATUS "LLVM Built type : ${LLVM_BUILD_TYPE}")
string(REGEX MATCH "[0-9]+\\.[0-9]+" LLVM_VER ${LLVM_PACKAGE_VERSION})

# if(NOT ${LLVM_VER} VERSION_EQUAL "17.0")
# message(WARNING "LLVM version ${LLVM_VER} is not tested.")
# endif()
include_directories(
    include
    "/usr/lib/llvm-20/include"
)

target_sources(deviant PRIVATE
    main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/user_input.cpp
    src/deviant_llvm.cpp
)
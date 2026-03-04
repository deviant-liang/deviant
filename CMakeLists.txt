cmake_minimum_required(VERSION 3.15)

# --- Project Configuration ---
project(deviant
    VERSION 0.1.0
    LANGUAGES CXX
)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# --- LLVM Dependency Configuration ---
# Set LLVM_DIR as a fallback for custom installations (e.g., C:/LLVM)
set(LLVM_ROOT "C:/LLVM" CACHE PATH "Path to LLVM installation")
list(APPEND CMAKE_MODULE_PATH "${LLVM_ROOT}/lib/cmake/llvm")

# Attempt to find LLVM using the standard package manager logic
find_package(LLVM 17 REQUIRED CONFIG
    PATHS "${LLVM_ROOT}/lib/cmake/llvm"
    NO_DEFAULT_PATH
)

# Manual workaround for broken LLVMConfig.cmake paths in some Windows builds
if(NOT TARGET LLVMSupport)
    include(LLVMConfig)
    include(LLVM-Config)
endif()

message(STATUS "Found LLVM ${LLVM_PACKAGE_VERSION}")
message(STATUS "Using LLVMConfig from ${LLVM_DIR}")

# --- Target Definition ---
add_executable(deviant
    main.cpp
    src/lexer.cpp
    src/parser.cpp
    src/ast.cpp
    src/user_input.cpp
    src/deviant_llvm.cpp
)

# --- Include Directories ---
target_include_directories(deviant PRIVATE
    "include"
    ${LLVM_INCLUDE_DIRS}
)

# --- Compiler Definitions & Flags ---
target_compile_definitions(deviant PRIVATE ${LLVM_DEFINITIONS})

if(MSVC)
    target_compile_options(deviant PRIVATE /W4 /Zc:__cplusplus)
endif()

# --- Linking ---
# Map the required LLVM components to library names
llvm_map_components_to_libnames(llvm_libs support core irreader)

# Link project against LLVM and system-specific libraries
target_link_libraries(deviant PRIVATE
    ${llvm_libs}
)

# Handle Windows-specific system dependencies (Addressing your zlib/shell32 issues)
if(WIN32)
    target_link_libraries(deviant PRIVATE
        shell32
        ntdll
        psapi
        version
        uuid
        advapi32
    )

    # Note: If LLVM was built with Z3 or ZLib, they might be required here.
    # target_link_libraries(deviant PRIVATE zlib)
endif()

# --- Installation (Optional for Open Source) ---
install(TARGETS deviant DESTINATION bin)
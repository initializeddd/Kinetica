cmake_minimum_required(VERSION 3.20)
project(Kinetica
    VERSION 0.1.0
    DESCRIPTION "Free and open-source 3D modeling software specialized for low-poly art"
    HOMEPAGE_URL "https://github.com/initializeddd/Kinetica"
    LANGUAGES CXX
)

# Enforce modern CMake practices
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)  # Prefer -std=c++20 over -std=gnu++20

# Output directories
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

# ---- Options ----
option(KINETICA_BUILD_EXAMPLES "Build example tools or utilities" OFF)
option(KINETICA_ENABLE_WARNINGS "Enable compiler warnings" ON)
option(KINETICA_INSTALL "Generate install rules" ON)

# ---- Dependencies ----
include(FetchContent)

# GLFW
FetchContent_Declare(
    glfw
    GIT_REPOSITORY https://github.com/glfw/glfw.git
    GIT_TAG        3.4
)
set(GLFW_BUILD_EXAMPLES OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_TESTS    OFF CACHE BOOL "" FORCE)
set(GLFW_BUILD_DOCS     OFF CACHE BOOL "" FORCE)
set(GLFW_INSTALL        OFF CACHE BOOL "" FORCE)

# GLEW — Use ONLY the CMake-native version
FetchContent_Declare(
    glew
    GIT_REPOSITORY https://github.com/Perlmint/glew-cmake.git
    GIT_TAG        f456deace7b408655109aaeff71421ef2d3858c6
)
set(BUILD_UTILS OFF CACHE BOOL "" FORCE)
set(glew-cmake_BUILD_SHARED OFF CACHE BOOL "" FORCE)  # Build static only
set(glew-cmake_BUILD_STATIC ON CACHE BOOL "" FORCE)

FetchContent_MakeAvailable(glfw glew)

# System OpenGL (required on Linux/macOS; Windows has it in system DLLs)
find_package(OpenGL REQUIRED)

# ---- Source files ----
file(GLOB_RECURSE KINETICA_SOURCES
    CONFIGURE_DEPENDS
    src/*.cpp
    src/*.cc
)

file(GLOB_RECURSE KINETICA_HEADERS
    CONFIGURE_DEPENDS
    include/*.h
    include/*.hpp
)

# ---- Main executable ----
add_executable(kinetica ${KINETICA_SOURCES} ${KINETICA_HEADERS})

target_compile_definitions(kinetica PRIVATE GLEW_EXPERIMENTAL)

# ---- Include directories ----
target_include_directories(kinetica
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---- Compiler warnings (professional hardening) ----
if(KINETICA_ENABLE_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID STREQUAL "GNU" OR CMAKE_CXX_COMPILER_ID STREQUAL "Clang")
        target_compile_options(kinetica PRIVATE
            -Wall -Wextra -Wpedantic
            -Wshadow -Wnon-virtual-dtor -Wold-style-cast
            -Wcast-align -Wunused -Woverloaded-virtual
            -Wnull-dereference -Wdouble-promotion
            -Wformat=2
        )
    elseif(CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
        target_compile_options(kinetica PRIVATE
            /W4
            /permissive-
        )
    endif()
endif()

# ---- Platform-specific settings ----
if(WIN32)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
    # On Windows, GLEW static needs this define
    target_compile_definitions(kinetica PRIVATE GLEW_STATIC)
elseif(APPLE)
    # macOS: may need -framework OpenGL (handled by OpenGL::GL)
endif()

# ---- Link libraries ----
target_link_libraries(kinetica PRIVATE
    glfw
    libglew_static
    OpenGL::GL
)
# ---- Install rules ----
if(KINETICA_INSTALL)
    install(TARGETS kinetica
        RUNTIME DESTINATION bin
    )
    install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*"
    )
endif()

# ---- Examples (optional) ----
if(KINETICA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ---- Build summary ----
message(STATUS "Kinetica Build Configuration:")
message(STATUS "  C++ Standard: C++20")
message(STATUS "  Build Type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "  Source Dir:   ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "  Binary Dir:   ${CMAKE_BINARY_DIR}")
message(STATUS "  Warnings:     ${KINETICA_ENABLE_WARNINGS}")

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
option(KINETICA_ENABLE_WARNINGS "Enable compiler warnings as errors" ON)
option(KINETICA_INSTALL "Generate install rules" ON)

# ---- Find dependencies----


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

# ---- Include directories (PRIVATE for impl, PUBLIC for API) ----
target_include_directories(kinetica
    PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
        $<INSTALL_INTERFACE:include>
    PRIVATE
        ${CMAKE_CURRENT_SOURCE_DIR}/src
)

# ---- Compile definitions & features ----
target_compile_features(kinetica PRIVATE cxx_std_20)

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
            /permissive-  # Strict standards conformance
        )
    endif()
endif()

# ---- Platform-specific settings ----
if(WIN32)
    # Enable UTF-8 manifest and console support (optional)
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} /MANIFEST:NO")
    # Require admin? Not here—handled by installer
elseif(UNIX AND NOT APPLE)
    # Linux: nothing special yet
elseif(APPLE)
    # macOS: bundle support can be added later
endif()

# ---- Install rules (for future packaging) ----
if(KINETICA_INSTALL)
    install(TARGETS kinetica
        RUNTIME DESTINATION bin
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
    )

    install(DIRECTORY include/
        DESTINATION include
        FILES_MATCHING PATTERN "*.h*"
    )
endif()

# ---- Optional: Add alias for development (e.g., for plugins) ----
add_library(kinetica::core ALIAS kinetica)

# ---- Examples or tools (optional) ----
if(KINETICA_BUILD_EXAMPLES)
    add_subdirectory(examples)
endif()

# ---- Print summary (nice for devs) ----
message(STATUS "Kinetica Build Configuration:")
message(STATUS "  C++ Standard: C++20")
message(STATUS "  Build Type:   ${CMAKE_BUILD_TYPE}")
message(STATUS "  Source Dir:   ${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "  Binary Dir:   ${CMAKE_BINARY_DIR}")
message(STATUS "  Warnings:     ${KINETICA_ENABLE_WARNINGS}")

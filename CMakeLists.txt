cmake_minimum_required(VERSION 3.16)

# Project Definition
project(1bit-playground VERSION 1.0)

# Add this block near the top
option(WITH_FILE_LOADERS "Build with support for toml/tmx file loaders" ON)
if (WITH_FILE_LOADERS)
    message(STATUS "Building with file loaders (toml++, tmxlite)")
    # Use FetchContent to manage external dependencies
    include(FetchContent)
    FetchContent_Declare(
            tomlplusplus
            GIT_REPOSITORY https://github.com/marzer/tomlplusplus.git
            GIT_TAG        v3.3.0
    )
    FetchContent_MakeAvailable(tomlplusplus)
    # --- END SECTION ---

    set(TMXLITE_STATIC_LIB ON)
    set(TMXLITE_USE_ZLIB OFF)
    set(TMXLITE_USE_ZSTD OFF)
    add_subdirectory(${CMAKE_SOURCE_DIR}/src/vendor/tmxlite/tmxlite)
    include_directories(${CMAKE_SOURCE_DIR}/src/vendor/tmxlite/tmxlite/include)
else()
    message(STATUS "Building without legacy file loaders.")
endif ()

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_INTERPROCEDURAL_OPTIMIZATION TRUE)

# Compiler flags to put each function/data in its own section
set(DEAD_CODE_STRIPPING_FLAGS "-ffunction-sections -fdata-sections")
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} ${DEAD_CODE_STRIPPING_FLAGS}")
set(CMAKE_CXX_FLAGS_MINSIZEREL "${CMAKE_CXX_FLAGS_MINSIZEREL} ${DEAD_CODE_STRIPPING_FLAGS}")

# Linker flag to remove unused sections (platform-specific)
if(APPLE)
    set(DEAD_STRIP_LINKER_FLAG "-Wl,-dead_strip")
else()
    set(DEAD_STRIP_LINKER_FLAG "-Wl,--gc-sections")
endif()

set(CMAKE_EXE_LINKER_FLAGS_RELEASE "${CMAKE_EXE_LINKER_FLAGS_RELEASE} ${DEAD_STRIP_LINKER_FLAG}")
set(CMAKE_EXE_LINKER_FLAGS_MINSIZEREL "${CMAKE_EXE_LINKER_FLAGS_MINSIZEREL} ${DEAD_STRIP_LINKER_FLAG}")

# Find SDL2 Library
# On macOS, you can install this with Homebrew: `brew install sdl2`
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add our vendor directory to the include paths
include_directories(src/vendor/entt/single_include)

# --- Automatically find .cpp files ONLY from our source directories ---
file(GLOB_RECURSE CORE_SOURCES "src/core/*.cpp")
file(GLOB_RECURSE COMPONENTS_SOURCES "src/components/*.cpp")
file(GLOB_RECURSE SCENES_SOURCES "src/scenes/*.cpp")
file(GLOB_RECURSE SYSTEMS_SOURCES "src/systems/*.cpp")
file(GLOB_RECURSE UTIL_SOURCES "src/util/*.cpp")

# Define the Executable
# Combine the sources from our specific directories.
add_executable(game
    ${CORE_SOURCES}
        ${COMPONENTS_SOURCES}
    ${SCENES_SOURCES}
    ${SYSTEMS_SOURCES}
    ${UTIL_SOURCES}
)

# Explicitly tell our 'game' target where to find the toml++ header files.
target_include_directories(game PRIVATE ${tomlplusplus_INCLUDE_DIRS})
target_include_directories(game PUBLIC
        $<BUILD_INTERFACE:${CMAKE_CURRENT_LIST_DIR}/include>
        $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
        ${CMAKE_BINARY_DIR}/_deps/tomlplusplus-src/include
)

# Link SDL2 to the Executable
if (WITH_FILE_LOADERS)
    target_link_libraries(game PRIVATE
            tmxlite
            ${SDL2_LIBRARIES}
    )
else ()
    target_link_libraries(game PRIVATE
            ${SDL2_LIBRARIES}
    )
endif ()

# Platform-specific settings for Apple (macOS, iOS)
if(APPLE)
    # This allows CMake to create an application bundle (.app)
    set_target_properties(game PROPERTIES MACOSX_BUNDLE TRUE)

    # This tells SDL2 to use Metal, which is default on modern macOS,
    target_compile_definitions(game PRIVATE SDL_HINT_RENDER_DRIVER="metal")

    # Define the resource directory relative to the CMakeLists.txt file
    set(RESOURCE_DIR "${CMAKE_SOURCE_DIR}/res")

    # Add a custom command to copy the res folder into the .app bundle's Resources directory
    add_custom_command(
        TARGET game POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E copy_directory
                ${RESOURCE_DIR}
                $<TARGET_FILE_DIR:game>/../Resources/res
        COMMENT "Copying resources to bundle"
    )
endif()
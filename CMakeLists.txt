cmake_minimum_required(VERSION 3.16)

# Project Definition
project(1bit-playground VERSION 1.0)

# Set C++ Standard
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

# Find SDL2 Library
# On macOS, you can install this with Homebrew: `brew install sdl2`
find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIRS})

# Add our vendor directory to the include paths
include_directories(src/vendor/entt/include)

# Define the Executable
add_executable(game
    src/core/main.cpp
    src/core/engine.cpp
    src/scenes/game_scene.cpp
    src/systems/renderer.cpp
    src/util/texture_loader.cpp
    src/util/resource_manager.cpp
)

# Link SDL2 to the Executable
target_link_libraries(game ${SDL2_LIBRARIES})

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
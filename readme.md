# 1-Bit Playground

A simple 2D game engine built from scratch in C++20 and SDL2. This project is a playground for learning and experimenting with game engine architecture concepts.

<!-- ![Screenshot of the Player Sprite](https://i.imgur.com/8a68hTf.png) -->

## Table of Contents

- [1-Bit Playground](#1-bit-playground)
  - [Table of Contents](#table-of-contents)
  - [About the Project](#about-the-project)
  - [Getting Started](#getting-started)
    - [Prerequisites](#prerequisites)
    - [Cloning](#cloning)
  - [Usage (for now :3)](#usage-for-now-3)
  - [License](#license)

## About the Project

This project was created to explore the fundamentals of game engine development. It avoids using a pre-built engine framework in favor of building core systems from the ground up.

**Core Features:**
* **Rendering:** Uses SDL2 for cross-platform windowing and 2D rendering.
* **Architecture:** Implements an Entity Component System (ECS) using the powerful **EnTT** library to decouple data from logic.
* **Resource Management:** A centralized `ResourceManager` handles loading and caching of assets.
* **Custom Asset Format:** Features a human-readable `.sprite` format for defining simple sprites, palettes, and animations.
* **Build System:** Uses CMake for easy configuration and building across multiple platforms.

## Getting Started

Follow these steps to get a local copy up and running.

### Prerequisites

You will need the following tools installed on your system:
* [Git](https://git-scm.com/)
* [CMake](https://cmake.org/download/) (version 3.16 or higher)
* A C++20 compatible compiler (e.g., Clang, GCC, MSVC)
* **SDL2 Library**

Here is how to install SDL2 on different platforms:

**macOS (using Homebrew):**
```bash
brew install sdl2
```

**Linux (Debian Based)**
```bash
sudo apt-get install libsdl2-dev
```

**Windows (using vcpkg)**
```bash
git clone [https://github.com/Microsoft/vcpkg.git](https://github.com/Microsoft/vcpkg.git)
cd vcpkg
./bootstrap-vcpkg.bat
./vcpkg install sdl2:x64-windows
```
Then, you'll need to tell CMake where to find the vcpkg toolchain file when you configure the build.

### Cloning

This repository uses Git submodules to manage the EnTT library. To clone the repository and automatically initialize the submodules, use the `--recurse-submodules` flag:

```bash
git clone --recurse-submodules [https://github.com/](https://github.com/)fabzter/1bit-sdl2-playground
cd 1bit-sdl2-playground
```

If you have already cloned the repository without the flag, you can initialize the submodules with this command:

```bash
git submodule update --init --recursive
```

## Usage (for now :3)

To create a new renderable object in the game:

Define a new sprite in a `.sprite` file inside `res/sprites/`.

In `src/scenes/game_scene.cpp`, preload the new asset in the `load()` method.

Create a new entity using `m_registry.create()`.

Attach a `TransformComponent` and a `SpriteComponent` to the new entity. The `RenderSystem` will automatically pick it up and draw it.

** TODO **
* Maybe it is not very flexible to have to modify the game_scene.cpp in order to load. An automatic seach for assets and entities should happen.
  

## License
This project is not currently licensed. You are free to use the code for educational purposes.
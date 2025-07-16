# 1-Bit Playground

A simple 2D game engine built from scratch in C++20 and SDL2. This project is a playground for learning and experimenting with game engine architecture concepts.

<!-- ![Screenshot of the Player Sprite](https://i.imgur.com/8a68hTf.png) -->

## Table of Contents

- [1-Bit Playground](#1-bit-playground)
  - [Table of Contents](#table-of-contents)
  - [About the Project](#about-the-project)
  - [Getting Started](#getting-started)
    - [cloning](#cloning)
    - [Prerequisites](#prerequisites)

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

### cloning

Clone using:

git clone --recurse-submodules https://github.com/fabzter/1bit-sdl2-playground.git

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

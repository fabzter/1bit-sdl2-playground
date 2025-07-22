#include "engine.hpp"
#include "../scenes/game_scene.hpp"

int main(int argc, char* argv[]) {
    Engine engine;
    if (!engine.init()) {
        return 1;
    }

    // The user now define the "recipe" for their scenes.
    engine.registerSceneFactory("Level1", []() {
        auto playerIntentSystem = std::make_unique<PlayerIntentSystem>();
        auto topDownMovementSystem = std::make_unique<TopDownMovementSystem>();
        auto animationSystem = std::make_unique<AnimationSystem>();
        auto renderSystem = std::make_unique<RenderSystem>();
        auto cameraSystem = std::make_unique<CameraSystem>();

        return std::make_unique<GameScene>(
            std::move(playerIntentSystem),
            std::move(topDownMovementSystem),
            std::move(animationSystem),
            std::move(renderSystem),
            std::move(cameraSystem)
        );
    });

    // The user tells the engine which scene to start with.
    engine.run("Level1");

    return 0;
}
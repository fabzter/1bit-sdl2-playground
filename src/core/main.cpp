#include "engine.hpp"
#include "../scenes/game_scene.hpp"

int main(int argc, char* argv[]) {
    Engine engine;
    if (!engine.init()) {
        return 1;
    }

    // The user now define the "recipe" for their scenes.
    engine.registerSceneFactory("Level1", []() {
        return std::make_unique<GameScene>(
            std::make_unique<PlayerIntentSystem>(),
            std::make_unique<TopDownMovementSystem>(),
            std::make_unique<AnimationStateSystem>(),
            std::make_unique<AnimationSystem>(),
            std::make_unique<RenderSystem>(),
            std::make_unique<CameraSystem>()
        );
    });

    // The user tells the engine which scene to start with.
    engine.run("Level1");
    return 0;
}
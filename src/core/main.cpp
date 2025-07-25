#include "engine.hpp"
#include "../scenes/game_scene.hpp"
#include "../util/toml_scene_loader.hpp"

int main(int argc, char* argv[]) {
    Engine engine;
    if (!engine.init()) {
        return 1;
    }

    // Create the scene instance and register it
    auto gameScene = std::make_unique<GameScene>(
        std::make_unique<TomlSceneLoader>(),
        engine.getResourceManager()->getBasePath() + "res/scenes/level1.toml",
        std::make_unique<PlayerIntentSystem>(),
        std::make_unique<TopDownMovementSystem>(),
        std::make_unique<AnimationStateSystem>(),
        std::make_unique<AnimationSystem>(),
        std::make_unique<RenderSystem>(),
        std::make_unique<TilemapRenderSystem>(),
        std::make_unique<CameraSystem>()
    );
    engine.getSceneManager()->registerScene("Level1", std::move(gameScene));

    // The user tells the engine which scene to start with.
    engine.run("Level1");
    return 0;
}
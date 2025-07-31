#include "engine.hpp"
#include "system_manager.hpp"
#include "../scenes/game_scene.hpp"
#include "../util/toml_scene_loader.hpp"
#include "../systems/player_intent_system.hpp"
#include "../systems/character_controller_system.hpp"
#include "../systems/statemachine_system.hpp"
#include "../systems/animation.hpp"
#include "../systems/renderer.hpp"
#include "../systems/debug_render_system.hpp"
#include "../systems/tilemap_render_system.hpp"
#include "../systems/camera_system.hpp"
#include "../systems/debug_info_system.hpp"
#include "../systems/collision_system.hpp"
#include "../systems/physics_system.hpp"
#include "../systems/behavior_system.hpp"
#include "../systems/character_controller_system.hpp"

int main(int argc, char* argv[]) {
    Engine engine;
    if (!engine.init()) {
        return 1;
    }

    // 1. Create the scene object. It's a simple container.
    auto gameScene = std::make_unique<GameScene>(
        std::make_unique<TomlSceneLoader>(),
        engine.getResourceManager()->getBasePath() + "res/scenes/level1.toml"
    );

    // 2. Create and configure the SystemManager (The "Assembler" part)
    auto systemManager = std::make_unique<SystemManager>();
    // Define the world boundaries for the Quadtree.
    // For now, we'll hardcode it. Later, this could come from the scene file.
    //TODO: this data must come frome the scene file or at least the map file. MAy from both! This is a discuccion topic.
    QuadtreeRect worldBounds = {0, 0, 1280, 720};
    // ---- THE CORRECT PHYSICS LOOP ORDER ----
    systemManager->addUpdateSystem(std::make_unique<PlayerIntentSystem>());
    systemManager->addUpdateSystem(std::make_unique<CharacterControllerSystem>());
    systemManager->addUpdateSystem(std::make_unique<PhysicsSystem>());
    systemManager->addUpdateSystem(std::make_unique<CollisionSystem>(worldBounds));
    systemManager->addUpdateSystem(std::make_unique<BehaviorSystem>());
    // ------------------------------------------
    systemManager->addUpdateSystem(std::make_unique<StateMachineSystem>());
    systemManager->addUpdateSystem(std::make_unique<AnimationSystem>());
    systemManager->addUpdateSystem(std::make_unique<CameraSystem>());
    systemManager->addUpdateSystem(std::make_unique<DebugInfoSystem>());

    // The order we add render systems determines the draw order (background first)
    systemManager->addRenderSystem(std::make_unique<TilemapRenderSystem>());
    systemManager->addRenderSystem(std::make_unique<RenderSystem>());
    systemManager->addRenderSystem(std::make_unique<DebugRenderSystem>());

    // 3. Inject the configured manager into the scene.
    gameScene->setSystemManager(std::move(systemManager));

    engine.getSceneManager()->registerScene("Level1", std::move(gameScene));

    // The user tells the engine which scene to start with.
    engine.run("Level1");
    return 0;
}
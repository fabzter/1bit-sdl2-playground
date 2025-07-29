#include "engine.hpp"
#include "system_manager.hpp"
#include "../scenes/game_scene.hpp"
#include "../util/toml_scene_loader.hpp"
#include "../systems/player_intent_system.hpp"
#include "../systems/top_down_movement_system.hpp"
#include "../systems/animation_state_system.hpp"
#include "../systems/animation.hpp"
#include "../systems/renderer.hpp"
#include "../systems/tilemap_render_system.hpp"
#include "../systems/camera_system.hpp"
#include "../systems/debug_info_system.hpp"
#include "../systems/collision_system.hpp"
#include "../systems/physics_system.hpp"

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
    QuadtreeRect worldBounds = {0, 0, 1280, 720};
    // ---- THE CORRECT PHYSICS LOOP ORDER ----
    systemManager->addUpdateSystem(std::make_unique<PlayerIntentSystem>());      // 1. Propose: Input updates velocity intent
    systemManager->addUpdateSystem(std::make_unique<TopDownMovementSystem>());  //    (This system does the same thing)
    systemManager->addUpdateSystem(std::make_unique<CollisionSystem>(worldBounds)); // 2. Resolve: Collision system corrects velocity
    systemManager->addUpdateSystem(std::make_unique<PhysicsSystem>());          // 3. Apply: Physics system applies final velocity to position
    // ------------------------------------------
    systemManager->addUpdateSystem(std::make_unique<AnimationStateSystem>());
    systemManager->addUpdateSystem(std::make_unique<AnimationSystem>());
    systemManager->addUpdateSystem(std::make_unique<CameraSystem>());
    systemManager->addUpdateSystem(std::make_unique<DebugInfoSystem>());

    // The order we add render systems determines the draw order (background first)
    systemManager->addRenderSystem(std::make_unique<TilemapRenderSystem>());
    systemManager->addRenderSystem(std::make_unique<RenderSystem>());

    // 3. Inject the configured manager into the scene.
    gameScene->setSystemManager(std::move(systemManager));

    engine.getSceneManager()->registerScene("Level1", std::move(gameScene));

    // The user tells the engine which scene to start with.
    engine.run("Level1");
    return 0;
}
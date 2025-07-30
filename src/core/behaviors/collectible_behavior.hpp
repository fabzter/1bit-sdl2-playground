#include "icollision_responder.hpp"
#include "../../components/player_control.hpp"

class CollectibleBehavior : public ICollisionResponder {
public:
    void onCollision(entt::entity self, entt::entity other, entt::registry& registry) override {
        // Only trigger if the "other" entity is the player
        if (registry.all_of<PlayerControlComponent>(other)) {
            std::cout << "Collected entity " << static_cast<uint32_t>(self) << "!" << std::endl;
            // Add to score, play sound, etc.
            registry.destroy(self);
        }
    }
};

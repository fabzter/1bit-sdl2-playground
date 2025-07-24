#pragma once

namespace BlackboardKeys {

    namespace State {
        inline constexpr const char* IsMoving = "isMoving";
    }

    namespace Camera {
        inline constexpr const char* FollowSpeed = "followSpeed";
        inline constexpr const char* DeadZoneRadius = "deadZoneRadius";
        inline constexpr const char* Target = "cameraTarget";
    }

    // We can add more categories as needed, for example:
    // namespace Combat {
    //     inline constexpr const char* Health = "health";
    //     inline constexpr const char* IsAttacking = "isAttacking";
    // }
}

#pragma once

namespace Game {
    namespace UI {
        namespace HUD {
            extern bool targetLock;
            extern Fixed2D4 targetPosition;
            extern Fixed2D4 targetVelocity;
            void draw();
        }
    }
}
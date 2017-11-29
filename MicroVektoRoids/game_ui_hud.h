#pragma once

namespace Game {
    namespace UI {
        namespace HUD {
            extern bool targetLock;
            extern Fixed2D4 targetPosition;
            extern Fixed2D4 targetVelocity;
            extern bool targetIsShip;
            extern uint8_t targetIndex;
            extern uint8_t targetDamage;
            extern uint8_t targetMaxDamage;
            extern const char* highlightInfo;
            void draw();
        }
    }
}

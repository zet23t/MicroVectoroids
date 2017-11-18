#pragma once
#include "lib_FixedMath.h"

namespace Game {
    #define ShipCount 4
    struct Ship {
        Fixed2D4 pos;
        Fixed2D4 prevPos;
        Fixed2D4 direction;
        Fixed2D4 velocity;
        uint8_t type;
        uint8_t damage;
        uint8_t shootCooldown;
        uint8_t takenHitCooldown;

        void tick();
        void draw();
        void init(int8_t type, int16_t x, int16_t y, int8_t dx, int8_t dy);
        void shoot();
        void takeDamage(uint8_t dmg);
    };

    struct ShipManager {
        Ship ships[ShipCount];
        void tick();
        void draw();
        void init();
    };
    extern ShipManager shipManager;
    extern bool directionalControls;
}

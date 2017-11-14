#pragma once
#include "lib_FixedMath.h"

namespace Game {
    #define AsteroidsCount 64

    struct Asteroid {
        Fixed2D4 pos;
        Fixed2D4 velocity;
        uint8_t type;
        uint8_t hits;
        void tick();
        void draw();
        void init(uint8_t type, int16_t x, int16_t y);
    };

    struct AsteroidManager {
        Asteroid asteroids[AsteroidsCount];
        void init();
        void tick();
        void draw();
        Asteroid* spawn();
    };
}

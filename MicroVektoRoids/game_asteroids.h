#pragma once
#include "lib_FixedMath.h"

namespace Game {
    #define AsteroidsCount 64
    namespace AsteroidType {
        extern const uint8_t White;
        extern const uint8_t Red;
        extern const uint8_t Yellow;
    }

    struct Asteroid {
        Fixed2D4 pos;
        Fixed2D4 velocity;
        uint8_t type;
        uint8_t hits;
        void tick();
        void drawOnUI(int8_t x, int8_t y);
        void draw();
        void destroy();
        void push(Fixed2D4 change);
        void init(uint8_t type, int16_t x, int16_t y);
    };

    struct AsteroidManager {
        Asteroid asteroids[AsteroidsCount];
        void init();
        void tick();
        void draw();
        Asteroid* spawn();
        uint8_t countAll();
    };

    extern AsteroidManager asteroidManager;
    extern const uint8_t asteroidRadiusByType[];
    extern const uint16_t asteroidRadiusSqByType[];
    extern const uint8_t asteroidMaxHitsByType[];
}

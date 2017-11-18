#pragma once

#include "lib_FixedMath.h"
#include "lib_input.h"
#include "lib_RenderBuffer.h"

namespace Game {
    #define ProjectileMaxCount 32
    struct Projectile {
        Fixed2D4 pos;
        Fixed2D4 prevPos;
        Fixed2D4 velocity;
        uint8_t type;
        uint8_t age;
        void tick();
        void draw();
        void impact();
        void init(uint8_t t, Fixed2D4 xy, Fixed2D4 v);
    };

    struct ProjectileManager {
        Projectile projectiles[ProjectileMaxCount];
        void tick();
        void draw();
        void init();
        void spawn(uint8_t t, Fixed2D4 xy, Fixed2D4 v);
    };

    extern ProjectileManager projectileManager;
}

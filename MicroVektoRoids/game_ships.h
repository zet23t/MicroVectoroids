#pragma once
#include "lib_FixedMath.h"

namespace Game {
    #define ShipCount 24
    #define ShipTypePlayer ((uint8_t)1)
    #define ShipTypeStation ((uint8_t)2)
    #define ShipTypeEnemySmall ((uint8_t)3)
    #define ShipTypeWormHole ((uint8_t)4)
    #define ShipTypeWormHoleInactive ((uint8_t)5)
    #define ShipJumpStart 64
    #define ShipJumpEnd 128
    struct Ship {
        Fixed2D4 pos;
        Fixed2D4 prevPos;
        Fixed2D4 direction;
        Fixed2D4 velocity;
        uint8_t type;
        uint8_t damage;
        uint8_t shootCooldown;
        uint8_t takenHitCooldown;
        int16_t screenPos[2];
        const char* info;
        union {
            uint8_t destinationId;
            uint8_t charge;
        };

        void tickPlayer();
        void tickPlayerControls();
        void tickEnemySmallShip();
        void handleAsteroidsCollisions();
        void handleProjectileCollisions(uint8_t ptype);
        void tick();
        void draw();
        void init(int8_t type, int16_t x, int16_t y, const char *inf, uint8_t dest);
        void init(int8_t type, int16_t x, int16_t y, int8_t dx, int8_t dy, const char* info);
        void shoot();
        bool calcParallaxScreenPos();
        void takeDamage(uint8_t dmg);
        void explode();
        uint8_t maxDamage();
        void drawOnUI(int8_t x, int8_t y);
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

#pragma once

#include "lib_FixedMath.h"
namespace Game {
    #define ParticleCount 64

    namespace ParticleType {
        extern uint8_t PlayerShipTrail;
        extern uint8_t PlayerShot;
        extern uint8_t PlayerShotImpact;
        extern uint8_t AsteroidFragmentWhite;
        extern uint8_t EnemyShot;
        extern uint8_t EnemyShotImpact;
        extern uint8_t EnemyShipTrail;
    }

    struct Particle {
        Fixed2D4 pos,vel;
        uint8_t type, age;
        void tick();
        void draw();
        void init(uint8_t type, Fixed2D4 pos, Fixed2D4 vel);
    };
    struct ParticleSystem {
        Particle particles[ParticleCount];
        void tick();
        void draw();
        Particle* spawn(uint8_t type, Fixed2D4 pos, Fixed2D4 vel);
    };
    extern ParticleSystem particleSystem;
}

#pragma once

#include "lib_FixedMath.h"
namespace Game {
    #define ParticleCount 64

    namespace ParticleType {
        extern const uint8_t PlayerShipTrail;
        extern const uint8_t PlayerShot;
        extern const uint8_t PlayerShotImpact;
        extern const uint8_t AsteroidFragmentWhite;
        extern const uint8_t EnemyShot;
        extern const uint8_t EnemyShotImpact;
        extern const uint8_t EnemyShipTrail;
        extern const uint8_t PlayerShipExplosion;
        extern const uint8_t EnemyShipExplosion;
        extern const uint8_t WormHoleParticle;
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

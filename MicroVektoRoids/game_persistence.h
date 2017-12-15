#pragma once
#include "game_player_stats.h"
#include "game_ships.h"
#include "game_asteroids.h"
#include "game_particles.h"

namespace Game {
    namespace DB {
        struct GameState {
            ShipManager shipManager;
            AsteroidManager asteroidManager;
            ParticleSystem particleSystem;

        };
        struct Data {
            uint16_t formatVersion;
            char playerName[8];
            PlayerStats::HighScoreTable scores[32];
            GameState state[3];
        };

        void init();
        void save();
        void load();
    }

    namespace PlayerStats {
        void save(DB::Data* d);
    }
}

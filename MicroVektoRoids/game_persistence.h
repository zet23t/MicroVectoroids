#pragma once
#include "game_player_stats.h"
#include "game_ships.h"
#include "game_asteroids.h"
#include "game_particles.h"
#include "game_collectable.h"

namespace Game {
    namespace DB {
        struct GameState {
            char playerName[8];
            uint8_t isValid;
            uint8_t currentLevelId;
            uint16_t playerScore;

        };
        struct Data {
            uint16_t formatVersion;
            char playerName[8];
            PlayerStats::HighScoreTable scores[32];
            GameState state[3];
        };

        void init();
        void save(int8_t state);
        void load(int8_t state);

    }
}

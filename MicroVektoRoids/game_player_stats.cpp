#include "game_player_stats.h"

namespace Game {
    namespace PlayerStats {
        uint32_t score, visitedSectors;
        uint16_t jumps;

        void jumped(uint8_t from, uint8_t to) {
            jumps += 1;
            flagVisited(to);
        }
        uint16_t getJumpCount() {
            return jumps;
        }
        void awardScore(int points) {
            score += points;
        }
        void init() {
            score = 0;
            jumps = 0;
            visitedSectors = 0;
        }

        void flagVisited(uint8_t id) {
            visitedSectors |= 1<<id;
        }
        bool hasVisited(uint8_t id) {
            return (visitedSectors & 1<<id) != 0;
        }
    }

}

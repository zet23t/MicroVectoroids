#include "game_player_stats.h"

namespace Game {
    namespace PlayerStats {
        uint32_t score, visitedSectors;
        uint16_t jumps;
        char name[8];

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
            name[0] = 'N';
            name[1] = 'o';
            name[2] = 'b';
            name[3] = 'o';
            name[4] = 'd';
            name[5] = 'y';
            name[7] = 0;
        }

        void flagVisited(uint8_t id) {
            visitedSectors |= 1<<id;
        }
        bool hasVisited(uint8_t id) {
            return (visitedSectors & 1<<id) != 0;
        }
    }

}

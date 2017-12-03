#include "game_player_stats.h"

namespace Game {
    namespace PlayerStats {
        uint32_t score;
        uint16_t jumps;

        void jumped() {
            jumps += 1;
        }
        uint16_t getJumpCount() {
            return jumps;
        }
        void awardScore(int points) {
            score += points;
        }
        void init() {
            score = 0;
            jumps += 1;
        }
    }

}

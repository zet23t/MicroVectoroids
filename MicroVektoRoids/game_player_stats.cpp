#include "game_player_stats.h"

namespace Game {
    namespace PlayerStats {
        uint32_t score;


        void awardScore(int points) {
            score += points;
        }
        void init() {
            score = 0;
        }
    }

}

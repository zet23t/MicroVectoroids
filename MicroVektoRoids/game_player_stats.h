#pragma once
#include <inttypes.h>

namespace Game {
    namespace PlayerStats {
        extern uint32_t score;
        void init();
        void awardScore(int points);

        void jumped();
        uint16_t getJumpCount();
    }

}

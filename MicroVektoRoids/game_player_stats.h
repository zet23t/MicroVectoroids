#pragma once
#include <inttypes.h>

namespace Game {
    namespace PlayerStats {
        extern uint32_t score;
        void init();
        void awardScore(int points);

        void jumped(uint8_t from, uint8_t to);
        uint16_t getJumpCount();
        void flagVisited(uint8_t id);
        bool hasVisited(uint8_t id);
    }

}

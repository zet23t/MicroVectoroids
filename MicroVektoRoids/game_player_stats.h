#pragma once
#include <inttypes.h>

namespace Game {
    namespace PlayerStats {
        extern uint16_t score;
        extern char name[8];

        struct HighScoreEntry {
            char name[8];
            uint16_t score;
        };

        struct HighScoreTable {
            HighScoreEntry entries[3];
            void addEntry(const char name[8], uint16_t score);
        };

        extern HighScoreTable levelScores[32];

        void init();
        void awardScore(int points);

        void jumped(uint8_t from, uint8_t to);
        uint16_t getJumpCount();
        void flagVisited(uint8_t id);
        bool hasVisited(uint8_t id);

    }

}

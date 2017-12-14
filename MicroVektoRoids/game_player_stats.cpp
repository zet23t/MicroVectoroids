#include "game_common.h"
#include "game_player_stats.h"
#include <string.h>

namespace Game {
    namespace PlayerStats {
        uint16_t score, visitedSectors;
        uint16_t jumps;
        char name[8];

        void HighScoreTable::addEntry(const char name[8], uint16_t score) {
            HighScoreEntry entry;
            memcpy(entry.name, name, 8);
            entry.score = score;
            for (int i=0;i<3;i+=1) {
                if (entry.score > entries[i].score && entry.score > 0) {
                    HighScoreEntry tmp = entries[i];
                    entries[i] = entry;
                    entry = tmp;
                }
            }
        }

        HighScoreTable levelScores[32];

        void jumped(uint8_t from, uint8_t to) {
            jumps += 1;
            flagVisited(to);
            if (score > 0 && from < 32) {
                levelScores[from].addEntry(name, score);
                score = 0;
            }
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
            name[6] = 0;
            name[7] = 0;
            levelScores[DESTINATION_01].addEntry("KenBurns", 150);
            levelScores[DESTINATION_01].addEntry("Damien  ", 95);
            levelScores[DESTINATION_01].addEntry("Flaki   ", 90);
            levelScores[DESTINATION_02].addEntry("KenBurns", 210);
            levelScores[DESTINATION_02].addEntry("Damien  ", 150);
            levelScores[DESTINATION_02].addEntry("Flaki   ", 165);
        }

        void flagVisited(uint8_t id) {
            visitedSectors |= 1<<id;
        }
        bool hasVisited(uint8_t id) {
            return (visitedSectors & 1<<id) != 0;
        }
    }

}

#include "game_level_main.h"
#include "game_ships.h"
#include "game_common.h"
#include "game_player_stats.h"
#include "game_ui_intermission.h"

namespace Game {
    namespace Level {
        namespace Main {
            uint8_t textId;
            const char* texts[] = {
                "Here you are!",
                "There are a few sectors that need your attention.",
                "Feel free to clear them as you like.",
                0,

            };

            static void addWormhole(uint8_t &id, int16_t x, int16_t y, const char *inf, uint8_t dest, bool active) {
                shipManager.ships[id++].init(active ? ShipTypeWormHole : ShipTypeWormHoleInactive,x,y,inf, dest);
            }

            void init() {
                uint8_t id = 2;
                shipManager.ships[1].init(ShipTypeStation,15,8,15,0,0);
                addWormhole(id, -135,200,"W-03a", DESTINATION_MAIN, true);
                addWormhole(id, -225,230,"W-03b", DESTINATION_MAIN, false);
                addWormhole(id, -125,340,"W-03c", DESTINATION_MAIN, false);                if (PlayerStats::hasVisited(DESTINATION_MAIN)) {
                    textId = 3;
                } else {
                    textId = 0;
                }
            }
            void tick() {
                if (!UI::Intermission::isActive() && texts[textId]) {
                    UI::Intermission::show(texts[textId]);
                    textId += 1;
                }
            }
        }
    }
}


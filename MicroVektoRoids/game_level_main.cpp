#include "game_level_main.h"
#include "game_ships.h"
#include "game_asteroids.h"
#include "game_common.h"
#include "game_player_stats.h"
#include "game_ui_intermission.h"

static void addWormhole(uint8_t &id, int16_t x, int16_t y, const char *inf, uint8_t dest, bool active) {
    Game::shipManager.ships[id++].init(active ? ShipTypeWormHole : ShipTypeWormHoleInactive,x,y,inf, dest);
}

namespace Game {
    namespace Level {
        namespace L03 {
            void init () {
                for (int i=0;i<6;i+=1) {
                    int x = (Math::randInt() % 1024) - 512;
                    int y = (Math::randInt() % 1024) - 512;
                    if (x*x+y*y > 1000)
                        asteroidManager.spawn()->init(1,x,y);
                }
                for (int i=0;i<1;i+=1) {
                    int x = (Math::randInt() % 1024) - 512;
                    int y = (Math::randInt() % 1024) - 512;
                    if (x*x+y*y > 20)
                        shipManager.ships[i+2].init(3,x,y,15,0,0);
                }

                uint8_t id = 1;
                addWormhole(id, -15,20,"W-HOME", DESTINATION_MAIN, PlayerStats::hasVisited(DESTINATION_03));
            }
            void tick() {
            }
        }
        namespace Main {
            uint8_t textId;
            const char* texts[] = {
                "Here you are!",
                "There are a few sectors that need your attention.",
                "Feel free to clear them as you like.",
                0,

            };


            void init() {
                uint8_t id = 2;
                shipManager.ships[1].init(ShipTypeStation,15,8,15,0,0);
                addWormhole(id, -135,200,"W-03a", DESTINATION_03, true);
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


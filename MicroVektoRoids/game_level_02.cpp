#include "game_common.h"
#include "game_level_02.h"
#include "game_player_stats.h"
#include "game_ships.h"
#include "game_asteroids.h"
#include "game_ui_intermission.h"
#include "game_ui_hud.h"

namespace Game {
    namespace Level {
        namespace L02 {
            uint8_t textId;
            const char* texts[] = {
                "There are four enemies in this sector.",
                "The EDGE RADAR shows them as RED.",
                "If you need an overview, ..."
                "... open the SHIP MENU and look on the MAP.",
                0,
                "Well done, all enemies are destroyed.",
                "The wormhole home is active now.",
                0
            };
            void init() {
                if (!PlayerStats::hasVisited(DESTINATION_02)) {
                    textId = 0;
                }
                /*for (int i=0;i<3;i+=1) {
                    int x = (Math::randInt() % 1024) - 512;
                    int y = (Math::randInt() % 1024) - 512;
                    if (x*x+y*y > 20)
                        asteroidManager.spawn()->init(1,x,y);
                }*/
                Math::setSeed(12,4239);
                for (int i=0;i<4;i+=1) {
                    int x = (Math::randInt() % 1024) - 512;
                    int y = (Math::randInt() % 1024) - 512;
                    if (x*x+y*y > 1000)
                        shipManager.ships[i+2].init(3,x,y,15,0,0);
                }
                shipManager.ships[4].init(!PlayerStats::hasVisited(DESTINATION_02) ? ShipTypeWormHoleInactive : ShipTypeWormHole,-75,-40,15,0,"W-HOME");
            }
            void tick() {
                if (!UI::Intermission::isActive() && texts[textId]) {
                    UI::Intermission::show(texts[textId]);
                    textId += 1;
                }
                if (textId < 5 && shipManager.countAlive() == 0) {
                    textId = 5;
                }

            }
        }
    }
}

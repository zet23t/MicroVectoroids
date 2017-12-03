#include "game_common.h"
#include "game_level_01.h"
#include "game_player_stats.h"
#include "game_ships.h"
#include "game_asteroids.h"
#include "game_ui_intermission.h"
#include "game_ui_hud.h"

namespace Game {
    namespace Level {
        namespace L01 {
            uint8_t textId;
            const char* texts[] = {
                "Three asteroids are nearby.",
                "The EDGE RADAR shows them as gray.",
                0, //2
                "Big asteroids break up. Destroy the small ones.",
                0, //4
                "Small asteroids become dust. Collect it.",
                0, //6
                "Dust is precious. Collect as much as you can.",
                0, // 8
                "Well done, you've destroyed all asteroids.",
                "The nearby wormhole is active now.",
                "Return home.",
                0
            };
            uint8_t damageWarned, damageId;
            const char* dmgTexts[]= {
                0,
                "Don't bump into ASTEROIDS!",
                "Each time your SHIELD gets damaged.",
                "If it's too low, you get destroyed.",
                0,
                "Bumping ASTEROIDS is only damaging YOU.",
                "Besides they start moving and it gets HARDER.",
                0,
                0
            };
            void init() {
                if (PlayerStats::getJumpCount() == 2) {
                    textId = 0;
                    damageWarned = 0;
                    damageId = 0;
                }
                for (int i=0;i<3;i+=1) {
                    int x = (Math::randInt() % 1024) - 512;
                    int y = (Math::randInt() % 1024) - 512;
                    if (x*x+y*y > 20)
                        asteroidManager.spawn()->init(1,x,y);
                }
                /*for (int i=0;i<1;i+=1) {
                    int x = (Math::randInt() % 1024) - 512;
                    int y = (Math::randInt() % 1024) - 512;
                    if (x*x+y*y > 20)
                        shipManager.ships[i+2].init(3,x,y,15,0,0);
                }*/
                shipManager.ships[4].init(ShipTypeWormHoleInactive,-75,-40,15,0,"W0:MAIN");
            }
            void tick() {
                if (!dmgTexts[damageId] && dmgTexts[damageId+1] && shipManager.ships[0].damage != damageWarned) {
                    damageWarned = shipManager.ships[0].damage;
                    damageId +=1;
                }
                if (textId == 2 && asteroidManager.countAll() == 2+5) {
                    textId += 1;
                }
                if (textId == 4 && asteroidManager.countAll() == 2+5-1) {
                    textId += 1;
                }
                if (textId == 6 && PlayerStats::score > 0) {
                    textId += 1;
                }
                if (textId == 8 && asteroidManager.countAll() == 0) {
                    textId += 1;
                }

                if (!UI::Intermission::isActive() && texts[textId]) {
                    UI::Intermission::show(texts[textId]);
                    textId += 1;
                }
                if (!UI::Intermission::isActive() && dmgTexts[damageId]) {
                    UI::Intermission::show(dmgTexts[damageId]);
                    damageId += 1;
                }
            }
        }
    }
}

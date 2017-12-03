#include "game_common.h"
#include "game_level_main.h"
#include "game_player_stats.h"
#include "game_ships.h"
#include "game_asteroids.h"
#include "game_ui_intermission.h"
#include "game_ui_hud.h"

namespace Game {
    namespace Level {
        namespace Main {
            uint8_t textId;
            const char* texts[] = {
                "Welcome, Triangle.",
                "Use the EDGE RADAR - fly to the green marker",
                0,
                "Activate this wormhole.",
                0,
            };
            void init() {
                shipManager.ships[1].init(ShipTypeStation,15,8,15,0,0);
                shipManager.ships[2].init(ShipTypeWormHole,-135,200,15,0,"W1:EASY");
                shipManager.ships[3].init(ShipTypeWormHoleInactive,-235,-100,15,0,"W2:NORMAL");
                shipManager.ships[2].destinationId = 1;
                if (PlayerStats::getJumpCount() == 1) {
                    textId = 0;
                }
            }
            void tick() {
                if (textId == 2 && UI::HUD::targetLock && UI::HUD::targetIsShip && UI::HUD::targetIndex == 2) {
                    textId += 1;
                }
                if (!UI::Intermission::isActive() && texts[textId]) {
                    UI::Intermission::show(texts[textId]);
                    textId += 1;
                }
            }
        }
    }
}
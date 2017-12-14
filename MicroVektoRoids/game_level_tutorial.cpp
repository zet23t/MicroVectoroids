#include "game_common.h"
#include "game_level_tutorial.h"
#include "game_player_stats.h"
#include "game_ships.h"
#include "game_asteroids.h"
#include "game_ui_intermission.h"
#include "game_ui_hud.h"

namespace Game {
    namespace Level {
        namespace Tutorial {
            uint8_t textId;
            const char* texts[] = {
                "Welcome - Let's get started.",
                "Use the EDGE RADAR - fly to the green marker",
                0,
                "Activate this WORMHOLE.",
                0,
                "Welcome back. ",
                "There's a new active WORMHOLE.",
                0,
                "Well done this finishes your training.",
                "Greater challenges await you in the next sector.",
                0
            };
            void init() {
                shipManager.ships[1].init(ShipTypeStation,15,8,15,0,0);
                shipManager.ships[2].init(ShipTypeWormHole,-135,200,15,0,"W-01");
                shipManager.ships[3].init(PlayerStats::hasVisited(DESTINATION_01) ? ShipTypeWormHole : ShipTypeWormHoleInactive,-235,-100,15,0,"W-02");
                shipManager.ships[4].init(PlayerStats::hasVisited(DESTINATION_02) ? ShipTypeWormHole : ShipTypeWormHoleInactive,135,-180,15,0,"W-MAIN");
                shipManager.ships[2].destinationId = DESTINATION_01;                shipManager.ships[3].destinationId = DESTINATION_02;
                if (!PlayerStats::hasVisited(DESTINATION_01)) {
                    textId = 0;
                } else {
                    textId = PlayerStats::hasVisited(DESTINATION_02) ? 8 : 5;
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

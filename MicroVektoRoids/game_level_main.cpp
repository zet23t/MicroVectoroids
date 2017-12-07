#include "game_level_main.h"
#include "game_ships.h"
#include "game_common.h"

namespace Game {
    namespace Level {
        namespace Main {
            void init() {
                shipManager.ships[1].init(ShipTypeStation,15,8,15,0,0);
                shipManager.ships[2].init(ShipTypeWormHole,-135,200,15,0,"W-03");
                shipManager.ships[2].destinationId = DESTINATION_MAIN;
            }
            void tick() {
            }
        }
    }
}


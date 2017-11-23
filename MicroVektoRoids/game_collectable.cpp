#include "game_collectable.h"
#include "game_common.h"
#include "game_player_stats.h"
#include "game_ships.h"
#include "game_balancing.h"

namespace Game {
    namespace Collectable {
        struct CollectableObject {
            Fixed2D4 position;
            Fixed2D4 velocity;
            uint8_t age;
            bool active;
        };

        #define CollectableCount 48

        uint8_t collectableId;
        CollectableObject collectables[CollectableCount];

        void spawn(Fixed2D4 pos, Fixed2D4 vel) {
            collectables[collectableId].active = true;
            collectables[collectableId].position = pos;
            collectables[collectableId].velocity = vel;
            collectables[collectableId].age = 0;
            collectableId = (collectableId + 1) % CollectableCount;
        }
        void tick() {
            Ship* ship = shipManager.ships;
            if (ship->type == 0) return;
            Fixed2D4 sp = ship->pos;
            for (int i=0;i<CollectableCount;i+=1) {
                if (!collectables[i].active) continue;
                collectables[i].position += collectables[i].velocity;
                collectables[i].velocity = collectables[i].velocity  * COLLECTABLE_DRAG;
                if (collectables[i].age < COLLECTABLE_SPAWN_TIME) {
                    collectables[i].age+=1;
                    continue;
                }
                Fixed2D4 pos = collectables[i].position;
                int dx = sp.x.getIntegerPart() - pos.x.getIntegerPart();
                int dy = sp.y.getIntegerPart() - pos.y.getIntegerPart();
                if (abs(dx) < PLAYER_SHIP_DUST_COLLECTION_RANGE && abs(dy) < PLAYER_SHIP_DUST_COLLECTION_RANGE) {
                    collectables[i].active = false;
                    PlayerStats::awardScore(5);
                    continue;
                }
                if (abs(dx) < PLAYER_SHIP_DUST_ATTRACTION_RANGE && abs(dy) < PLAYER_SHIP_DUST_ATTRACTION_RANGE) {
                    Fixed2D4 dir = sp - pos;
                    Fix4 len = dir.length();
                    if (len < Fix4(PLAYER_SHIP_DUST_ATTRACTION_RANGE,0)) {
                        collectables[i].velocity += dir * (PLAYER_SHIP_DUST_ATTRACTION_POWER / len);
                    }
                }
            }
        }
        void draw() {
            for (int i=0;i<CollectableCount;i+=1) {
                if (!collectables[i].active) continue;
                Fixed2D4 pos = collectables[i].position;
                int display = (i + frame) % 4;
                drawCenteredSprite(pos.x.getIntegerPart(),pos.y.getIntegerPart(),ImageAsset::TextureAtlas_atlas::collectable_score.sprites[display])->blend(RenderCommandBlendMode::add);
            }
        }
        void init() {
            memset(collectables, 0, sizeof(collectables));
            collectableId = 0;
        }
    }

}

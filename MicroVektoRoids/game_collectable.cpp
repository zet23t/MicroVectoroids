#include "game_collectable.h"
#include "game_common.h"
#include "game_player_stats.h"
#include "game_ships.h"

namespace Game {
    namespace Collectable {
        struct CollectableObject {
            Fixed2D4 position;
            Fixed2D4 velocity;
            bool active;
        };

        #define CollectableCount 48

        uint8_t collectableId;
        CollectableObject collectables[CollectableCount];

        void spawn(Fixed2D4 pos, Fixed2D4 vel) {
            collectables[collectableId].active = true;
            collectables[collectableId].position = pos;
            collectables[collectableId].velocity = vel;
            collectableId = (collectableId + 1) % CollectableCount;
        }
        void tick() {
            Ship* ship = shipManager.ships;
            if (ship->type == 0) return;
            Fixed2D4 sp = ship->pos;
            for (int i=0;i<CollectableCount;i+=1) {
               if (!collectables[i].active) continue;
               Fixed2D4 pos = collectables[i].position;
               int dx = sp.x.getIntegerPart() - pos.x.getIntegerPart();
               int dy = sp.y.getIntegerPart() - pos.y.getIntegerPart();
               if (abs(dx) < 3 && abs(dy) < 3) {
                    collectables[i].active = false;
                    PlayerStats::awardScore(1);
                    continue;
               }
               if (abs(dx) < 22 && abs(dy) < 22) {
                    Fixed2D4 dir = sp - pos;
                    Fix4 len = dir.length();
                    if (len < Fix4(22,0)) {
                        collectables[i].velocity += dir * (Fix4(2,0) / len);
                    }
               }
               collectables[i].position += collectables[i].velocity;
               collectables[i].velocity = collectables[i].velocity  * Fix4(0,12);
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

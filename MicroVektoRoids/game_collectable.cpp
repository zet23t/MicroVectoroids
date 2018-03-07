#include "game_collectable.h"
#include "game_common.h"
#include "game_player_stats.h"
#include "game_ships.h"
#include "game_balancing.h"
#include "lib_sound.h"

namespace Game {
    namespace Collectable {
        struct CollectableObject {
            Fixed2D4 position;
            Fixed2D4 velocity;
            uint8_t age;
            bool active;
        };


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
                if (collectables[i].age < COLLECTABLE_SPAWN_TIME || !ship->type) {
                    collectables[i].age+=1;

                    continue;
                }
                Fixed2D4 pos = collectables[i].position;
                int dx = sp.x.getIntegerPart() - pos.x.getIntegerPart();
                int dy = sp.y.getIntegerPart() - pos.y.getIntegerPart();
                if (abs(dx) < PLAYER_SHIP_DUST_COLLECTION_RANGE && abs(dy) < PLAYER_SHIP_DUST_COLLECTION_RANGE) {
                    collectables[i].active = false;
                    PlayerStats::awardScore(5);
                    static const int8_t b[] = {100,-100,80,-80,60,-60,80,-80,100,-100};
                    static uint8_t k = 110;
                    Sound::playSample(k++,b, sizeof(b), 0x3d,0x100,100)->setChange(0x600,-3,0);
                    Sound::playSample(k++,b, sizeof(b), 0x60,0x80,100)->setChange(0x300,-1,0)->interpolate = 1;
                    if (k > 115) k = 110;
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

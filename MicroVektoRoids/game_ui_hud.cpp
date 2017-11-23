#include "game_common.h"
#include "game_asteroids.h"
#include "game_ui_hud.h"
#include "game_ships.h"


namespace Game {
    namespace UI {
        namespace HUD {


            bool targetLock;
            Fixed2D4 targetPosition;
            Fixed2D4 targetVelocity;

            Fixed2D4 highlightPosition;
            uint8_t highlightRadius;


            void highlightTarget(Fixed2D4 pos, uint8_t sz) {
                int x = pos.x.getIntegerPart();
                int y = pos.y.getIntegerPart();
                drawCenteredSprite(x - sz, y + sz, ImageAsset::TextureAtlas_atlas::ui_target_marker.sprites[0])->blend(RenderCommandBlendMode::add)->setDepth(100);
                drawCenteredSprite(x + sz, y + sz, ImageAsset::TextureAtlas_atlas::ui_target_marker.sprites[1])->blend(RenderCommandBlendMode::add)->setDepth(100);
                drawCenteredSprite(x + sz, y - sz, ImageAsset::TextureAtlas_atlas::ui_target_marker.sprites[2])->blend(RenderCommandBlendMode::add)->setDepth(100);
                drawCenteredSprite(x - sz, y - sz, ImageAsset::TextureAtlas_atlas::ui_target_marker.sprites[3])->blend(RenderCommandBlendMode::add)->setDepth(100);
            }
            void updateTargetHighlight(bool hasTarget, Fixed2D4 target, uint8_t rad) {
                if (hasTarget) {
                    int dx = target.x.getIntegerPart() - highlightPosition.x.getIntegerPart();
                    int dy = target.y.getIntegerPart() - highlightPosition.y.getIntegerPart();
                    if (abs(dx) < 50 && abs(dy) < 50) {
                        highlightPosition = highlightPosition * Fix4(0,6) + target * Fix4(0,10);
                        highlightRadius = highlightRadius / 2 + rad / 2;
                        if (rad > highlightRadius) highlightRadius += 1;
                        if (rad < highlightRadius) highlightRadius -=1;
                    }
                    else {
                        highlightPosition = target;
                        highlightRadius = rad;
                    }
                } else {
                    if (highlightRadius > 0) highlightRadius -=1;
                }
                if (highlightRadius > 0) highlightTarget(highlightPosition, highlightRadius);
            }


            void draw() {
                Ship* ship = &shipManager.ships[0];
                if (ship->type != 1) return;
                Fixed2D4 dir = ship->direction;
                int16_t sx = ship->pos.x.getIntegerPart();
                int16_t sy = ship->pos.y.getIntegerPart();
                Fixed2D4 bestTarget;
                uint8_t targetRad = 0;
                Fix4 bestDot = Fix4(-1,0);
                Fixed2D4 bestVelocity;
                for (int i=0;i<AsteroidsCount;i+=1) {
                    Asteroid *a = &asteroidManager.asteroids[i];
                    if (!a->type) continue;
                    int16_t ax = a->pos.x.getIntegerPart();
                    int16_t ay = a->pos.y.getIntegerPart();
                    int16_t dx = ax - sx;
                    int16_t dy = ay - sy;
                    if (abs(dx) > 50 ||abs(dy) > 50) continue;
                    Fixed2D4 targetDir = a->pos - ship->pos;
                    targetDir = targetDir.normalize();
                    Fix4 dot = targetDir.dot(dir);
                    if (dot > bestDot) {
                        bestDot = dot;
                        bestTarget = a->pos;
                        bestVelocity = a->velocity;
                        targetRad = asteroidRadiusByType[a->type] / 2 + 2;
                    }
                }
                Fixed2D4 bestTargetEnemy;
                uint8_t targetRadEnemy = 0;
                Fix4 bestDotEnemy = Fix4(-1,0);
                Fixed2D4 bestVelocityEnemy;
                for (int i=1;i<ShipCount;i+=1) {
                    Ship *s = &shipManager.ships[i];
                    if (s->type != 3) continue;
                    int16_t ax = s->pos.x.getIntegerPart();
                    int16_t ay = s->pos.y.getIntegerPart();
                    int16_t dx = ax - sx;
                    int16_t dy = ay - sy;
                    if (abs(dx) > 50 ||abs(dy) > 50) continue;
                    Fixed2D4 targetDir = s->pos - ship->pos;
                    targetDir = targetDir.normalize();
                    Fix4 dot = targetDir.dot(dir);
                    if (dot > bestDotEnemy) {
                        bestDotEnemy = dot;
                        bestTargetEnemy = s->pos;
                        targetRadEnemy = 6;
                        bestVelocityEnemy = s->velocity;
                    }
                }
                int sel = bestDotEnemy > Fix4(0,10) ? 1 : (bestDot > Fix4(0,12) ? 2 : 0);
                if (sel) {
                    targetLock = true;
                    targetPosition = sel == 1 ? bestTargetEnemy : bestTarget;
                    targetVelocity=sel == 1? bestVelocityEnemy : bestVelocity;
                    updateTargetHighlight(true, targetPosition, sel== 1 ?targetRadEnemy : targetRad);
                    //highlightTarget(bestTarget,targetRad);
                } else {
                    targetLock = false;
                    updateTargetHighlight(false, Fixed2D4(0,0),0);
                }
            }
        }
    }
}
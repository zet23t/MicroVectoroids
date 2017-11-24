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
            uint8_t targetDamage;
            uint8_t targetMaxDamage;
            Fixed2D4 highlightPosition;
            uint8_t highlightRadius;
            char* highlightInfo;


            void highlightTarget(Fixed2D4 pos, uint8_t sz) {
                int x = pos.x.getIntegerPart();
                int y = pos.y.getIntegerPart();
                drawCenteredSprite(x - sz, y + sz, ImageAsset::TextureAtlas_atlas::ui_target_marker.sprites[0])->blend(RenderCommandBlendMode::add)->setDepth(100);
                drawCenteredSprite(x + sz, y + sz, ImageAsset::TextureAtlas_atlas::ui_target_marker.sprites[1])->blend(RenderCommandBlendMode::add)->setDepth(100);
                drawCenteredSprite(x + sz, y - sz, ImageAsset::TextureAtlas_atlas::ui_target_marker.sprites[2])->blend(RenderCommandBlendMode::add)->setDepth(100);
                drawCenteredSprite(x - sz, y - sz, ImageAsset::TextureAtlas_atlas::ui_target_marker.sprites[3])->blend(RenderCommandBlendMode::add)->setDepth(100);
                if (highlightInfo) {
                    int x = buffer.getOffsetX();
                    int y = buffer.getOffsetY();
                    buffer.setOffset(0,0);
                    buffer.drawText(highlightInfo, 0,58,96,6,0,0,false,FontAsset::font,120, RenderCommandBlendMode::opaque);
                    buffer.setOffset(x,y);
                }
            }

            void updateTargetHighlight(bool hasTarget, Fixed2D4 target, int16_t* pos, uint8_t rad, char* info) {
                if (hasTarget) {
                    if (pos) {
                        target.x.setIntegerPart(pos[0]);
                        target.y.setIntegerPart(pos[1]);
                    }
                    int dx = target.x.getIntegerPart() - highlightPosition.x.getIntegerPart();
                    int dy = target.y.getIntegerPart() - highlightPosition.y.getIntegerPart();
                    if (abs(dx) < 50 && abs(dy) < 50) {
                        highlightPosition = highlightPosition * Fix4(0,6) + target * Fix4(0,10);
                        highlightRadius = highlightRadius / 2 + rad / 2;
                        highlightInfo = info;
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

            struct BestInfo {
                Fixed2D4 pos;
                int16_t* screenPos;
                Fixed2D4 vel;
                Fix4 dot;
                uint8_t rad;
                uint8_t dmg;
                uint8_t maxDmg;
                char *info;

                void init(Fixed2D4 p, Fixed2D4 v, Fix4 d, uint8_t r, uint8_t dm, uint8_t maxd, int16_t *screen, char *inf) {
                    if (dot > d) return;
                    pos = p;
                    vel = v;
                    dot = d;
                    rad = r;
                    dmg = dm;
                    maxDmg = maxd;
                    screenPos = screen;
                    info = inf;
                }
            };


            void draw() {
                Ship* ship = &shipManager.ships[0];
                if (ship->type != 1) return;
                Fixed2D4 dir = ship->direction;
                int16_t sx = ship->pos.x.getIntegerPart();
                int16_t sy = ship->pos.y.getIntegerPart();
                BestInfo asteroidInfo;
                asteroidInfo.dot = Fix4(-1,0);

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
                    asteroidInfo.init(a->pos, a->velocity, dot, asteroidRadiusByType[a->type] /2 + 2, a->hits,
                                      asteroidMaxHitsByType[a->type], 0,0);
                }
                BestInfo shipInfo;
                shipInfo.dot = Fix4(-1,0);
                for (int i=1;i<ShipCount;i+=1) {
                    Ship *s = &shipManager.ships[i];
                    if (s->type < 3) continue;
                    int16_t ax = s->pos.x.getIntegerPart();
                    int16_t ay = s->pos.y.getIntegerPart();
                    int16_t dx = ax - sx;
                    int16_t dy = ay - sy;
                    if (abs(dx) > 50 ||abs(dy) > 50) continue;
                    Fixed2D4 targetDir = s->pos - ship->pos;
                    targetDir = targetDir.normalize();
                    Fix4 dot = targetDir.dot(dir);
                    shipInfo.init(s->pos, s->velocity, dot, s->type == ShipTypeWormHole ? 12 : 6, s->damage, s->maxDamage(), s->screenPos,s->info);
                }
                int sel = shipInfo.dot > Fix4(0,10) ? 1 : (asteroidInfo.dot > Fix4(0,12) ? 2 : 0);
                if (sel) {
                    BestInfo info = sel == 1 ? shipInfo : asteroidInfo;
                    targetLock = true;
                    targetPosition = info.pos;
                    targetVelocity = info.vel;
                    targetMaxDamage = info.maxDmg;
                    targetDamage = info.dmg;
                    updateTargetHighlight(true, targetPosition, info.screenPos, info.rad, info.info);
                    //highlightTarget(bestTarget,targetRad);
                } else {
                    targetLock = false;
                    updateTargetHighlight(false, Fixed2D4(0,0), 0,0, 0);
                }
            }
        }
    }
}

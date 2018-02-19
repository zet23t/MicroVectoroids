#include "game_common.h"
#include "game_ui_radar.h"
#include "game_asteroids.h"
#include "game_ships.h"

namespace Game {
    namespace UI {
        namespace Radar {
            const uint16_t blipShipColors[] = {
                0xffff,0xffff,
                RGB565(80,130,250), //station
                RGB565(255,0,0), // enemy
                RGB565(80,255,0), // wormhole
                RGB565(255,255,0), // wormhole inakt
            };

            #ifndef max
            int max(const int a, const int b) {
                return a < b ? b : a;
            }
            #endif // max

            void drawRadarBlip(Fixed2D4 cen, Fixed2D4 pos, uint16_t col) {
                //printf("%d %x\n",pos.x.getIntegerPart(),col);
                Fixed2D4 dir = pos - cen;
                int dx = dir.x.getIntegerPart();
                int dy = dir.y.getIntegerPart();
                if (abs(dx) < 48 && abs(dy) < 32) return;

                int sdx = dx * 32;
                int sdy = dy * 48;
                int dist = max(abs(dx),abs(dy));
                int w = 2;
                if (dist < 120 && (frame / 4 & 1)) {
                    w = 4;
                }
                int barlen = max(1,8-dist/32) + (frame >> 3 & 1)*2 + 1;

                if (abs(sdx) >= abs(sdy)) {
                    sdy /= abs(sdx) / 32;
                    buffer.drawRect(sdx > 0 ? 96-w : 0,sdy+32 - barlen/2,w,barlen)->filledRect(col)->setDepth(100);
                } else {
                    sdx /= abs(sdy) / 48;
                    buffer.drawRect(48+sdx-barlen/2,sdy > 0 ? 64-w : 0,barlen,w)->filledRect(col)->setDepth(100);
                }
                    //printf("%d %d %d \n",dx ,dy,sdy);

            }
            struct Blip{
                Fixed2D4 pos;
                uint32_t dist;
                uint16_t color;

                void init(Fixed2D4 p, uint32_t d, uint16_t c) {
                    pos= p;
                    dist = d;
                    color = c;
                }
            };
            #define BlipCount 5
            void addBlip(Blip* blips, Fixed2D4 cen, Fixed2D4 p, uint16_t c) {
                int dx = cen.x.getIntegerPart() - p.x.getIntegerPart();
                int dy = cen.y.getIntegerPart() - p.y.getIntegerPart();
                int d = dx*dx + dy*dy;
                //printf("%d\n",d);
                for(int i=0;i<BlipCount;i+=1) {
                    if (blips[i].dist == 0) {
                        //printf("%d %d\n",i,d);
                        blips[i].init(p,d,c);
                        return;
                    }
                }
                for(int i=0;i<BlipCount;i+=1) {
                    if (blips[i].dist > d) {
                        //printf("%d %d\n",i,d);
                        blips[i].init(p,d,c);
                        return;
                    }
                }
            }

            void draw() {
                int16_t x = buffer.getOffsetX();
                int16_t y = buffer.getOffsetY();
                Blip blipsAsteroids[BlipCount];
                Blip blipsShips[BlipCount];
                memset(blipsAsteroids,0,sizeof(blipsAsteroids));
                memset(blipsShips,0,sizeof(blipsShips));

                buffer.setOffset(0,0);
                buffer.drawRect(0,0,96,1)->filledRect(RGB565(0,0,0))->setDepth(100)->blend(RenderCommandBlendMode::average);
                buffer.drawRect(0,63,96,1)->filledRect(RGB565(0,0,0))->setDepth(100)->blend(RenderCommandBlendMode::average);
                buffer.drawRect(0,0,1,64)->filledRect(RGB565(0,0,0))->setDepth(100)->blend(RenderCommandBlendMode::average);
                buffer.drawRect(95,0,1,64)->filledRect(RGB565(0,0,0))->setDepth(100)->blend(RenderCommandBlendMode::average);
                Fixed2D4 cen = Fixed2D4(x+48,y+32);
                for (int i=0;i<AsteroidsCount;i+=1) {
                    uint8_t t = asteroidManager.asteroids[i].type;
                    if (t) {
                        addBlip(blipsAsteroids, cen, asteroidManager.asteroids[i].pos,
                                t == AsteroidType::WhiteSmall ?
                                RGB565(90,90,90) :
                                RGB565(140,140,140));
                    }
                }
                for (int i=0;i<ShipCount;i+=1) {
                    if (shipManager.ships[i].type >= 2) {
                        addBlip(blipsShips, cen, shipManager.ships[i].pos, blipShipColors[shipManager.ships[i].type]);
                    }
                }
                for (int i=0;i<BlipCount;i+=1) {
                    if (blipsAsteroids[i].dist > 0) {
                        drawRadarBlip(cen, blipsAsteroids[i].pos, blipsAsteroids[i].color);
                    }
                    if (blipsShips[i].dist > 0) {
                        drawRadarBlip(cen, blipsShips[i].pos, blipsShips[i].color);
                    }
                }
                buffer.setOffset(x,y);
            }
        }
    }
}

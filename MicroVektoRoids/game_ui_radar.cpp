#include "game_common.h"
#include "game_ui_radar.h"
#include "game_asteroids.h"
#include "game_ships.h"

namespace Game {
    namespace UI {
        namespace Radar {

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
                int w = 1;
                if (dist < 120 && (frame / 4 & 1)) {
                    w = 2;
                }
                int barlen = max(1,8-dist/32);

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
            #define BlipCount 8
            void addBlip(Blip* blips, Fixed2D4 cen, Fixed2D4 p, uint16_t c) {
                int16_t dx = cen.x.getIntegerPart() - p.x.getIntegerPart();
                int16_t dy = cen.y.getIntegerPart() - p.y.getIntegerPart();
                uint32_t d = dx*dx + dy*dy;
                for(int i=0;i<BlipCount;i+=1) {
                    if (blips[i].dist == 0 || blips[i].dist > d) {
                        //printf("%d %d\n",i,d);
                        blips[i].init(p,d,c);
                        break;
                    }
                }
            }

            void draw() {
                int16_t x = buffer.getOffsetX();
                int16_t y = buffer.getOffsetY();
                Blip blips[BlipCount];
                memset(blips,0,sizeof(blips));

                buffer.setOffset(0,0);
                Fixed2D4 cen = Fixed2D4(x+48,y+32);
                for (int i=0;i<AsteroidsCount;i+=1) {
                    if (asteroidManager.asteroids[i].type) {
                        addBlip(blips, cen, asteroidManager.asteroids[i].pos, RGB565(255,0,0));
                    }
                }
                for (int i=0;i<ShipCount;i+=1) {
                    if (shipManager.ships[i].type == 2) {
                        addBlip(blips, cen, shipManager.ships[i].pos, RGB565(50,80,250));
                    }
                }
                for (int i=0;i<BlipCount;i+=1) {
                    if (blips[i].dist > 0) {

                        drawRadarBlip(cen, blips[i].pos, blips[i].color);
                    }
                }
                buffer.setOffset(x,y);
            }
        }
    }
}

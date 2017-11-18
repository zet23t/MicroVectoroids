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

            void draw() {
                int16_t x = buffer.getOffsetX();
                int16_t y = buffer.getOffsetY();
                buffer.setOffset(0,0);
                Fixed2D4 cen = Fixed2D4(x+48,y+32);
                for (int i=0;i<AsteroidsCount;i+=1) {
                    if (asteroidManager.asteroids[i].type) {
                        drawRadarBlip(cen, asteroidManager.asteroids[i].pos, RGB565(255,0,0));
                    }
                }
                for (int i=0;i<ShipCount;i+=1) {
                    if (shipManager.ships[i].type == 2) {
                        drawRadarBlip(cen, shipManager.ships[i].pos, RGB565(50,80,250));
                    }
                }
                buffer.setOffset(x,y);
            }
        }
    }
}

#include "game_common.h"
#include "game_asteroids.h"
#include "game_particles.h"
#include "game_ships.h"

TinyScreen display = TinyScreen(TinyScreenPlus);
RenderBuffer<uint16_t,RENDER_COMMAND_COUNT> buffer;
TileMap::SceneRenderer<uint16_t,RENDER_COMMAND_COUNT> renderer;
TileMap::Scene<uint16_t> tilemap;


namespace Time {
    uint32_t millis;
}

namespace Game {
    int frame;
    Texture<uint16_t> atlas;
    ParticleSystem particleSystem;
    ShipManager shipManager;
    AsteroidManager asteroidManager;

    void drawSpaceBackground(int layer, uint16_t col) {
        int16_t x = buffer.getOffsetX();
        int16_t y = buffer.getOffsetY();
        int16_t mask= (64 << layer)-1;
        int16_t ox = x & ~mask;
        int16_t oy = y & ~mask;
        buffer.setOffset(x>>layer,y>>layer);
        for (int8_t i=0; i<3;i+=1) {
            for (int8_t j=0;j<2;j+=1) {
                int16_t xx = ox + (i << (6 + layer));
                int16_t yy = oy + (j << (6 + layer));
                Math::setSeed(xx + layer * 17,yy + layer * 7);
                for (int8_t k = 0;k<8;k+=1) {
                    int16_t px = xx + (Math::randInt()&mask);
                    int16_t py = yy + (Math::randInt()&mask);
                    buffer.drawRect(px>>layer,py>>layer,1,1)->filledRect(col);
                }
            }
        }
        buffer.setOffset(x,y);
        //buffer.drawRect(ox,oy,64,64)->filledRect(RGB565(0,255,0));
    }

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

    void drawRadar() {
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

    void tick() {
        frame += 1;
        Ship* ship = shipManager.ships;
        Fixed2D4 input =  Joystick::getJoystick();
        if (input.x !=0 || input.y !=0) {
            input.normalize();
            if (input.dot(ship->direction) > Fix4(0,12)) {
                ship->velocity += ship->direction * Fix4(0,12);
            }

            Fix4 force = Fix4(0,7);
            Fix4 dot = ship->direction.dot(input);
            if (dot < Fix4(-1,2)) {
                Fixed2D4 right = input.right();
                Fixed2D4 left = input.left();
                Fix4 leftDot = left.dot(ship->direction);
                Fix4 rightDot = right.dot(ship->direction);
                input = leftDot > rightDot ? left : right;
            }
            //force = Fix4(3,0);
            ship->direction += input * force;
            ship->direction.normalize();
        }
        shipManager.tick();
        particleSystem.tick();
        asteroidManager.tick();
        Fixed2D4 cam = ship->pos + ship->direction * 6;
        buffer.setOffset(cam.x.getIntegerPart() - 48, cam.y.getIntegerPart() -32);

        drawSpaceBackground(0, RGB565(62,62,62));
        drawSpaceBackground(1, RGB565(180,180,180));
        drawSpaceBackground(2, RGB565(120,120,120));
        drawRadar();
        shipManager.draw();
        particleSystem.draw();
        asteroidManager.draw();
    }

    void initialize(){
        frame = 0;
        atlas = Texture<uint16_t>(ImageAsset::atlas);
        const Ship* ship = shipManager.ships;
        shipManager.ships[0].init(1,10,5,15,0);
        shipManager.ships[1].init(2,15,8,15,0);
        buffer.setClearBackground(true, RGB565(0,0,0));
        asteroidManager.init();
        asteroidManager.spawn()->init(1,30,15);
    }
}

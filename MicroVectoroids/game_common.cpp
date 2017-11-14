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

    void tick() {
        frame += 1;
        Ship* ship = shipManager.ships;
        Fixed2D4 input =  Joystick::getJoystick();
        if (input.x !=0 || input.y !=0) {
            Fix4 force = Fix4(0,8);
            Fix4 dot = ship->direction.dot(input);
            if (dot < 0) {
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
        if (Joystick::getButton(0)) {
            ship->velocity += ship->direction * Fix4(0,12);
        }
        shipManager.tick();
        particleSystem.tick();
        asteroidManager.tick();
        Fixed2D4 cam = ship->pos + ship->direction * 6;
        buffer.setOffset(cam.x.getIntegerPart() - 48, cam.y.getIntegerPart() -32);

        drawSpaceBackground(0, RGB565(62,62,62));
        drawSpaceBackground(1, RGB565(180,180,180));
        drawSpaceBackground(2, RGB565(120,120,120));
        shipManager.draw();
        particleSystem.draw();
        asteroidManager.draw();
    }

    void initialize(){
        frame = 0;
        atlas = Texture<uint16_t>(ImageAsset::atlas);
        const Ship* ship = shipManager.ships;
        shipManager.ships[0].init(1,10,5,15,0);
        shipManager.ships[1].init(1,15,8,15,0);
        buffer.setClearBackground(true, RGB565(0,0,0));
        asteroidManager.init();
        asteroidManager.spawn()->init(1,30,15);
    }
}

#include "game_common.h"
#include "game_asteroids.h"
#include "game_particles.h"
#include "game_projectile.h"
#include "game_ships.h"
#include "game_menu.h"
#include "game_ui_radar.h"
#include "game_ui_shield.h"

TinyScreen display = TinyScreen(TinyScreenPlus);
RenderBuffer<uint16_t,RENDER_COMMAND_COUNT> buffer;
TileMap::SceneRenderer<uint16_t,RENDER_COMMAND_COUNT> renderer;
TileMap::Scene<uint16_t> tilemap;


namespace Time {
    uint32_t millis;
}

namespace Game {
    namespace GameState {
        const uint8_t Running = 0;
        const uint8_t Menu = 1;
    }
    uint8_t gameState;
    int frame, frameUnpaused;
    Texture<uint16_t> atlas;
    int16_t camX, camY;

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
        Ship* ship = shipManager.ships;
        frameUnpaused += 1;
        if (gameState == GameState::Running) {

            frame += 1;
            shipManager.tick();
            particleSystem.tick();
            asteroidManager.tick();
            projectileManager.tick();
        }
        Menu::tick();

        Fixed2D4 cam = ship->pos + ship->direction * Fix4(8,0) + ship->velocity * Fix4(1,8);
        camX = (cam.x.getIntegerPart() * 7 + camX * 9) / 16;
        camY = (cam.y.getIntegerPart() * 7 + camY * 9) / 16;
        buffer.setOffset(camX - 48, camY -32);

        drawSpaceBackground(0, RGB565(62,62,62));
        drawSpaceBackground(1, RGB565(180,180,180));
        drawSpaceBackground(2, RGB565(120,120,120));
        UI::Shield::draw();
        UI::Radar::draw();
        shipManager.draw();
        particleSystem.draw();
        asteroidManager.draw();
        projectileManager.draw();
        Menu::draw();

    }

    void initialize(){
        frame = 0;
        atlas = Texture<uint16_t>(ImageAsset::atlas);
        shipManager.init();
        const Ship* ship = shipManager.ships;
        shipManager.ships[0].init(1,10,5,15,0);
        shipManager.ships[1].init(2,15,8,15,0);
        buffer.setClearBackground(true, RGB565(0,0,0));
        asteroidManager.init();
        asteroidManager.spawn()->init(1,600,145);
        asteroidManager.spawn()->init(1,-300,215);
        asteroidManager.spawn()->init(1,400,-385);
        asteroidManager.spawn()->init(1,-140,-185);
        projectileManager.init();
    }
}

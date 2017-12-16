#include "game_common.h"
#include "game_asteroids.h"
#include "game_particles.h"
#include "game_projectile.h"
#include "game_ships.h"
#include "game_menu.h"
#include "game_ui_radar.h"
#include "game_ui_shield.h"
#include "game_ui_hud.h"
#include "game_collectable.h"
#include "game_player_stats.h"
#include "game_ui_info.h"
#include "game_ui_intermission.h"
#include "game_persistence.h"

#include "game_level_main.h"
#include "game_level_tutorial.h"
#include "game_level_01.h"
#include "game_level_02.h"
#include "game_intro.h"

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
    bool showDebugInfo;
    uint8_t gameState;
    int frame, frameUnpaused;
    Texture<uint16_t> atlas;
    int16_t camX, camY;
    uint8_t currentLevelId;
    uint8_t whiteInAnimation;

    uint8_t screenBrightness = 255;

    void setScreenBrightness(uint8_t b) {
        if(b> 15) b = 15;
        if (b==screenBrightness) return;
        screenBrightness = b;
        display.setBrightness(b);
    }

    uint8_t getScreenBrightness() {
        return screenBrightness;
    }

    RenderCommand<uint16_t>* drawCenteredSprite(int x,int y,SpriteSheetRect rect) {
        return buffer.drawRect(x + rect.offsetX - rect.origWidth / 2,
                               y + rect.offsetY - rect.origHeight / 2,
                               rect.width,rect.height)->sprite(&atlas, rect.x, rect.y);
    }
    RenderCommand<uint16_t>* drawSprite(int x,int y,SpriteSheetRect rect) {
        return buffer.drawRect(x + rect.offsetX,
                               y + rect.offsetY,
                               rect.width,rect.height)->sprite(&atlas, rect.x, rect.y);
    }

    bool isPressed(int id) {
        return Joystick::getButton(id,Joystick::Phase::CURRENT) && !Joystick::getButton(id,Joystick::Phase::PREVIOUS);
    }
    bool isReleased(int id) {
        return !Joystick::getButton(id,Joystick::Phase::CURRENT) && Joystick::getButton(id,Joystick::Phase::PREVIOUS);
    }

    void drawSpaceBackground(int layer, uint16_t col, uint8_t cntMask) {
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
                Math::setSeed(xx + layer * 17 - yy/8,yy + xx / 4 + layer * 7);
                uint8_t n = (Math::randInt() & cntMask)+1;
                for (int8_t k = 0;k<n;k+=1) {
                    int16_t px = xx + (Math::randInt()&mask);
                    int16_t py = yy + (Math::randInt()&mask);
                    drawCenteredSprite(px>>layer,py>>layer,ImageAsset::TextureAtlas_atlas::stars.sprites[px&7])->blend(RenderCommandBlendMode::bitwiseOr);
                    //buffer.drawRect(px>>layer,py>>layer,1,1)->filledRect(col)->blend(RenderCommandBlendMode::bitwiseOr);
                }
            }
        }
        buffer.setOffset(x,y);
        //buffer.drawRect(ox,oy,64,64)->filledRect(RGB565(0,255,0));
    }

    void drawBackgrounds() {

        drawSpaceBackground(0, RGB565(62,62,62),1);
        drawSpaceBackground(1, RGB565(180,180,180),3);
        drawSpaceBackground(2, RGB565(120,120,120),3);
        drawSpaceBackground(3, RGB565(120,120,120),7);
    }

    void tick() {
        frameUnpaused += 1;
        if (currentLevelId == DESTINATION_INTRO) {
            Intro::tick();
            return;
        }
        if(whiteInAnimation > 0) {
            if (whiteInAnimation > 16) whiteInAnimation -= 16;
            else whiteInAnimation = 0;


            buffer.setClearBackground(true, RGB565(whiteInAnimation,whiteInAnimation,whiteInAnimation));

        }
        switch (currentLevelId) {
        case DESTINATION_TUTORIAL: Level::Tutorial::tick(); break;
        case DESTINATION_MAIN: Level::Main::tick(); break;
        case DESTINATION_01: Level::L01::tick(); break;
        case DESTINATION_02: Level::L02::tick(); break;
        case DESTINATION_03: Level::L03::tick(); break;
        }
        Ship* ship = shipManager.ships;
        if (!UI::Intermission::isActive()) {
            if (gameState == GameState::Running) {

                frame += 1;
                particleSystem.tick();
                shipManager.tick();
                asteroidManager.tick();
                projectileManager.tick();
                Collectable::tick();
                UI::Info::tick();
            }
            Menu::tick();
        }
        UI::Intermission::tick();

        int16_t targetX = ship->pos.x.getIntegerPart() + ship->direction.x.getIntegerPart() * 8 + ship->velocity.x.getIntegerPart();
        int16_t targetY = ship->pos.y.getIntegerPart() + ship->direction.y.getIntegerPart() * 8 + ship->velocity.y.getIntegerPart();
        int16_t dx = targetX - camX;
        int16_t dy = targetY - camY;
        if (abs(dx) < 200 && abs(dy) < 200) {
            camX = (targetX * 7 + camX * 9) / 16;
            camY = (targetY * 7 + camY * 9) / 16;
        } else {
            camX = targetX;
            camY = targetY;
        }
        buffer.setOffset(camX - 48, camY -32);
        drawBackgrounds();

        Math::setSeed(frame, frame * frame - 1283321);

        shipManager.draw();
        particleSystem.draw();
        asteroidManager.draw();
        projectileManager.draw();
        Collectable::draw();
        if (shipManager.ships[0].type) {
            UI::Radar::draw();
            UI::HUD::draw();
            if (gameState == GameState::Running) {
                UI::Shield::draw();
                UI::Info::draw();
            }
        }
        Menu::draw();
        UI::Intermission::draw();
    }

    void initializeLevel(uint8_t id) {
        uint8_t from = currentLevelId;
        currentLevelId = id;
        frame = 0;
        shipManager.init();
        asteroidManager.init();
        projectileManager.init();
        Collectable::init();
        UI::Info::init();
        gameState = GameState::Running;
        if (id == DESTINATION_INTRO) {
            Intro::init();
            return;
        }

        shipManager.ships[0].init(1,0,0,15,0,0);
        //shipManager.ships[2].init(3,35,8,15,0);
        whiteInAnimation = 255;
        buffer.setClearBackground(true, RGB565(255,255,255));

        switch (id) {
        case DESTINATION_TUTORIAL:
            Level::Tutorial::init();
            break;
        case DESTINATION_MAIN:
            Level::Main::init();
            break;
        case DESTINATION_01:
            Level::L01::init();
            break;
        case DESTINATION_02:
            Level::L02::init();
            break;
        case DESTINATION_03:
            Level::L03::init();
            break;
        }
        PlayerStats::jumped(from, id);
    }

    void checkLevelTermination(int16_t x, int16_t y) {
        for (int i=0;i<AsteroidsCount;i+=1) {
            if (asteroidManager.asteroids[i].type) return;
        }
        for (int i=0;i<ShipCount;i+=1) {
            if (shipManager.ships[i].type == ShipTypeEnemySmall) return;
        }
        for (int i=0;i<ShipCount;i+=1) {
            if (shipManager.ships[i].type == ShipTypeWormHoleInactive) {
                shipManager.ships[i].type = ShipTypeWormHole;
                return;
            }
        }
    }

    void initialize() {
        DB::init();
        currentLevelId = DESTINATION_INTRO;
        PlayerStats::init();
        atlas = Texture<uint16_t>(ImageAsset::atlas);
        setScreenBrightness(8);
        DB::load(-1);
        initializeLevel(DESTINATION_INTRO);
    }
}

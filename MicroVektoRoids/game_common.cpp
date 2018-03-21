#include "game_common.h"
#include "game_sound.h"
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
#include "lib_sound.h"
#include "lib_flashstore.h"

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
    struct HighScoreEntry {
        uint16_t score;
        char name[7];
    };

    HighScoreEntry entries[3] = {
        {2600, "Ken   "},
        {1950, "Damien"},
        {50, "Flaki "},

    };

    void addHighScoreEntry(uint16_t score) {
        for (int i=0;i<3;i+=1) {
            if (score >= entries[i].score) {
                for (int j=i+1;j<3;j+=1) {
                    entries[j] = entries[j-1];
                }
                entries[i].score = score;
                for (int j=0;j<sizeof(entries[i].name);j+=1) {
                    entries[i].name[j] = ' ';
                }
                break;
            }
        }
    }

    void handleHighscoreTickAndDraw() {
        static uint8_t pressed;
        static uint8_t position;
        static uint32_t stickActionTime;
        uint8_t scored =255;

        buffer.drawText("HIGH SCORES",0,10,96,8,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
        buffer.drawRect(5,20,96-10,1)->filledRect(0xffff);

        for (int8_t i=0;i<3;i+=1) {
            HighScoreEntry entry = entries[i];
            int16_t y = i *10 + 24;

            buffer.drawText(stringBuffer.start().putDec(entry.score).put(" - ").get(),
                            0,y,52,8,1,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            if (entry.score == PlayerStats::score && PlayerStats::score > 0 && scored == 255) {
                scored = i;
                buffer.drawRect(4,y,96-8,7)->filledRect(RGB565(0,0,96))->blend(RenderCommandBlendMode::add);
                for (int j=0;j<6;j++) {
                    int16_t x = 52 + j * 6;
                    for (int k=0;k< (position == j && Time::millis % 200 < 100 ? 3 : 1); k+=1 ) {
                        buffer.drawRect(x,y,5,7)->filledRect(RGB565(0,0,64))->blend(RenderCommandBlendMode::add);
                    }
                    if (position == j && Time::millis % 200 < 100){
                        buffer.drawRect(x,y-1,5,1)->filledRect(0xffff);
                        buffer.drawRect(x+1,y-2,3,1)->filledRect(0xffff);
                        buffer.drawRect(x+2,y-3,1,1)->filledRect(0xffff);
                        buffer.drawRect(x,y+8,5,1)->filledRect(0xffff);
                        buffer.drawRect(x+1,y+9,3,1)->filledRect(0xffff);
                        buffer.drawRect(x+2,y+10,1,1)->filledRect(0xffff);
                    }
                    buffer.drawText(stringBuffer.start().put(entry.name[j]).get(),
                                x,y,6,8,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
                }
            } else {
                buffer.drawText(stringBuffer.start().put(entry.name).get(),
                                52,y,96-52,8,-1,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            }
        }

        if (scored<255 && stickActionTime < Time::millis) {
            auto stick = Joystick::getJoystick();
            if (stick.x > Fix4(0,4)) {
                position = (position + 1) % 6;
                stickActionTime = Time::millis + 150;
            }
            if (stick.x < Fix4(-1,12)) {
                position = (position + 5) % 6;
                stickActionTime = Time::millis + 150;
            }
            if (stick.y > Fix4(0,4)) {
                char c = entries[scored].name[position] + 1;
                if (c == 'z'+1) c = 'A';
                if (c == 'Z'+1) c = '0';
                if (c == '9'+1) c = ' ';
                if (c == ' '+1) c = 'a';
                entries[scored].name[position] = c;
                stickActionTime = Time::millis + 150;
            }

            if (stick.y < Fix4(-1,12)) {
                char c = entries[scored].name[position] - 1;
                if (c == 'a'-1) c = ' ';
                if (c == ' '-1) c = '9';
                if (c == '0'-1) c = 'Z';
                if (c == 'A'-1) c = 'z';
                entries[scored].name[position] = c;
                stickActionTime = Time::millis + 150;
            }
            if (stick.x == 0 && stick.y == 0) {
                stickActionTime = 0;
            }
        }

        if (isPressed(0) || isPressed(1)) {
            pressed = 1;
        }
        if (pressed && (isReleased(0) || isReleased(1))) {
            pressed = 0;
            if (scored<255) {
                FlashStore::store(0x0001,"MicroVektoRoids",sizeof(entries),entries);
                PlayerStats::score = 0;
            }
            else {
                initializeLevel(DESTINATION_INTRO);
            }
        }
    }

    void tick() {
        ::Sound::tick();
        //if (Joystick::getButton(0,Joystick::Phase::CURRENT)
        //    && !Joystick::getButton(0,Joystick::Phase::PREVIOUS))
        //        Sound::playSample(1,samples, sizeof(samples), 0x100,0x200,0x32)->setChange(0x200,-1,-1);

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
        if (currentLevelId == DESTINATION_HIGHSCORE) {
            handleHighscoreTickAndDraw();
            buffer.setOffset(camX - 48, camY -32);
            drawBackgrounds();

            return;
        }
        switch (currentLevelId) {
        case DESTINATION_TUTORIAL: Level::Tutorial::tick(); break;
        case DESTINATION_MAIN: Level::Main::tick(); break;
        case DESTINATION_01: Level::L01::tick(); break;
        case DESTINATION_02: Level::L02::tick(); break;
        case DESTINATION_03a: Level::L03a::tick(); break;
        case DESTINATION_03b: Level::L03b::tick(); break;
        case DESTINATION_03c: Level::L03c::tick(); break;
        }
        Ship* ship = shipManager.ships;
        if (!UI::Intermission::isBlocking()) {
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
        case DESTINATION_TUTORIAL: Level::Tutorial::init(); break;
        case DESTINATION_MAIN: Level::Main::init(); break;
        case DESTINATION_01: Level::L01::init(); break;
        case DESTINATION_02: Level::L02::init(); break;
        case DESTINATION_03a: Level::L03a::init(); break;
        case DESTINATION_03b: Level::L03b::init(); break;
        case DESTINATION_03c: Level::L03c::init(); break;
        case DESTINATION_HIGHSCORE:

                FlashStore::restore(0x0001,sizeof(entries),entries);
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

    void finishGameOver() {
        addHighScoreEntry(PlayerStats::score);
        initializeLevel(DESTINATION_HIGHSCORE);

    }

    void initialize() {
        static uint8_t isFirstInit = 1;
        PlayerStats::init();
        if (isFirstInit) {
            ::Sound::init();
            DB::init();
            DB::load(-1);
            isFirstInit = 0;
            Game::Sound::volume = 4;
        }
        currentLevelId = DESTINATION_INTRO;
        atlas = Texture<uint16_t>(ImageAsset::atlas);
        setScreenBrightness(8);
        asteroidManager.init();
        shipManager.init();
        initializeLevel(DESTINATION_INTRO);
    }
}

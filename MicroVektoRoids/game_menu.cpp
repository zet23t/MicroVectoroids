#include "game_menu.h"
#include "game_ships.h"
#include "game_common.h"
#include "game_ui_hud.h"
#include "game_ui_radar.h"
#include "game_ships.h"
#include "game_sound.h"
#include "game_player_stats.h"
#include "game_asteroids.h"
#include "lib_sound.h"

#define SCREEN_MAIN 0
#define SCREEN_BRIGHTNESS 2
#define SCREEN_SOUND 3
#define SCREEN_CONTROLS 4
#define SCREEN_SELF_DESTRUCT 5
#define SCREEN_WORMHOLE_DETAILS 6

namespace Game {
    namespace Menu {
        uint8_t transition;
        int8_t menuSelected;
        uint8_t submenuSelected;
        uint8_t activeScreen;
        uint8_t blockInput;


        const int menuHeight = 56;
        bool activate;

        #define SubmenuCount 4

        void tick() {
            if (Joystick::getButton(0, Joystick::Phase::CURRENT) && !Joystick::getButton(0, Joystick::Phase::PREVIOUS)) {
                if (gameState == GameState::Running) {
                    if (shipManager.ships[0].type) {
                        gameState = GameState::Menu;
                    }
                    else {
                        // check high scores
                        Game::finishGameOver();
                    }
                }
                else {
                    gameState = GameState::Running;
                }
            }
            if (gameState == GameState::Menu && Joystick::getButton(1, Joystick::Phase::CURRENT) && !Joystick::getButton(1, Joystick::Phase::PREVIOUS))
            {
                activate = true;
            }
        }

        void setClipping(int16_t vpos) {
            buffer.setClipping(vpos+2,88,64-vpos-2,8);
        }

        const char *getMenuText(bool selected, const char* str) {
            if (!selected) return str;
            if (str[0] == '\n') {
                return stringBuffer.start().put("\n>").put(&str[1]).put("<").get();
            }
            return stringBuffer.start().put(">").put(str).put("<").get();
        }


        void drawRect(int16_t x, int16_t y, int16_t w, int16_t h, uint16_t col, uint8_t mode, bool filled) {
            if(filled) {
                buffer.drawRect(x,y,w,h)->filledRect(col)->blend(mode)->setDepth(200);
            }
            else {
                buffer.drawRect(x,y,w,1)->filledRect(col)->blend(mode)->setDepth(200);
                buffer.drawRect(x,y+h-1,w,1)->filledRect(col)->blend(mode)->setDepth(200);
                buffer.drawRect(x,y,1,h)->filledRect(col)->blend(mode)->setDepth(200);
                buffer.drawRect(x+w-1,y,1,h)->filledRect(col)->blend(mode)->setDepth(200);
            }
        }

        void drawScreenBars(const char *title, int16_t vpos) {
            uint8_t mode = RenderCommandBlendMode::opaque;
            buffer.drawText(title,8,vpos,80,16,0,0,false, FontAsset::font, 200, mode);
            buffer.drawRect(0,vpos + 11,96,1)->filledRect(RGB565(255,255,255))->blend(mode)->setDepth(200);
            buffer.drawRect(0,vpos + 53,96,1)->filledRect(RGB565(255,255,255))->blend(mode)->setDepth(200);
            buffer.drawText("CLOSE",8,vpos + 54,80,8,0,0,false, FontAsset::font, 200, mode);

        }

        void drawLevelControls(uint8_t &v,uint8_t maxLevel) {
            int targetWidth = 60;
            int blockWidth = targetWidth / maxLevel;
            int rectWidth = blockWidth - 2;
            int actualWidth = blockWidth * maxLevel - 2;
            int left = 48 - actualWidth/2;
            for (int i=0;i<maxLevel;i+=1) {
                drawRect(left + i*blockWidth,28,rectWidth,8,0xffff,RenderCommandBlendMode::opaque, v > i);
            }
            Fixed2D4 stick = Joystick::getJoystick();
            if (stick.y != 0 || stick.x != 0) {
                if (!blockInput) {
                    blockInput = 1;
                    if (stick.x > 0 && v < maxLevel) v++;
                    if (stick.x < 0 && v > 0) v--;
                }
            } else {
                blockInput = 0;
            }
        }

        void drawScreenControls(int16_t vpos) {
            if (activate) activeScreen = SCREEN_MAIN;
            uint8_t mode = RenderCommandBlendMode::opaque;
            drawScreenBars("SHIP CONTROLS", vpos);
            buffer.drawText("ABSOLUTE",19,32-8,72,8,-1,0,false, FontAsset::font, 200, mode);
            buffer.drawText("DIRECTIONAL",19,35,72,8,-1,0,false, FontAsset::font, 200, mode);
            drawRect(10,32-8,7,7,0xffff,mode,false);
            drawRect(10,32+3,7,7,0xffff,mode,false);
            if (!directionalControls) drawRect(12,32-6,3,3,0xffff,mode,true);
            else drawRect(12,32+5,3,3,0xffff,mode,true);
            Fixed2D4 stick = Joystick::getJoystick();
            if (stick.y != 0 || stick.x != 0) {
                if (!blockInput) {
                    blockInput = 1;
                    if (stick.y != 0) directionalControls = !directionalControls;
                }
            } else {
                blockInput = 0;
            }
        }
        void drawScreenSelfDestruct(int16_t vpos) {
            static uint8_t counter = 0;
            if (activate) {
                activeScreen = SCREEN_MAIN;
                counter = 0;
            }
            drawScreenBars("SELF DESTRUCT", vpos);
            drawLevelControls(counter, 4);
            if (counter == 4) {
                counter = 0;
                activeScreen = SCREEN_MAIN;
                gameState = GameState::Running;
                shipManager.ships[0].takeDamage(255);
                shipManager.ships[0].explode();
            }
        }

        void drawScreenBrightness(int16_t vpos) {
            if (activate) activeScreen = SCREEN_MAIN;
            drawScreenBars("SCREEN BRIGHTNESS", vpos);
            uint8_t b = (getScreenBrightness()+1)/2;
            drawLevelControls(b, 8);
            setScreenBrightness(b*2);
        }

        void drawScreenSound(int16_t vpos) {
            if (activate) activeScreen = SCREEN_MAIN;
            drawScreenBars("SOUND VOLUME", vpos);
            drawLevelControls(Sound::volume, 4);
            ::Sound::setGlobalVolume((uint8_t)(Sound::volume*2));

        }

        void drawWormholeDetails(int16_t vpos) {
            if (activate) activeScreen = SCREEN_MAIN;
            Ship *s = &shipManager.ships[UI::HUD::targetIndex];

            drawScreenBars(s->info, vpos);
            int8_t y = 32-menuHeight / 2+vpos+ 12;
            buffer.drawText("HIGH SCORES",12,y,74,8,0,-1,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            y+=9;
            const char *test[]= {
                "no data", "n/a",
                "no data", "n/a",
                "no data", "n/a",
            };
            if (s->destinationId < 32) {
                PlayerStats::HighScoreTable table = PlayerStats::levelScores[s->destinationId];
                for (int i=0;i<3;i+=1) {
                    if (table.entries[i].score > 0) {
                        test[i*2] = stringBuffer.start().putTrimmed(table.entries[i].name,8).get();
                        test[i*2+1] = stringBuffer.start().putDec(table.entries[i].score).get();
                    }
                }
            }

            for (int i=0;i<3;i+=1) {
                drawRect(8,y,80,7, RGB565(16,16,16), RenderCommandBlendMode::average, true);
                drawCenteredSprite(12+36,y+3,ImageAsset::TextureAtlas_atlas::collectable_score.sprites[(frameUnpaused + i * 2) / 2 % ImageAsset::TextureAtlas_atlas::collectable_score.spriteCount])->setDepth(200)->blend(RenderCommandBlendMode::add);
                buffer.drawText(test[i*2],12,y+1,32,8,1,-1,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
                buffer.drawText(test[i*2+1],12+42,y+1,34,8,-1,-1,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);

                y+=8;
            }

        }



        void drawTargetInfo(int16_t vpos) {
            uint8_t mode = RenderCommandBlendMode::opaque;
            if (UI::HUD::targetLock) {
                drawRect(12,20,24,24,0,RenderCommandBlendMode::average,true);
                drawRect(12,20,24,24,0xffff,RenderCommandBlendMode::average,false);
                buffer.setClipping(19,36,44,12);
                const uint8_t yoff = 12;
                if (UI::HUD::targetIsShip) {
                    Ship *s = &shipManager.ships[UI::HUD::targetIndex];
                    s->drawOnUI(24,32);
                    setClipping(vpos);
                    const char* typeName[] = {0,"Player ship","Station","ENEMY-01","WORMHOLE\nACTIVE","WORMHOLE\nINACTIVE"};

                    buffer.drawText(typeName[s->type],40,32-menuHeight / 2+yoff,38,16,-1,-1,false, FontAsset::font, 200, mode);
                    if (s->info) {
                        buffer.drawText("DESTINATION",40,32-menuHeight / 2+yoff+14,38,16,-1,-1,false, FontAsset::font, 200, mode);
                        buffer.drawText(s->info,40,32-menuHeight / 2+yoff+21,38,16,-1,-1,false, FontAsset::font, 200, mode);
                    }
                    if (s->type == ShipTypeEnemySmall) {
                        buffer.drawText(stringBuffer.start().put("#").putDec(UI::HUD::targetIndex).get(),40,32-menuHeight / 2+yoff+14,38,16,-1,-1,false, FontAsset::font, 200, mode);
                    }
                    if (s->type == ShipTypeWormHole) {
                        drawRect(40,32-menuHeight / 2+yoff+28,46,7,RGB565(255,0,0), RenderCommandBlendMode::average, true);
                        drawRect(40,32-menuHeight / 2+yoff+28,46,7,RGB565(255,0,0), RenderCommandBlendMode::average, false);
                        if (frameUnpaused / 8 % 2 == 0)
                            buffer.drawText("DETAILS?",40,32-menuHeight / 2+yoff+29,46,16,0,-1,false, FontAsset::font, 200, mode);
                        if (activate) {
                            //initializeLevel(s->destinationId);
                            if (activate) activeScreen = SCREEN_WORMHOLE_DETAILS;
                        }
                    }
                } else {
                    Asteroid *a = &asteroidManager.asteroids[UI::HUD::targetIndex];
                    a->drawOnUI(24,32);
                    setClipping(vpos);
                    const char* typeName[] = {0,"WHITE-A\nFRAGMENTING","","","WHITE-B\nHARVESTABLE"};

                    buffer.drawText(typeName[a->type],40,32-menuHeight / 2+yoff,38,16,-1,-1,false, FontAsset::font, 200, mode);

                }
            } else {
                buffer.drawText("NO TARGET",0,0,96,64,0,0,false, FontAsset::font, 200, mode);
            }
        }

        void drawRadarContent() {
            static uint8_t mode = 1;
            drawRect(8,32-19,80,40,0x0000,RenderCommandBlendMode::average, true);
            if (mode > 0) {
                uint8_t zoom = mode + 2;
                Fixed2D4 origin = shipManager.ships[0].pos;
                uint8_t rs = mode > 2 ? 1 : (mode < 2 ? 3 : 2);
                uint8_t rso = rs / 2;
                for (int i=0;i<ShipCount;i+=1) {
                    Ship *s = &shipManager.ships[i];
                    if (s->type) {
                        Fixed2D4 rel = s->pos - origin;
                        int x = rel.x.getIntegerPart() >> zoom;
                        int y = rel.y.getIntegerPart() >> zoom;
                        if (abs(x) < 40&& abs(y) < 20) {
                            drawRect(x + 48 - rso,y + 32-rso,rs,rs,UI::Radar::blipShipColors[s->type],RenderCommandBlendMode::opaque,true);
                        }
                    }
                }
                for (int i=0;i<AsteroidsCount;i+=1) {
                    Asteroid *a = &asteroidManager.asteroids[i];
                    if (a->type) {
                        Fixed2D4 rel = a->pos - origin;
                        int x = rel.x.getIntegerPart() >> zoom;
                        int y = rel.y.getIntegerPart() >> zoom;
                        if (abs(x) < 40&& abs(y) < 20) {
                            drawRect(x + 48 - rso,y + 32-rso,rs,rs,RGB565(128,128,128),RenderCommandBlendMode::opaque,true);
                        }

                    }
                }
                uint8_t step = 128>>zoom;
                for (int x=48;x<=88;x+=step) {
                    drawRect(x,32-20,1,41,RGB565(20,20,80),RenderCommandBlendMode::add,true);
                }
                for (int x=48 - step;x>=8;x-=step) {
                    drawRect(x,32-20,1,41,RGB565(20,20,80),RenderCommandBlendMode::add,true);
                }
                for (int y = 32; y<=53;y+=step) {
                    drawRect(8,y,80,1,RGB565(20,20,80),RenderCommandBlendMode::add,true);
                }
                for (int y = 32-step; y>=12;y-=step) {
                    drawRect(8,y,80,1,RGB565(20,20,80),RenderCommandBlendMode::add,true);
                }

                buffer.drawText(stringBuffer.start().put("1:").putDec(1<<zoom).get(),8,12,80,41,1,1,false,FontAsset::font,200);
            } else {
                drawRect(8,14,3,5,RGB565(128,128,128), RenderCommandBlendMode::opaque,true);
                drawRect(8,20,3,5,UI::Radar::blipShipColors[1], RenderCommandBlendMode::opaque,true);
                drawRect(8,26,3,5,UI::Radar::blipShipColors[2], RenderCommandBlendMode::opaque,true);
                drawRect(8,32,3,5,UI::Radar::blipShipColors[3], RenderCommandBlendMode::opaque,true);
                drawRect(8,38,3,5,UI::Radar::blipShipColors[4], RenderCommandBlendMode::opaque,true);

                buffer.drawText("ASTEROID\n"
                                "YOU\n"
                                "STATION\n"
                                "ACTIVE WORMHOLE\n"
                                "INACTIVE WORMHOLE\n"
                                ,12,14,75,41,-1,-1,false,FontAsset::font,200);

            }
            if (activate) {
                mode = (mode + 1) % 4;
            }
        }

        void drawSubmenus(int16_t vpos) {
            const char *menuTitle[] = {
                "TARGET",
                "RADAR",
                "SHIP",
                "OPTIONS",
            };
            uint8_t mode = RenderCommandBlendMode::opaque;
            buffer.drawText(menuTitle[submenuSelected],8,vpos,80,16,0,0,false, FontAsset::font, 200, mode);
            buffer.drawRect(0,vpos + 11,96,1)->filledRect(RGB565(255,255,255))->blend(mode)->setDepth(200);
            buffer.drawRect(0,vpos + 53,96,1)->filledRect(RGB565(255,255,255))->blend(mode)->setDepth(200);

            buffer.drawRect(48-5-6+12*(submenuSelected-1),vpos + 54,11,8)->filledRect(RGB565(60,80,255))->blend(RenderCommandBlendMode::add)->setDepth(200);
            drawCenteredSprite(48-18,vpos+57,ImageAsset::TextureAtlas_atlas::icon_target.sprites[0])->setDepth(200)->blend(RenderCommandBlendMode::bitwiseOr);
            drawCenteredSprite(48-6,vpos+57,ImageAsset::TextureAtlas_atlas::icon_radar.sprites[0])->setDepth(200)->blend(RenderCommandBlendMode::bitwiseOr);
            drawCenteredSprite(48+6,vpos+57,ImageAsset::TextureAtlas_atlas::icon_ship.sprites[0])->setDepth(200)->blend(RenderCommandBlendMode::bitwiseOr);
            drawCenteredSprite(48+18,vpos+57,ImageAsset::TextureAtlas_atlas::icon_options.sprites[0])->setDepth(200)->blend(RenderCommandBlendMode::bitwiseOr);

            Ship *s = shipManager.ships;
            switch(submenuSelected) {
            case 0: // target
                drawTargetInfo(vpos);
                break;
            case 2: // ship
                buffer.drawText(stringBuffer.start().put("STATUS: ").putDec(100-s->damage*100/s->maxDamage()).put("%").get(),0,vpos+12,96,8,0,0,false, FontAsset::font, 200, mode);
                buffer.drawText(stringBuffer.start().put("DUST: ").putDec(PlayerStats::score).get(),0,vpos+20,96,8,0,0,false, FontAsset::font, 200, mode);
                if (menuSelected < 0) menuSelected +=2;
                menuSelected %= 2;
                buffer.drawText(getMenuText(menuSelected == 0, "CONTROLS"),0,vpos+30,96,8,0,0,false, FontAsset::font, 200, mode);
                buffer.drawText(getMenuText(menuSelected == 1, "SELF DESTRUCT"),0,vpos+38,96,8,0,0,false, FontAsset::font, 200, mode);
                if (activate) {
                    switch (menuSelected) {
                        case 0: activeScreen = SCREEN_CONTROLS; break;
                        case 1: activeScreen = SCREEN_SELF_DESTRUCT; break;
                    }
                }
                break;
            case 1:
                drawRadarContent();
                break;
            case 3:
                if (menuSelected < 0) menuSelected +=4;
                menuSelected %= 3;
                {
                    const uint8_t yoff = 16;
                    buffer.drawText(getMenuText(menuSelected == 0, "SOUND VOLUME"),0,vpos+yoff,96,8,0,0,false, FontAsset::font, 200, mode);
                    buffer.drawText(getMenuText(menuSelected == 1, "BRIGHTNESS"),0,vpos+yoff+8,96,8,0,0,false, FontAsset::font, 200, mode);
                    buffer.drawText(getMenuText(menuSelected == 2, Game::showDebugInfo ? "DEBUG INFO ON" : "DEBUG INFO OFF"),0,vpos+yoff+24,96,8,0,0,false, FontAsset::font, 200, mode);
                }
                if (activate) {
                    switch (menuSelected) {
                        case 0: activeScreen = SCREEN_SOUND; break;
                        case 1: activeScreen = SCREEN_BRIGHTNESS; break;
                        case 2: showDebugInfo = !showDebugInfo; break;
                    }
                }
            }

            Fixed2D4 stick = Joystick::getJoystick();
            if (stick.y != 0 || stick.x != 0) {
                if (!blockInput) {
                    blockInput = 1;
                    if (stick.y > 0)
                        menuSelected = (menuSelected + 1);
                    if (stick.y < 0)
                        menuSelected = (menuSelected - 1);
                    if (stick.x > 0) submenuSelected = (submenuSelected + 1) % SubmenuCount;
                    if (stick.x < 0) submenuSelected = (submenuSelected - 1 + SubmenuCount) % SubmenuCount;
                }
            } else {
                blockInput = 0;
            }
        }

        void draw() {
            buffer.setOffset(0,0);

            if (gameState == GameState::Menu) {
                if (transition < menuHeight) {
                    transition += (menuHeight - transition) / 2 + 1;
                    blockInput = 1;
                }
            }
            else {
                activeScreen = SCREEN_MAIN;
                menuSelected = 0;
                if (transition > 0) transition -= (transition) / 2 + 1;
            }
            if (transition > 0) {
                if (shipManager.ships[0].type) {
                    uint16_t col = RGB565(148-transition,160-transition,240-transition);
                    int16_t h = transition/2;
                    uint8_t mode = RenderCommandBlendMode::average;
                    buffer.drawRect(8,32-h,96-16,h*2)->filledRect(col)->blend(mode)->setDepth(200);
                    buffer.drawRect(9,32-h-1,96-18,1)->filledRect(col)->blend(mode)->setDepth(200);
                    buffer.drawRect(9,32+h,96-18,1)->filledRect(col)->blend(mode)->setDepth(200);
                    buffer.drawRect(10,32-h-2,96-20,1)->filledRect(col)->blend(mode)->setDepth(200);
                    buffer.drawRect(10,32+h+1,96-20,1)->filledRect(col)->blend(mode)->setDepth(200);
                    int16_t vpos = menuHeight/2 - h;
                    setClipping(vpos);

                    switch (activeScreen) {
                    default: drawSubmenus(vpos); break;
                    case SCREEN_BRIGHTNESS: drawScreenBrightness(vpos); break;
                    case SCREEN_SOUND: drawScreenSound(vpos); break;
                    case SCREEN_CONTROLS: drawScreenControls(vpos); break;
                    case SCREEN_SELF_DESTRUCT: drawScreenSelfDestruct(vpos); break;
                    case SCREEN_WORMHOLE_DETAILS: drawWormholeDetails(vpos); break;
                    }
                    activate = false;
                    buffer.setClipping(0,96,64,0);
                } else {

                }


            }
            if (shipManager.ships[0].type == 0) {
                const char *tx = stringBuffer.start().put("GAME\nOVER\n\nYOUR SCORE:\n\n").putDec(PlayerStats::score).get();
                buffer.drawText(tx,0,0,96,64,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            }
            //buffer.drawText("MicroVektoRoids Alpha",0,0,96,64,1,1,false, FontAsset::font, 200, RenderCommandBlendMode::add);
            buffer.setOffset(camX, camY);
        }
    }
}

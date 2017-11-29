#include "game_menu.h"
#include "game_ships.h"
#include "game_common.h"
#include "game_ui_hud.h"
#include "game_ui_radar.h"
#include "game_ships.h"
#include "game_sound.h"
#include "game_player_stats.h"
#include "game_asteroids.h"

#define SCREEN_MAIN 0
#define SCREEN_CREDITS 1
#define SCREEN_BRIGHTNESS 2
#define SCREEN_SOUND 3
#define SCREEN_CONTROLS 4
#define SCREEN_SELF_DESTRUCT 5

namespace Game {
    namespace Menu {
        uint8_t transition;
        int8_t menuSelected;
        uint8_t submenuSelected;
        uint8_t activeScreen;
        uint8_t blockInput;
        uint16_t creditsAnim;


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
                        Game::initialize();
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
            return stringBuffer.start().put(">").put(str).put("<").get();
        }

        void putText(int &y, const char *tx, int space) {
            buffer.drawText(tx,8,y,80,8,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            y+=7+space;
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
        }

        void drawCredits(int16_t vpos) {
            if (activate) activeScreen = SCREEN_MAIN;
            buffer.setOffset(0,creditsAnim - 60);
            drawCenteredSprite(48,20, ImageAsset::TextureAtlas_atlas::logo.sprites[0])->setDepth(200)->blend(RenderCommandBlendMode::add);
            //buffer.drawRect(48,0,1,64)->filledRect(RGB565(60,80,255))->blend(RenderCommandBlendMode::add)->setDepth(200);
            int y = 40;
            putText(y,"DEVELOPED BY",0);
            putText(y,"Eike Decker",5);

            putText(y,"MY PATRONS",2);
            putText(y,"Ken Burns",0);
            putText(y,"Flaki",5);

            putText(y,"Your name could",0);
            putText(y,"be here too!",0);
            putText(y,"Support me on",0);
            putText(y,"patreon.com/zet23t",5);

            putText(y,"MICRO VEKTOROIDS",0);
            putText(y,"is based on",2);
            putText(y,"SUPER VEKTOROIDS",2);
            putText(y,"by",2);
            putText(y,"PIXEL STRIKE GAMES",5);

            putText(y,"FONTS",2);
            putText(y,"HEMI HEAD",0);
            putText(y,"by TypoDermic Fonts",15);

            putText(y,"THANKS TO",3);
            putText(y,"TINY CIRCUITS",0);
            putText(y,"for the Tiny Arcade",8);

            putText(y,"SPECIAL THANKS TO",3);
            putText(y,"Swantje,",0);
            putText(y,"Hilda, Till and Piet",15);

            putText(y,"Thank you for playing",0);
            putText(y,"Micro VektoRoids",15);
            putText(y,"Let me know if",0);
            putText(y,"you liked it!",0);
            buffer.setOffset(0,0);

            if (frameUnpaused % 4 == 0) {
                creditsAnim +=1;
                if (creditsAnim > y + 60) creditsAnim = 0;
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
                            buffer.drawText("ACTIVATE?",40,32-menuHeight / 2+yoff+29,46,16,0,-1,false, FontAsset::font, 200, mode);
                        if (activate) {
                            initializeLevel(s->destinationId);
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
            int menuCount = 1;
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
                menuSelected %= 4;
                {
                    const uint8_t yoff = 16;
                    buffer.drawText(getMenuText(menuSelected == 0, "SOUND VOLUME"),0,vpos+yoff,96,8,0,0,false, FontAsset::font, 200, mode);
                    buffer.drawText(getMenuText(menuSelected == 1, "BRIGHTNESS"),0,vpos+yoff+8,96,8,0,0,false, FontAsset::font, 200, mode);
                    buffer.drawText(getMenuText(menuSelected == 2, "CREDITS"),0,vpos+yoff+16,96,8,0,0,false, FontAsset::font, 200, mode);
                    buffer.drawText(getMenuText(menuSelected == 3, Game::showDebugInfo ? "DEBUG INFO ON" : "DEBUG INFO OFF"),0,vpos+yoff+24,96,8,0,0,false, FontAsset::font, 200, mode);
                }
                if (activate) {
                    switch (menuSelected) {
                        case 0: activeScreen = SCREEN_SOUND; break;
                        case 1: activeScreen = SCREEN_BRIGHTNESS; break;
                        case 2:
                            creditsAnim = 0;
                            activeScreen = SCREEN_CREDITS;
                            break;
                        case 3: showDebugInfo = !showDebugInfo; break;
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
                    case SCREEN_CREDITS: drawCredits(vpos); break;
                    case SCREEN_CONTROLS: drawScreenControls(vpos); break;
                    case SCREEN_SELF_DESTRUCT: drawScreenSelfDestruct(vpos); break;
                    }
                    activate = false;
                    buffer.setClipping(0,96,64,0);
                } else {

                }


            }
            if (shipManager.ships[0].type == 0) {
                buffer.drawText("GAME\nOVER",0,0,96,64,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            }
            //buffer.drawText("MicroVektoRoids Alpha",0,0,96,64,1,1,false, FontAsset::font, 200, RenderCommandBlendMode::add);
            buffer.setOffset(camX, camY);
        }
    }
}

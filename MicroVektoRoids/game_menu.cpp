#include "game_menu.h"
#include "game_ships.h"
#include "game_common.h"
#include "game_ui_hud.h"
#include "game_ships.h"
#include "game_player_stats.h"

#define SCREEN_MAIN 0
#define SCREEN_CREDITS 1
#define SCREEN_SCREEN 2

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
                if (gameState == GameState::Running)
                    gameState = GameState::Menu;
                else {
                    gameState = GameState::Running;
                }
            }
            if (Joystick::getButton(1, Joystick::Phase::CURRENT) && !Joystick::getButton(1, Joystick::Phase::PREVIOUS))
            {
                activate = true;
            }
        }
        const char *getMenuText(bool selected, const char* str) {
            if (!selected) return str;
            return stringBuffer.start().put(">").put(str).put("<").get();
        }

        void putText(int &y, const char *tx, int space) {
            buffer.drawText(tx,8,y,80,8,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            y+=7+space;
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

        void drawSubmenus(int16_t vpos) {
            uint8_t mode = RenderCommandBlendMode::opaque;
            const char *menuTitle[] = {
                "TARGET",
                "RADAR",
                "SHIP",
                "OPTIONS",
            };
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
                if (UI::HUD::targetLock) {
                    buffer.drawText(UI::HUD::highlightInfo ? UI::HUD::highlightInfo : "< ??? >",8,32-menuHeight / 2+8,80,menuHeight-16,1,1,false, FontAsset::font, 200, mode);
                } else {
                    buffer.drawText("NO TARGET",0,0,96,64,0,0,false, FontAsset::font, 200, mode);
                }
                break;
            case 2: // ship
                buffer.drawText(stringBuffer.start().put("STATUS: ").putDec(100-s->damage*100/s->maxDamage()).put("%").get(),0,vpos+12,96,8,0,0,false, FontAsset::font, 200, mode);
                buffer.drawText(stringBuffer.start().put("DUST: ").putDec(PlayerStats::score).get(),0,vpos+20,96,8,0,0,false, FontAsset::font, 200, mode);
                if (menuSelected < 0) menuSelected +=3;
                menuSelected %= 3;
                buffer.drawText(getMenuText(menuSelected == 0, "CONTROLS"),0,vpos+30,96,8,0,0,false, FontAsset::font, 200, mode);
                buffer.drawText(getMenuText(menuSelected == 1, "SHIP INFO"),0,vpos+38,96,8,0,0,false, FontAsset::font, 200, mode);
                buffer.drawText(getMenuText(menuSelected == 2, "SELF DESTRUCT"),0,vpos+46,96,8,0,0,false, FontAsset::font, 200, mode);
                if (activate) {
                    if (menuSelected == 0) {

                    }
                }
                break;
            case 1:
                break;
            case 3:
                if (menuSelected < 0) menuSelected +=3;
                menuSelected %= 3;
                buffer.drawText(getMenuText(menuSelected == 0, "SOUND"),0,vpos+20,96,8,0,0,false, FontAsset::font, 200, mode);
                buffer.drawText(getMenuText(menuSelected == 1, "SCREEN"),0,vpos+28,96,8,0,0,false, FontAsset::font, 200, mode);
                buffer.drawText(getMenuText(menuSelected == 2, "CREDITS"),0,vpos+36,96,8,0,0,false, FontAsset::font, 200, mode);
                if (activate) {
                    if (menuSelected == 2) {
                        creditsAnim = 0;
                        activeScreen = SCREEN_CREDITS;
                    }
                }
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
                uint16_t col = RGB565(148-transition,160-transition,240-transition);
                int16_t h = transition/2;
                uint8_t mode = RenderCommandBlendMode::average;
                buffer.drawRect(8,32-h,96-16,h*2)->filledRect(col)->blend(mode)->setDepth(200);
                buffer.drawRect(9,32-h-1,96-18,1)->filledRect(col)->blend(mode)->setDepth(200);
                buffer.drawRect(9,32+h,96-18,1)->filledRect(col)->blend(mode)->setDepth(200);
                buffer.drawRect(10,32-h-2,96-20,1)->filledRect(col)->blend(mode)->setDepth(200);
                buffer.drawRect(10,32+h+1,96-20,1)->filledRect(col)->blend(mode)->setDepth(200);
                int16_t vpos = menuHeight/2 - h;
                buffer.setClipping(vpos+1,88,64-vpos-2,8);

                switch (activeScreen) {
                default:
                    drawSubmenus(vpos);
                    break;
                case SCREEN_CREDITS:
                    drawCredits(vpos);
                    break;
                }
                activate = false;

                buffer.setClipping(0,96,64,0);
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

            //buffer.drawText("MicroVektoRoids Alpha",0,0,96,64,1,1,false, FontAsset::font, 200, RenderCommandBlendMode::add);
            buffer.setOffset(camX, camY);
        }
    }
}

#include "game_menu.h"
#include "game_ships.h"
#include "game_common.h"
#include "game_ui_hud.h"

namespace Game {
    namespace Menu {
        uint8_t transition;
        uint8_t menuSelected;
        uint8_t submenuSelected;
        uint8_t blockInput;

        struct Submenu {
            virtual const char *getTitle() const {
                return "Menu";
            }
            virtual const char *getText() const {
                return "undefined";
            }
            virtual void activate() const {

            }
        };

        struct SubmenuRestart:Submenu {
            virtual const char *getTitle() const {
                return "SHIP COMPUTER";
            }
            virtual const char *getText() const {
                return "Restart";
            }
            virtual void activate() const {
                gameState = GameState::Running;
                initialize();
            }
        };

        struct SubmenuToggleInput:Submenu {
            virtual const char *getTitle() const {
                return "Controls";
            }
            virtual const char *getText() const {
                return directionalControls ? "Directional" : "Absolute";
            }
            virtual void activate() const {
                directionalControls = !directionalControls;
            }
        };

        struct SubmenuToggleDebug:Submenu {
            virtual const char *getTitle() const {
                return "Debug";
            }
            virtual const char *getText() const {
                return showDebugInfo ? "Show info" : "Hide info";
            }
            virtual void activate() const {
                showDebugInfo = !showDebugInfo;
            }
        };

        SubmenuRestart restart;
        SubmenuToggleInput toggleInput;
        SubmenuToggleDebug toggleDebug;
        #define SubmenuCount 3
        const Submenu* submenus[] = {
            &restart,
            &toggleInput,
            &toggleDebug,
        };

        void tick() {
            if (Joystick::getButton(0, Joystick::Phase::CURRENT) && !Joystick::getButton(0, Joystick::Phase::PREVIOUS))
            {
                if (gameState == GameState::Running)
                    gameState = GameState::Menu;
                else {
                    if (menuSelected == 0)
                        gameState = GameState::Running;
                    else {
                        submenus[submenuSelected]->activate();
                    }
                }
            }
        }
        void draw() {
            const int menuHeight = 56;
            buffer.setOffset(0,0);

            if (gameState == GameState::Menu) {
                if (transition < menuHeight) {
                    transition += (menuHeight - transition) / 2 + 1;
                    blockInput = 1;
                }
            }
            else {
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
                mode = RenderCommandBlendMode::opaque;
                const char *menuTitle[] = {
                    "TARGET",
                    "RADAR",
                    "SHIP",
                };
                buffer.drawText(menuTitle[submenuSelected],8,vpos,80,16,0,0,false, FontAsset::font, 200, mode);
                buffer.drawRect(0,vpos + 11,96,1)->filledRect(RGB565(255,255,255))->blend(mode)->setDepth(200);
                buffer.drawRect(0,vpos + 53,96,1)->filledRect(RGB565(255,255,255))->blend(mode)->setDepth(200);
                //const int menuWidth = 11;
                //buffer.drawRect(menuWidth + 8,vpos + 11,1,60)->filledRect(RGB565(255,255,255))->setDepth(200)->blend(mode);
                //buffer.drawRect(48,vpos + 11,1,60)->filledRect(RGB565(255,255,255))->setDepth(200)->blend(mode);


                buffer.drawRect(48-5+12*(submenuSelected-1),vpos + 54,11,8)->filledRect(RGB565(60,80,255))->blend(RenderCommandBlendMode::add)->setDepth(200);
                drawCenteredSprite(46-12,vpos+59,ImageAsset::TextureAtlas_atlas::icon_target.sprites[0])->setDepth(200)->blend(RenderCommandBlendMode::bitwiseOr);
                drawCenteredSprite(46,vpos+59,ImageAsset::TextureAtlas_atlas::icon_radar.sprites[0])->setDepth(200)->blend(RenderCommandBlendMode::bitwiseOr);
                drawCenteredSprite(46+12,vpos+59,ImageAsset::TextureAtlas_atlas::icon_ship.sprites[0])->setDepth(200)->blend(RenderCommandBlendMode::bitwiseOr);

                switch(submenuSelected) {
                case 0: // target
                    if (UI::HUD::targetLock) {
                        buffer.drawText(UI::HUD::highlightInfo ? UI::HUD::highlightInfo : "<???>",8,32-menuHeight / 2+8,80,menuHeight-16,1,1,false, FontAsset::font, 200, mode);
                    } else {
                        buffer.drawText("NO TARGET",0,0,96,64,0,0,false, FontAsset::font, 200, mode);
                    }
                    break;
                case 2: // ship
                    buffer.drawText("STATUS: 100%",0,vpos+12,96,8,0,0,false, FontAsset::font, 200, mode);
                    buffer.drawText("DUST: 123",0,vpos+20,96,8,0,0,false, FontAsset::font, 200, mode);
                    buffer.drawText("CONTROLS: DIRECT",0,vpos+30,96,8,0,0,false, FontAsset::font, 200, mode);
                    buffer.drawText("SHIP INFO",0,vpos+38,96,8,0,0,false, FontAsset::font, 200, mode);
                    buffer.drawText("SELF DESTRUCT",0,vpos+46,96,8,0,0,false, FontAsset::font, 200, mode);
                    break;
                case 1:
                    break;
                }
                /*buffer.drawText("TARGET",8,vpos + 12,menuWidth,8,1,0,false,FontAsset::font,200, mode);
                buffer.drawText("SHIP",8,vpos + 19,menuWidth,8,1,0,false,FontAsset::font,200, mode);
                buffer.drawText("RADAR",8,vpos + 26,menuWidth,8,1,0,false,FontAsset::font,200, mode);*/

                //buffer.drawText("CREDITS",8,vpos + 33,menuWidth,8,1,0,false,FontAsset::font,200, mode);

                /*buffer.drawRect(8,32-transition/2,96-16,transition)->filledRect(RGB565(180,160,120))->blend(RenderCommandBlendMode::subtract)->setDepth(200);

                buffer.drawRect(7,32-transition/2,3,transition+1)->filledRect(col)->blend(RenderCommandBlendMode::add)->setDepth(200);
                buffer.drawRect(8,32-transition/2-1,1,transition+2)->filledRect(col)->blend(RenderCommandBlendMode::add)->setDepth(200);
                buffer.drawRect(86,32-transition/2,3,transition+1)->filledRect(col)->blend(RenderCommandBlendMode::add)->setDepth(200);
                buffer.drawRect(87,32-transition/2-1,1,transition+2)->filledRect(col)->blend(RenderCommandBlendMode::add)->setDepth(200);

                buffer.drawRect(8,31-transition/2,80,3)->filledRect(col)->blend(RenderCommandBlendMode::add)->setDepth(200);
                buffer.drawRect(7,32-transition/2,82,1)->filledRect(col)->blend(RenderCommandBlendMode::add)->setDepth(200);
                buffer.drawRect(8,31+transition/2,80,3)->filledRect(col)->blend(RenderCommandBlendMode::add)->setDepth(200);
                buffer.drawRect(7,32+transition/2,82,1)->filledRect(col)->blend(RenderCommandBlendMode::add)->setDepth(200);

                int16_t vpos = 31 - transition/2;
                buffer.setClipping(vpos+1,88,64-vpos,8);
                buffer.drawText(submenus[submenuSelected]->getTitle(),8,vpos+1,80,16,0,0,false, FontAsset::hemifont, 200, RenderCommandBlendMode::bitwiseOr);

                buffer.drawText(submenus[submenuSelected]->getText(),8,vpos+18,80,16,0,0,false, FontAsset::hemifont, 200, RenderCommandBlendMode::bitwiseOr);
                buffer.drawText("Continue",8,vpos+35,80,16,0,0,false, FontAsset::hemifont, 200, RenderCommandBlendMode::bitwiseOr);
                const SpriteSheetRect *sheet = &ImageAsset::TextureAtlas_atlas::menu_triangle_left.sprites[0];
                buffer.drawRect(6+sheet->offsetX,vpos + 17 + sheet->offsetY,sheet->width,sheet->height)->sprite(&atlas,sheet->x, sheet->y)->blend(RenderCommandBlendMode::add)->setDepth(200);
                sheet = &ImageAsset::TextureAtlas_atlas::menu_triangle_right.sprites[0];
                buffer.drawRect(73+sheet->offsetX,vpos + 17 + sheet->offsetY,sheet->width,sheet->height)->sprite(&atlas,sheet->x, sheet->y)->blend(RenderCommandBlendMode::add)->setDepth(200);
                uint8_t anim = frameUnpaused % 22;
                anim = abs(11-anim);
                anim*=6;



                buffer.drawRect(7,vpos + (menuSelected ? 18 : 35),82,15)->filledRect(RGB565(anim,anim,anim))->blend(RenderCommandBlendMode::add)->setDepth(200);
                */
                buffer.setClipping(0,96,64,0);
                Fixed2D4 stick = Joystick::getJoystick();
                if (stick.y != 0 || stick.x != 0) {
                    if (!blockInput) {
                        blockInput = 1;
                        if (stick.y != 0)
                            menuSelected = (menuSelected + 1)%2;
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

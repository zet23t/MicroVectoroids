#include "game_menu.h"
#include "game_ships.h"
#include "game_common.h"

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
                return "Game";
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

        SubmenuRestart restart;
        SubmenuToggleInput toggleInput;
        #define SubmenuCount 2
        const Submenu* submenus[] = {
            &restart,
            &toggleInput
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
            buffer.setOffset(0,0);

            if (gameState == GameState::Menu) {
                if (transition < 48) {
                    transition += (48 - transition) / 2 + 1;
                    blockInput = 1;
                }
            }
            else {
                menuSelected = 0;
                if (transition > 0) transition -= (transition) / 2 + 1;
            }
            if (transition > 0) {
                uint16_t col = RGB565(118-transition,140-transition,180-transition);
                buffer.drawRect(8,32-transition/2,96-16,transition)->filledRect(RGB565(180,160,120))->blend(RenderCommandBlendMode::subtract)->setDepth(200);

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

                buffer.drawRect(7,vpos + (menuSelected ? 18 : 35),82,15)->filledRect(RGB565(anim,anim,anim))->blend(RenderCommandBlendMode::add)->setDepth(200);

                buffer.setClipping(0,96,64,0);
            }

            buffer.setOffset(camX, camY);
        }
    }
}

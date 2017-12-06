#include "game_intro.h"
#include "game_common.h"
#include "game_menu.h"

#define INTRO_MODE_LOGO 0
#define INTRO_MODE_MENU 1

namespace Game {
    namespace Intro {
        uint8_t mode;
        uint8_t selected;
        uint8_t blockJoystick;

        static void menuNewGame() {
            initializeLevel(DESTINATION_TUTORIAL);
        }

        const char *menuTexts[] = {
            "PLAY",
            "NEW GAME",
            "LOAD GAME",
            "CREDITS",
            0,
            "DIFFICULTY",
            "TUTORIAL",
            "NORMAL",
            0,
            "LOADING",
            "STATE 1",
            "STATE 2",
            "STATE 3",
            0,
        };

        typedef void (*Action)();

        static Action menuActions[] = {
            0,
            menuNewGame,
            menuNewGame,
            menuNewGame,

        };

        static void drawMenu(const char **texts, Action* actions) {
            buffer.drawText(texts[0],0,12,96,16,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            buffer.drawRect(12,19+6,96-24,1)->filledRect(0xffff);
            int y = 26;
            int n = 1;
            for (;texts[n];n+=1) {
                buffer.drawText(Menu::getMenuText(selected == n-1,texts[n]),0,y,96,16,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
                y+=8;
            }
            n-=1;

            Fixed2D4 stick = Joystick::getJoystick();
            if (stick.y == 0) blockJoystick = 0;
            if (blockJoystick > 0) blockJoystick--;
            if (!blockJoystick && stick.y>0) {
                blockJoystick = 5;
                selected = (selected + 1) % n;
            }
            if (!blockJoystick && stick.y<0) {
                blockJoystick= 5;
                selected = (selected - 1 + n) % n;
            }
            //buffer.drawText(Menu::getMenuText(selected == 1,"NORMAL"),0,36,96,16,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            if (isReleased(0) || isReleased(1))
            {
                //mode = INTRO_MODE_MENU;
                //
                actions[selected + 1]();
            }

        }

        void tick() {
            for(int i=0;i<1;i+=1) {
                buffer.setOffset(frameUnpaused * 2 + i,0);
                drawBackgrounds();
            }
            buffer.setOffset(0,0);
            switch (mode) {
            case INTRO_MODE_MENU:
                drawMenu(menuTexts, menuActions);
                break;
            case INTRO_MODE_LOGO:
                {
                    drawCenteredSprite(48,32,ImageAsset::TextureAtlas_atlas::logo.sprites[0])->blend(RenderCommandBlendMode::add);
                    for (int i=0;i<abs(frameUnpaused/2%8-4);i+=1)
                        buffer.drawText("START",0,48,96,16,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::average);
                }
                if (isReleased(0) || isReleased(1)) {
                    mode = INTRO_MODE_MENU;
                    //initializeLevel(DESTINATION_MAIN);
                }
                break;
            }

        }

        void init() {
            mode = INTRO_MODE_LOGO;
            selected = 0;
            buffer.setClearBackground(true, RGB565(0,0,0));
        }
    }
}

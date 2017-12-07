#include "game_intro.h"
#include "game_common.h"
#include "game_menu.h"

#define INTRO_MODE_LOGO 255
#define INTRO_MODE_CREDITS 254
#define INTRO_MODE_MENU 0
#define INTRO_MODE_PLAY 5
#define INTRO_MODE_LOAD 10

namespace Game {
    namespace Intro {
        uint8_t mode;
        uint8_t selected;
        uint8_t blockJoystick;
        uint16_t creditsAnim;

        static void menuNewGame() {
            mode = INTRO_MODE_PLAY;
        }
        static void menuTutorial() {
            initializeLevel(DESTINATION_TUTORIAL);
        }
        static void menuNormal() {
            initializeLevel(DESTINATION_MAIN);
        }
        static void menuCancelPlay() {
            mode = INTRO_MODE_MENU;
        }
        static void menuCancelLoad() {
            mode = INTRO_MODE_MENU;
        }
        static void menuLoadGame() {
            mode = INTRO_MODE_LOAD;
        }
        static void menuCredits() {
            mode = INTRO_MODE_CREDITS;
        }
        static void menuLoadGameState0() {
            mode = INTRO_MODE_PLAY;
        }
        static void menuLoadGameState1() {
            mode = INTRO_MODE_PLAY;
        }
        static void menuLoadGameState2() {
            mode = INTRO_MODE_PLAY;
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
            "\ncancel",
            0,
            "LOADING",
            "STATE 1",
            "STATE 2",
            "STATE 3",
            "\ncancel",
            0,
        };

        typedef void (*Action)();

        static Action menuActions[] = {
            0,
            menuNewGame,
            menuLoadGame,
            menuCredits,
            0,
            0,
            menuTutorial,
            menuNormal,
            menuCancelPlay,
            0,
            0,
            menuLoadGameState0,
            menuLoadGameState1,
            menuLoadGameState2,
            menuCancelLoad,

        };

        void putText(int &y, const char *tx, int space) {
            buffer.drawText(tx,8,y,80,8,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            y+=7+space;
        }

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
                actions[selected + 1]();
                selected = 0;
            }

        }

        void drawCredits(int16_t vpos) {
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
            if (isReleased(0) || isReleased(1))
            {
                mode = INTRO_MODE_MENU;
                selected = 0;
            }
        }

        void tick() {
            for(int i=0;i<1;i+=1) {
                buffer.setOffset(frameUnpaused * 2 + i,0);
                drawBackgrounds();
            }
            buffer.setOffset(0,0);
            switch (mode) {
            case INTRO_MODE_LOAD:
            case INTRO_MODE_PLAY:
            case INTRO_MODE_MENU:
                drawMenu(&menuTexts[mode], &menuActions[mode]);
                break;
            case INTRO_MODE_CREDITS:
                drawCredits(0);
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

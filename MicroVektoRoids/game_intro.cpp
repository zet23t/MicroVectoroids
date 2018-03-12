#include "game_intro.h"
#include "game_common.h"
#include "game_menu.h"
#include "game_player_stats.h"
#include "lib_sound.h"

#define INTRO_MODE_LOGO 255
#define INTRO_MODE_CREDITS 254
#define INTRO_MODE_MENU 0
#define INTRO_MODE_HIGHSCORES 6
#define INTRO_MODE_LOAD 10
#define INTRO_MODE_INPUTNAME_TRAINING 15
#define INTRO_MODE_INPUTNAME_NORMAL 16

namespace Game {
    namespace Intro {
        uint8_t mode;
        uint8_t selected;
        uint8_t blockJoystick;
        uint16_t creditsAnim;

        static void menuNewGame() {
            //mode = INTRO_MODE_PLAY;
            initializeLevel(DESTINATION_MAIN);
        }
        static void menuTutorial() {
            initializeLevel(DESTINATION_TUTORIAL);
        }
        static void menuHighScores() {
            mode = INTRO_MODE_HIGHSCORES;
        }
        static void menuCredits() {
            mode = INTRO_MODE_CREDITS;
        }

        const char *menuTexts[] = {
            "PLAY",
            "NEW GAME",
            "TUTORIAL",
            "HIGH SCORES",
            "CREDITS",
            0,
            "HIGH SCORES",
            0,
        };

        typedef void (*Action)();

        static Action menuActions[] = {
            0,
            menuNewGame,
            menuTutorial,
            menuHighScores,
            menuCredits,
            0,
            0,
            0,
        };
        void playMenuSelectSound() {
            static const int8_t b[] = {100,100,80,-80,-100,-100,-80,80};
            Sound::playSample(0,b, sizeof(b), 324,200,40)->setChange(600,-3,-1);
        }
        void playPushButtonSound() {
            static const int8_t b[] = {100,-100,80,-80,100,-100,80,-80};
            Sound::playSample(0,b, sizeof(b), 256,200,40)->setChange(120,-1,-1);
        }

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
                playMenuSelectSound();
            }
            if (!blockJoystick && stick.y<0) {
                blockJoystick= 5;
                selected = (selected - 1 + n) % n;
                playMenuSelectSound();
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
            putText(y,"Damien Labonte",0);
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
        void drawInputName() {
            static int8_t activeElement = 0;
            int y = 16;
            buffer.drawText("YOUR NAME IS",0,y,96,8,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            y+=14;
            Fixed2D4 stick = Joystick::getJoystick();
            for (int i=0;i<8;i+=1) {
                char c = PlayerStats::name[i];
                int x = i*8 - 8*4 + 48;
                buffer.drawRect(x-1,y-1,8,9)->filledRect(RGB565(20,80,255))->setDepth(200)
                    ->blend(activeElement==i && (frameUnpaused / 4 %2 == 0 ||stick.y != 0) ? RenderCommandBlendMode::opaque : RenderCommandBlendMode::average);
                if (activeElement == i) {
                    buffer.drawRect(x-0,y-2,6,1)->filledRect(RGB565(20,80,255))->setDepth(200);
                    buffer.drawRect(x+1,y-3,4,1)->filledRect(RGB565(20,80,255))->setDepth(200);
                    buffer.drawRect(x+2,y-4,2,1)->filledRect(RGB565(20,80,255))->setDepth(200);
                    buffer.drawRect(x-0,y+8,6,1)->filledRect(RGB565(20,80,255))->setDepth(200);
                    buffer.drawRect(x+1,y+9,4,1)->filledRect(RGB565(20,80,255))->setDepth(200);
                    buffer.drawRect(x+2,y+10,2,1)->filledRect(RGB565(20,80,255))->setDepth(200);

                }
                if (c > 32) {
                    buffer.drawText(stringBuffer.start().put(c).get(),x,y,8,8,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
                } else PlayerStats::name[i] = ' ';
            }
            y+=16;
            buffer.drawRect(0,y-1,32,9)->filledRect(RGB565(20,80,255))->setDepth(200)
                    ->blend(activeElement==-1 && frameUnpaused / 4 %2 == 0 ? RenderCommandBlendMode::opaque : RenderCommandBlendMode::average);

            buffer.drawRect(64,y-1,32,9)->filledRect(RGB565(20,80,255))->setDepth(200)
                    ->blend(activeElement==8 && frameUnpaused / 4 %2 == 0 ? RenderCommandBlendMode::opaque : RenderCommandBlendMode::average);
            buffer.drawText("< CANCEL",0,y,96,8,-1,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            buffer.drawText("START >",0,y,96,8,1,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
            if (activeElement < 9 && stick.x > 0 && !blockJoystick) {
                activeElement += 1;
                if (activeElement > 8) activeElement = -1;
                blockJoystick = true;
            }
            if (activeElement >= -1 && stick.x < 0 && !blockJoystick) {
                activeElement -= 1;
                if (activeElement < -1) activeElement = 8;
                blockJoystick = true;
            }
            if (activeElement >=0 && activeElement < 8 && !blockJoystick) {
                char c = PlayerStats::name[activeElement];
                bool up = stick.y > 0;
                if (stick.y < 0) c +=1, blockJoystick = 1;
                if (stick.y > 0) c -=1, blockJoystick = 1;

                if (c < ' ') c = 'z';
                else if (c > ' ' && c < '0') c = !up ? '0' : ' ';
                else if (c > '9' && c < 'A') c = !up ? 'A' : '9';
                else if (c > 'Z' && c < 'a') c = !up ? 'a' : 'Z';
                else if (c > 'z') c = ' ';
                PlayerStats::name[activeElement] = c;
            }
            if(stick.x == 0 && stick.y == 0) blockJoystick = false;
            if (isReleased(0) || isReleased(1)) {
                if (activeElement <= -1) {
                    //mode = INTRO_MODE_PLAY;
                }
                else if (activeElement >= 8) {
                    initializeLevel(mode == INTRO_MODE_INPUTNAME_NORMAL ? DESTINATION_MAIN : DESTINATION_TUTORIAL);
                }
                else {
                    if (isReleased(0)) {
                        activeElement = 8;
                    } else {
                        char c = PlayerStats::name[activeElement];
                        if (c >='A' && c <='Z') c = c - 'A' + 'a';
                        else if (c >='a' && c <='z') c = c + 'A' - 'a';
                        PlayerStats::name[activeElement]= c;
                    }
                }
            }
        }

        void tick() {
            for(int i=0;i<1;i+=1) {
                buffer.setOffset(frameUnpaused * 2 + i,0);
                drawBackgrounds();
            }
            if (isReleased(0) || isReleased(1)) {
                playPushButtonSound();
            }
            buffer.setOffset(0,0);
            switch (mode) {
            case INTRO_MODE_HIGHSCORES:
                if (isReleased(0) || isReleased(1)) {
                    mode = INTRO_MODE_MENU;
                    //initializeLevel(DESTINATION_MAIN);
                }
                {
                    int y = 30;
                    putText(y,"Not implemented\n(sorry)",2);
                }
                break;
            case INTRO_MODE_LOAD:
            case INTRO_MODE_MENU:
                drawMenu(&menuTexts[mode], &menuActions[mode]);
                break;
            case INTRO_MODE_INPUTNAME_NORMAL:
            case INTRO_MODE_INPUTNAME_TRAINING:
                drawInputName();
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

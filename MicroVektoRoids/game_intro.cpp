#include "game_intro.h"
#include "game_common.h"

#define INTRO_MODE_LOGO 0
#define INTRO_MODE_MENU 1

namespace Game {
    namespace Intro {
        uint8_t mode;
        uint8_t selected;

        void tick() {
            for(int i=0;i<1;i+=1) {
                buffer.setOffset(frameUnpaused * 2 + i,0);
                drawBackgrounds();
            }
            buffer.setOffset(0,0);
            switch (mode) {
            case INTRO_MODE_MENU:
                buffer.drawText("SELECT",0,12,96,16,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
                buffer.drawRect(12,19+6,96-24,1)->filledRect(0xffff);
                buffer.drawText("TUTORIAL",0,26,96,16,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
                buffer.drawText("NORMAL",0,36,96,16,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::opaque);
                break;
            case INTRO_MODE_LOGO:
                {
                    drawCenteredSprite(48,32,ImageAsset::TextureAtlas_atlas::logo.sprites[0])->blend(RenderCommandBlendMode::add);
                    for (int i=0;i<abs(frameUnpaused/2%8-4);i+=1)
                        buffer.drawText("START",0,48,96,16,0,0,false, FontAsset::font, 200, RenderCommandBlendMode::average);
                }
                break;
            }

            if (isReleased(0) || isReleased(1)) {
                mode = INTRO_MODE_MENU;
                //initializeLevel(DESTINATION_MAIN);
            }
        }

        void init() {
            mode = INTRO_MODE_LOGO;
            selected = 0;
            buffer.setClearBackground(true, RGB565(0,0,0));
        }
    }
}

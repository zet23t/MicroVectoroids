#include "game_common.h"
#include "game_ui_intermission.h"


namespace Game {
    namespace UI {
        namespace Intermission {
            const char *text;
            uint8_t isArmed;

            void show(const char *tx) {
                text = tx;
                isArmed = 0;
            }
            void tick() {
                if (!isArmed && text && (isPressed(0) || isPressed(1))) {
                    isArmed = 1;
                }
                if (isArmed && text && (isReleased(0) || isReleased(1))) {
                    text = 0;
                    isArmed = 0;
                }
            }
            bool isActive() {
                return text != 0;
            }
            void draw() {
                if (!text)return;
                int x = buffer.getOffsetX();
                int y = buffer.getOffsetY();
                buffer.setOffset(0,0);
                buffer.drawRect(0,47,96,1)->filledRect(RGB565(0,0,255))->setDepth(222)->blend(RenderCommandBlendMode::average);
                buffer.drawRect(0,47,96,17)->filledRect(RGB565(0,0,255))->setDepth(222)->blend(RenderCommandBlendMode::average);
                buffer.drawText(text, 4,48,88,16,0,0,true,FontAsset::font,222,RenderCommandBlendMode::opaque);
                buffer.setOffset(x,y);
            }
        }
    }
}

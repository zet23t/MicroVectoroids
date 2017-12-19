#include "game_common.h"
#include "game_ui_intermission.h"


namespace Game {
    namespace UI {
        namespace Intermission {
            const char *text;
            uint8_t isArmed;
            uint8_t type;

            void show(const char *tx, uint8_t typ) {
                text = tx;
                isArmed = type == INTERMISSION_TYPE_MODAL ? 0 : 90;
                type = typ;
            }
            void show(const char *tx) {
                show(tx,INTERMISSION_TYPE_MODAL);
            }
            void tick() {
                if (!text) return;
                if (type == INTERMISSION_TYPE_MODAL) {
                    if (!isArmed && text && (isPressed(0) || isPressed(1))) {
                        isArmed = 1;
                    }
                    if (isArmed && text && (isReleased(0) || isReleased(1))) {
                        text = 0;
                        isArmed = 0;
                    }
                } else {
                    if (isArmed > 0) {
                        isArmed -=1;
                    } else {
                        text = 0;
                    }
                }
            }
            bool isActive() {
                return text != 0;
            }
            bool isBlocking() {
                return text != 0 && type == INTERMISSION_TYPE_MODAL;
            }
            void draw() {
                if (!text)return;
                int x = buffer.getOffsetX();
                int y = buffer.getOffsetY();
                buffer.setOffset(0,0);
                if (type == INTERMISSION_TYPE_MODAL) {
                    buffer.drawRect(0,47,96,1)->filledRect(RGB565(0,0,255))->setDepth(222)->blend(RenderCommandBlendMode::average);
                    buffer.drawRect(0,47,96,17)->filledRect(RGB565(0,0,255))->setDepth(222)->blend(RenderCommandBlendMode::average);
                    buffer.drawText(text, 4,48,88,16,0,0,true,FontAsset::font,222,RenderCommandBlendMode::opaque);
                }
                else {

                    buffer.drawText(text, 4,48,88,16,0,0,true,FontAsset::font,222,RenderCommandBlendMode::opaque);
                }
                buffer.setOffset(x,y);
            }
        }
    }
}

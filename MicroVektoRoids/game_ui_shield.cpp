#include "game_ui_shield.h"
#include "game_ships.h"
#include "game_common.h"

namespace Game {
    namespace UI {
        namespace Shield {
            void draw() {
                buffer.setOffset(0,0);
                const SpriteSheet* bar = &ImageAsset::TextureAtlas_atlas::ui_shield_bar;
                const SpriteSheet* inner = &ImageAsset::TextureAtlas_atlas::ui_shield_bar_inner;
                SpriteSheetRect rect = bar->sprites[0];
                SpriteSheetRect innerRect = inner->sprites[0];
                uint8_t y = 32-rect.origHeight/2;
                int8_t x = -3;
                buffer.drawRect(x+rect.offsetX,y + rect.offsetY,rect.width,rect.height)->sprite(&atlas, rect.x, rect.y)->blend(RenderCommandBlendMode::add);

                uint8_t dmg = shipManager.ships[0].damage * 35 / 256;
                buffer.setClipping(y+7+dmg,96,64,0);
                buffer.drawRect(x+innerRect.offsetX,y + innerRect.offsetY,innerRect.width,innerRect.height)->sprite(&atlas, innerRect.x, innerRect.y)->blend(RenderCommandBlendMode::add);
                buffer.setClipping(0,96,64,0);
                buffer.setOffset(camX-48,camY-32);
            }
        }
    }
}

#include "game_ui_shield.h"
#include "game_ships.h"
#include "game_common.h"
#include "game_ui_hud.h"

namespace Game {
    namespace UI {
        namespace Shield {

            static void drawBar(int8_t x, uint8_t dmgAbs, uint8_t maxDmg) {
                const SpriteSheet* bar = &ImageAsset::TextureAtlas_atlas::ui_shield_bar;
                const SpriteSheet* inner = &ImageAsset::TextureAtlas_atlas::ui_shield_bar_inner;
                SpriteSheetRect rect = bar->sprites[0];
                SpriteSheetRect innerRect = inner->sprites[0];
                uint8_t y = 32-rect.origHeight/2;
                buffer.drawRect(x+rect.offsetX,y + rect.offsetY,rect.width,rect.height)->sprite(&atlas, rect.x, rect.y)->blend(RenderCommandBlendMode::add)->setDepth(100);
                uint8_t dmg = dmgAbs * 35 / (maxDmg + 1);
                buffer.setClipping(y+7+dmg,96,64,0);
                buffer.drawRect(x+innerRect.offsetX,y + innerRect.offsetY,innerRect.width,innerRect.height)->sprite(&atlas, innerRect.x, innerRect.y)->blend(RenderCommandBlendMode::add)->setDepth(100);
                buffer.setClipping(0,96,64,0);

            }
            void draw() {
                buffer.setOffset(0,0);
                drawBar(-3, shipManager.ships[0].damage, shipManager.ships[0].maxDamage());
                if (Game::UI::HUD::targetMaxDamage > 0 && Game::UI::HUD::targetLock)
                    drawBar(82, Game::UI::HUD::targetDamage, Game::UI::HUD::targetMaxDamage);

                buffer.setOffset(camX-48,camY-32);
            }
        }
    }
}

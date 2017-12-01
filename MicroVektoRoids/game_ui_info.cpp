#include "game_ui_info.h"
#include "game_common.h"
#include "game_player_stats.h"

namespace Game {
    namespace UI {
        namespace Info {
            uint32_t displayedScore;

            void tick() {
                if (frame % 2 == 0) {
                    displayedScore = displayedScore / 2 + PlayerStats::score / 2;
                    if (displayedScore < PlayerStats::score) displayedScore +=1;
                    if (displayedScore > PlayerStats::score) displayedScore -=1;
                }
            }

            void draw() {
                if (displayedScore == 0) return;
                int x = buffer.getOffsetX();
                int y = buffer.getOffsetY();
                buffer.setOffset(0,0);
                buffer.drawText(stringBuffer.start().putDec(displayedScore).get(), 0,2,96,16,0,-1,false,FontAsset::numbersfont,100,RenderCommandBlendMode::add);
                buffer.setOffset(x,y);
            }

            void init() {
                displayedScore = 0;
            }
        }
    }
}

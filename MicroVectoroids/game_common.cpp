#include "game_common.h"

TinyScreen display = TinyScreen(TinyScreenPlus);
RenderBuffer<uint16_t,RENDER_COMMAND_COUNT> buffer;
TileMap::SceneRenderer<uint16_t,RENDER_COMMAND_COUNT> renderer;
TileMap::Scene<uint16_t> tilemap;


namespace Time {
    uint32_t millis;
}

namespace Game {
    void tick() {
        buffer.drawRect(0,0,4,4)->filledRect(RGB565(255,0,0));
    }
}

#ifndef __GAME_COMMON_H__
#define __GAME_COMMON_H__

// common library includes
#include "lib_FixedMath.h"
#include "lib_input.h"
#include "lib_RenderBuffer.h"
#include "lib_StringBuffer.h"
#include "lib_spritefont.h"
#include "lib_tilemap.h"

// general assets needed
#include "asset_tilemap.h"
#include "image_data.h"
#include "font_asset.h"

// game modules
#include "game_particles.h"

#define RENDER_COMMAND_COUNT 200

#define Fix4 FixedNumber16<4u>

// a bunch of global variables that shouldn't exist in this way (sorry)
extern TinyScreen display;
extern RenderBuffer<uint16_t,RENDER_COMMAND_COUNT> buffer;
extern TileMap::SceneRenderer<uint16_t,RENDER_COMMAND_COUNT> renderer;

namespace Time {
    extern uint32_t millis;
}

class Camera {
public:
    Fixed2D4 trackPoint;
    Fixed2D4 pos;

    void setPos(int x, int y) {
        trackPoint.x.setIntegerPart(x-48);
        trackPoint.y.setIntegerPart(y-32);
        pos = trackPoint;
    }
};
class Rect {
public:
    int16_t x1,y1,x2,y2;
    void init(int16_t x1, int16_t y1, int16_t x2, int16_t y2) {
        this->x1 = x1;
        this->x2 = x2;
        this->y1 = y1;
        this->y2 = y2;
    }
    void print() {
        #ifdef WIN32
        printf("%d %d %d %d\n",x1,y1,x2,y2);
        #endif
    }
};

namespace Game {
    extern Texture<uint16_t> atlas;
    extern ParticleSystem particleSystem;

    void initialize();
    void tick();
}

#endif // __GAME_COMMON_H__

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



#define RENDER_COMMAND_COUNT 512

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

// destinations > 32 have no high score tracking
#define DESTINATION_TUTORIAL 0
#define DESTINATION_01 1
#define DESTINATION_02 2
#define DESTINATION_MAIN 100
#define DESTINATION_03a 3
#define DESTINATION_03b 4
#define DESTINATION_03c 5
#define DESTINATION_INTRO 222
#define DESTINATION_HIGHSCORE 223

namespace Game {
    namespace GameState {
        extern const uint8_t Running;
        extern const uint8_t Menu;
    }
    extern bool showDebugInfo;
    extern uint8_t gameState;
    extern Texture<uint16_t> atlas;
    extern int frame, frameUnpaused;
    extern int16_t camX, camY;
    extern uint8_t currentLevelId;

    void checkLevelTermination(int16_t x, int16_t y);

    void initializeLevel(uint8_t id);
    void setScreenBrightness(uint8_t b);
    uint8_t getScreenBrightness();
    void initialize();
    void tick();
    void drawBackgrounds();
    bool isPressed(int id);
    bool isReleased(int id);

    void finishGameOver();
    RenderCommand<uint16_t>* drawCenteredSprite(int x,int y,SpriteSheetRect rect);
    RenderCommand<uint16_t>* drawSprite(int x,int y,SpriteSheetRect rect);
}

#endif // __GAME_COMMON_H__

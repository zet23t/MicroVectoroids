#include "game_ships.h"
#include "game_common.h"

namespace Game {
void Ship::tick() {
        prevPos = pos;
        pos += velocity;
        velocity = velocity * (Fix4(0,15));
        if (velocity.x.absolute() < Fix4(0,3)) velocity.x = 0;
        if (velocity.y.absolute() < Fix4(0,3)) velocity.y = 0;

        Fixed2D4 dir = pos - prevPos;
        Fix4 dist = (dir).length();
        if (dist > 0) {
                Fix4 v = Fix4(1,12)/dist;
                dir.scale(v);
        } else return;
        Fixed2D4 p = pos - direction * 6;
        for (Fix4 f = 0; f<=dist + Fix4(0,8); f+=Fix4(1,0)) {

            particleSystem.spawn(1,p,Fixed2D4());
            p -= dir;
        }
    }
    void Ship::draw() {
        const int pivotX = -8;
        const int pivotY = -8;
        FixedNumber16<4> dirModY = direction.y;
        // making up for missing asin transformation (for converting dot to angle) - rotation ain't linear
        if (dirModY < 0 && dirModY.getFractionPart() > 2 && dirModY.getFractionPart() < 7) dirModY += FixedNumber16<4>(0,3);
        if (dirModY < 0 && dirModY.getFractionPart() > 11 && dirModY.getFractionPart() < 13) dirModY -= FixedNumber16<4>(0,1);
        if (dirModY > 0 && dirModY.getFractionPart() > 2 && dirModY.getFractionPart() < 9) dirModY -= FixedNumber16<4>(0,3);
        if (dirModY > 0 && dirModY.getFractionPart() > 8 && dirModY.getFractionPart() < 13) dirModY -= FixedNumber16<4>(0,1);
        int idir = 8-((dirModY + FixedNumber16<4>(1,0)) * FixedNumber16<4>(4,0)).getIntegerPart();
        if (idir < 0) idir = 0;
        if (idir > 8) idir = 8;
        if (direction.x < 0) idir = 16 - idir;

        SpriteSheetRect rect = ImageAsset::TextureAtlas_atlas::ship_triangle.sprites[(idir + 8) % 16];

        int w = rect.width, h = rect.height;
        buffer.drawRect(pos.x.getIntegerPart() + rect.offsetX + pivotX,pos.y.getIntegerPart() + rect.offsetY + pivotY,w,h)->sprite(&atlas,rect.x,rect.y)->blend(RenderCommandBlendMode::add);
//
//        buffer.drawRect(pos.x.getIntegerPart(),pos.y.getIntegerPart(),4,4)->filledRect(RGB565(255,0,0));
    }
    void Ship::init(int8_t type, int16_t x, int16_t y, int8_t dx, int8_t dy) {
        this->type = type;
        pos.setXY(x,y);
        prevPos = pos;
        direction.x = Fix4(0,dx);
        direction.y = Fix4(0,dy);
    }

    void ShipManager::tick() {
        for (int i=0;i<ShipCount;i+=1) {
            if (ships[i].type) ships[i].tick();
        }
    }
    void ShipManager::draw() {
        for (int i=0;i<ShipCount;i+=1) {
            if (ships[i].type) ships[i].draw();
        }
    }
}

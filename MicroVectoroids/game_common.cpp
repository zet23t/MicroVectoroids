#include "game_common.h"

TinyScreen display = TinyScreen(TinyScreenPlus);
RenderBuffer<uint16_t,RENDER_COMMAND_COUNT> buffer;
TileMap::SceneRenderer<uint16_t,RENDER_COMMAND_COUNT> renderer;
TileMap::Scene<uint16_t> tilemap;


namespace Time {
    uint32_t millis;
}

namespace Game {
    const uint8_t ParticleCount = 64;
    Texture<uint16_t> atlas;


    struct Particle {
        Fixed2D4 pos,vel;
        uint8_t type, age;
        void tick();
        void draw();
        void init(uint8_t type, Fixed2D4 pos, Fixed2D4 vel);
    };
    struct ParticleSystem {
        Particle particles[ParticleCount];
        void tick();
        void draw();
        Particle* spawn(uint8_t type, Fixed2D4 pos, Fixed2D4 vel);
    };

    void Particle::draw() {
        int sel = age * ImageAsset::TextureAtlas_atlas::ship_exhaust.spriteCount / 5;
        SpriteSheetRect rect = ImageAsset::TextureAtlas_atlas::ship_exhaust.sprites[sel];
        int w = rect.width, h = rect.height;
        buffer.drawRect(pos.x.getIntegerPart() + rect.offsetX - w/2 ,pos.y.getIntegerPart() + rect.offsetY - h/2,w,h)->sprite(&atlas,rect.x,rect.y)->blend(RenderCommandBlendMode::bitwiseOr);
//            ->blend(RenderCommandBlendMode::add)->filledRect(RGB565(255,255,32));
    }
    void Particle::init(uint8_t type, Fixed2D4 pos, Fixed2D4 vel) {
        this->type = type;
        this->pos = pos;
        this->vel = vel;
        age = 0;
    }
    void Particle::tick() {
        age +=1;
        if (age > 5) type = 0;
    }
    void ParticleSystem::draw() {
        for (int i=0;i<ParticleCount;i+=1) {
            if (particles[i].type) particles[i].draw();
        }
    }
    void ParticleSystem::tick() {
        for (int i=0;i<ParticleCount;i+=1) {
            if (particles[i].type) particles[i].tick();
        }
    }
    Particle* ParticleSystem::spawn(uint8_t type, Fixed2D4 pos, Fixed2D4 vel) {
        for (int i=0;i<ParticleCount;i+=1) {
            if (particles[i].type == 0){
                particles[i].init(type,pos,vel);
                return &particles[i];
            }
        }
        static Particle def;
        return &def;
    }

    ParticleSystem particleSystem;

    const uint8_t ShipCount = 64;
    struct Ship {
        Fixed2D4 pos;
        Fixed2D4 prevPos;
        Fixed2D4 direction;
        Fixed2D4 velocity;
        uint8_t type;
        uint8_t hitPoints;
        void tick();
        void draw();
        void init(int8_t type, int16_t x, int16_t y, int8_t dx, int8_t dy);
    };

    struct ShipManager {
        Ship ships[ShipCount];
        void tick();
        void draw();
    };

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

    ShipManager shipManager;

    void drawSpaceBackground(int layer, uint16_t col) {
        int16_t x = buffer.getOffsetX();
        int16_t y = buffer.getOffsetY();
        int16_t mask= (64 << layer)-1;
        int16_t ox = x & ~mask;
        int16_t oy = y & ~mask;
        buffer.setOffset(x>>layer,y>>layer);
        for (int8_t i=0; i<3;i+=1) {
            for (int8_t j=0;j<2;j+=1) {
                int16_t xx = ox + (i << (6 + layer));
                int16_t yy = oy + (j << (6 + layer));
                Math::setSeed(xx + layer * 17,yy + layer * 7);
                for (int8_t k = 0;k<8;k+=1) {
                    int16_t px = xx + (Math::randInt()&mask);
                    int16_t py = yy + (Math::randInt()&mask);
                    buffer.drawRect(px>>layer,py>>layer,1,1)->filledRect(col);
                }
            }
        }
        buffer.setOffset(x,y);
        //buffer.drawRect(ox,oy,64,64)->filledRect(RGB565(0,255,0));
    }

    void tick() {

        Ship* ship = shipManager.ships;
        Fixed2D4 input =  Joystick::getJoystick();
        if (input.x !=0 || input.y !=0) {
            Fix4 force = Fix4(0,8);
            Fix4 dot = ship->direction.dot(input);
            if (dot < 0) {
                Fixed2D4 right = input.right();
                Fixed2D4 left = input.left();
                Fix4 leftDot = left.dot(ship->direction);
                Fix4 rightDot = right.dot(ship->direction);
                input = leftDot > rightDot ? left : right;
            }
            //force = Fix4(3,0);
            ship->direction += input * force;
            ship->direction.normalize();
        }
        if (Joystick::getButton(0)) {
            ship->velocity += ship->direction * Fix4(0,12);
        }
        shipManager.tick();
        particleSystem.tick();
        Fixed2D4 cam = ship->pos + ship->direction * 6;
        buffer.setOffset(cam.x.getIntegerPart() - 48, cam.y.getIntegerPart() -32);

        drawSpaceBackground(0, RGB565(62,62,62));
        drawSpaceBackground(1, RGB565(180,180,180));
        drawSpaceBackground(2, RGB565(120,120,120));
        shipManager.draw();
        particleSystem.draw();
    }

    void initialize(){
        atlas = Texture<uint16_t>(ImageAsset::atlas);
        const Ship* ship = shipManager.ships;
        shipManager.ships[0].init(1,10,5,15,0);
        shipManager.ships[1].init(1,15,8,15,0);
        buffer.setClearBackground(true, RGB565(0,0,0));
    }
}

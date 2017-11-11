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
        void spawn(uint8_t type, Fixed2D4 pos, Fixed2D4 vel);
    };

    void Particle::draw() {
        buffer.drawRect(pos.x.getIntegerPart(),pos.y.getIntegerPart(),4,4)->filledRect(RGB565(255,255,0));
    }
    void Particle::init(uint8_t type, Fixed2D4 pos, Fixed2D4 vel) {
        this->type = type;
        this->pos = pos;
        this->vel = vel;
        age = 0;
    }
    void Particle::tick() {
        age +=1;
        if (age > 10) type = 0;
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
    void ParticleSystem::spawn(uint8_t type, Fixed2D4 pos, Fixed2D4 vel) {
        for (int i=0;i<ParticleCount;i+=1) {
            if (particles[i].type == 0){
                particles[i].init(type,pos,vel);
                return;
            }
        }
    }

    ParticleSystem particleSystem;

    const uint8_t ShipCount = 64;
    struct Ship {
        Fixed2D4 pos;
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
        pos += velocity;
        velocity = velocity * (Fix4(0,15));
        if (velocity.x.absolute() < Fix4(0,3)) velocity.x = 0;
        if (velocity.y.absolute() < Fix4(0,3)) velocity.y = 0;
        particleSystem.spawn(1,pos,Fixed2D4());
    }
    void Ship::draw() {
        buffer.drawRect(pos.x.getIntegerPart(),pos.y.getIntegerPart(),4,4)->filledRect(RGB565(255,0,0));
    }
    void Ship::init(int8_t type, int16_t x, int16_t y, int8_t dx, int8_t dy) {
        this->type = type;
        pos.setXY(x,y);
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
        ship->direction += input * Fix4(1,1);
        ship->direction.normalize();
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
        const Ship* ship = shipManager.ships;
        shipManager.ships[0].init(1,10,5,15,0);
        shipManager.ships[1].init(1,15,8,15,0);
        buffer.setClearBackground(true, RGB565(0,0,0));
    }
}

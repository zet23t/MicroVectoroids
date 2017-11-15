#include "game_common.h"
#include "game_particles.h"

namespace Game {
void Particle::draw() {
        int sel = age * ImageAsset::TextureAtlas_atlas::ship_exhaust.spriteCount / 5;
        if (sel >= ImageAsset::TextureAtlas_atlas::ship_exhaust.spriteCount) sel = ImageAsset::TextureAtlas_atlas::ship_exhaust.spriteCount - 1;
        SpriteSheetRect rect = ImageAsset::TextureAtlas_atlas::ship_exhaust.sprites[sel];
        int w = rect.width, h = rect.height;
        buffer.drawRect(pos.x.getIntegerPart() + rect.offsetX - rect.origWidth/2 ,pos.y.getIntegerPart() + rect.offsetY - rect.origHeight/2,w,h)->sprite(&atlas,rect.x,rect.y)->blend(RenderCommandBlendMode::bitwiseOr);
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
}

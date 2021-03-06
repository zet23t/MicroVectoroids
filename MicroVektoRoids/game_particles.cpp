#include "game_common.h"
#include "game_particles.h"

namespace Game {

    ParticleSystem particleSystem;

    namespace ParticleType {
        const uint8_t PlayerShipTrail = 1;
        const uint8_t PlayerShot = 2;
        const uint8_t PlayerShotImpact = 3;
        const uint8_t AsteroidFragmentWhite = 4;
        const uint8_t EnemyShot = 5;
        const uint8_t EnemyShotImpact = 6;
        const uint8_t EnemyShipTrail = 7;
        const uint8_t PlayerShipExplosion = 8;
        const uint8_t EnemyShipExplosion = 9;
        const uint8_t WormHoleParticle = 10;
    }

    const SpriteSheet* sheetByType[] = {
        0,
        &ImageAsset::TextureAtlas_atlas::ship_exhaust,
        &ImageAsset::TextureAtlas_atlas::player_shot,
        &ImageAsset::TextureAtlas_atlas::player_shot,
        &ImageAsset::TextureAtlas_atlas::ship_asteroid_fragment_white,
        &ImageAsset::TextureAtlas_atlas::enemy_shot,
        &ImageAsset::TextureAtlas_atlas::enemy_shot,
        &ImageAsset::TextureAtlas_atlas::ship_enemy_exhaust,
        &ImageAsset::TextureAtlas_atlas::ship_exhaust,
        &ImageAsset::TextureAtlas_atlas::ship_enemy_exhaust,
        &ImageAsset::TextureAtlas_atlas::wormhole_particle,
    };

    const uint8_t maxAgeByType[] {
        0,5,2,5,10,2,5,5,20,20,9,
    };

    void Particle::draw() {
        const SpriteSheet *sheet = sheetByType[type];
        int maxAge = maxAgeByType[type];
        int sel = age * sheet->spriteCount / maxAge;
        if (sel >= sheet->spriteCount) sel = sheet->spriteCount - 1;
        SpriteSheetRect rect = sheet->sprites[sel];
        int w = rect.width, h = rect.height;
        buffer.drawRect(pos.x.getIntegerPart() + rect.offsetX - rect.origWidth/2 ,pos.y.getIntegerPart() + rect.offsetY - rect.origHeight/2,w,h)->sprite(&atlas,rect.x,rect.y)->blend(RenderCommandBlendMode::bitwiseOr);
        if (type == ParticleType::PlayerShotImpact) {
            particleSystem.spawn(ParticleType::PlayerShot,pos,Fixed2D4(0,0));
        }
        if (type == ParticleType::EnemyShotImpact) {
            particleSystem.spawn(ParticleType::EnemyShot,pos,Fixed2D4(0,0));
        }
//            ->blend(RenderCommandBlendMode::add)->filledRect(RGB565(255,255,32));
    }
    void Particle::init(uint8_t type, Fixed2D4 pos, Fixed2D4 vel) {
        this->type = type;
        this->pos = pos;
        this->vel = vel;
        age = 0;
    }
    void Particle::tick() {
        int maxAge = maxAgeByType[type];
        age +=1;
        pos += vel;
        if (age > maxAge) type = 0;
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
        static Particle def;
        int16_t dx = pos.x.getIntegerPart() - camX, dy = pos.y.getIntegerPart()-camY;
        if (dx < -56 || dx > 56 || dy < -40 || dy > 40) return &def;
        for (int i=0;i<ParticleCount;i+=1) {
            if (particles[i].type == 0){
                particles[i].init(type,pos,vel);
                return &particles[i];
            }
        }
        return &def;
    }
}

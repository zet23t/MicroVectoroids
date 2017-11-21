#include "game_asteroids.h"
#include "game_projectile.h"
#include "game_common.h"
#include "game_particles.h"
#include "game_collectable.h"
#include "game_balancing.h"

namespace Game {
    AsteroidManager asteroidManager;

    namespace AsteroidType {
        const uint8_t White = 1;
        const uint8_t Red = 2;
        const uint8_t Yellow = 3;
        const uint8_t WhiteSmall = 4;
    }
    const SpriteSheet* spriteSheetByType[] = {
        0,
        &ImageAsset::TextureAtlas_atlas::ship_asteroid,
        &ImageAsset::TextureAtlas_atlas::ship_asteroid,
        &ImageAsset::TextureAtlas_atlas::ship_asteroid,
        &ImageAsset::TextureAtlas_atlas::ship_asteroid_small
    };
    const uint8_t asteroidRadiusByType[] = {
        0,14,17,17, 7
    };
    const uint16_t asteroidRadiusSqByType[] = {
        (uint16_t)(asteroidRadiusByType[0] * asteroidRadiusByType[0]),
        (uint16_t)(asteroidRadiusByType[1] * asteroidRadiusByType[1]),
        (uint16_t)(asteroidRadiusByType[2] * asteroidRadiusByType[2]),
        (uint16_t)(asteroidRadiusByType[3] * asteroidRadiusByType[3]),
        (uint16_t)(asteroidRadiusByType[4] * asteroidRadiusByType[4]),
    };

    const uint8_t maxHitsByType[] = {
        0,5,5,5,2
    };

    void Asteroid::init(uint8_t type, int16_t x, int16_t y) {
        this->type = type;
        pos = Fixed2D4(x, y);
        velocity = Fixed2D4(0,0);
        hits = 0;
    }

    void Asteroid::draw() {
        const SpriteSheet* sheet = spriteSheetByType[type];
        SpriteSheetRect rect = sheet->sprites[(frame >> 1) % sheet->spriteCount];
        int w = rect.width, h = rect.height;
        buffer.drawRect(pos.x.getIntegerPart() + rect.offsetX - rect.origWidth/2 ,pos.y.getIntegerPart() + rect.offsetY - rect.origHeight/2,w,h)->sprite(&atlas,rect.x,rect.y)->blend(RenderCommandBlendMode::add);
    }

    void Asteroid::tick() {
        int16_t rad = asteroidRadiusByType[type];
        int16_t rad2 = asteroidRadiusSqByType[type];
        for (int i=0;i<ProjectileMaxCount;i+=1) {
            Projectile* p = &projectileManager.projectiles[i];
            if (p->type) {
                Fixed2D4 dir = p->pos - pos;
                int16_t dx = dir.x.getIntegerPart();
                int16_t dy = dir.y.getIntegerPart();

                if (abs(dx) < rad && abs(dy) < rad && dx*dx+dy*dy < rad2) {
                    p->impact();
                    hits+=1;
                    if (hits > maxHitsByType[type]) {
                        destroy();
                        return;
                    }
                }
            }
        }
        pos += velocity;
    }

    void Asteroid::push(Fixed2D4 change) {
        velocity+=change * Fix4(0,4);
    }

    void Asteroid::destroy() {
        if (type == AsteroidType::White) {
            int x = pos.x.getIntegerPart(), y = pos.y.getIntegerPart();
            for (int i=0;i<5;i+=1) {
                int8_t dx = Math::randInt() % 8 - 4;
                int8_t dy = Math::randInt() % 8 - 4;
                Asteroid* a = asteroidManager.spawn();
                a->init(AsteroidType::WhiteSmall,x+dx,y+dy);
                a->velocity.x = Fix4(dx < 0 ? -1 : 0,dx * 4);
                a->velocity.y = Fix4(dy < 0 ? -1 : 0,dy * 4);
                a->velocity += velocity;
            }
        }
        if (type == AsteroidType::White || type == AsteroidType::WhiteSmall) {
            for (int i=0;i<18;i+=1) {
                Fixed2D4 vel;
                vel = vel.randomCircle(Fix4(2,5));
                Particle *p = particleSystem.spawn(ParticleType::AsteroidFragmentWhite, pos, vel);
                p->age = Math::randInt() % 4;
            }
            if (type == AsteroidType::WhiteSmall) {
                for (int i=0;i<5;i+=1) {
                    Fixed2D4 vel;
                    vel = vel.randomCircle(ASTEROIDS_DUST_EJECTION_POWER);
                    Collectable::spawn(pos,vel);
                }
            }
        }
        type = 0;
    }

    void AsteroidManager::init() {
        memset(asteroids,0,sizeof(asteroids));
    }

    void AsteroidManager::draw() {
        for (int i=0;i<AsteroidsCount;i+=1) {
            if (asteroids[i].type) asteroids[i].draw();
        }
    }

    void AsteroidManager::tick() {
        for (int i=0;i<AsteroidsCount;i+=1) {
            if (asteroids[i].type) asteroids[i].tick();
        }
    }

    Asteroid* AsteroidManager::spawn() {
        for (int i=0;i<AsteroidsCount;i+=1) {
            if (!asteroids[i].type) return &asteroids[i];
        }
        static Asteroid def;
        return &def;
    }
}

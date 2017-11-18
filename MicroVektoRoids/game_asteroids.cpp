#include "game_asteroids.h"
#include "game_projectile.h"
#include "game_common.h"

namespace Game {
    AsteroidManager asteroidManager;

    namespace AsteroidType {
        const uint8_t White = 1;
        const uint8_t Red = 2;
        const uint8_t Yellow = 3;
    }

    const uint8_t asteroidRadiusByType[] = {
        0,14,17,17
    };
    const uint16_t asteroidRadiusSqByType[] = {
        (uint16_t)(asteroidRadiusByType[0] * asteroidRadiusByType[0]),
        (uint16_t)(asteroidRadiusByType[1] * asteroidRadiusByType[1]),
        (uint16_t)(asteroidRadiusByType[2] * asteroidRadiusByType[2]),
        (uint16_t)(asteroidRadiusByType[3] * asteroidRadiusByType[3]),
    };

    void Asteroid::init(uint8_t type, int16_t x, int16_t y) {
        this->type = type;
        pos = Fixed2D4(x, y);
        velocity = Fixed2D4(0,0);
    }

    void Asteroid::draw() {
        SpriteSheetRect rect = ImageAsset::TextureAtlas_atlas::ship_asteroid.sprites[
            (frame >> 1) % ImageAsset::TextureAtlas_atlas::ship_asteroid.spriteCount];
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
                    if (hits > 5) {
                        type = 0;
                        return;
                    }
                }
            }
        }
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

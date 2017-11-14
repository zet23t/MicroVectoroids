#include "game_asteroids.h"
#include "game_common.h"

namespace Game {

    void Asteroid::init(uint8_t type, int16_t x, int16_t y) {
        this->type = type;
        pos = Fixed2D4(x, y);
        velocity = Fixed2D4(0,0);
    }

    void Asteroid::draw() {
        SpriteSheetRect rect = ImageAsset::TextureAtlas_atlas::ship_asteroid.sprites[
            (frame >> 1) % ImageAsset::TextureAtlas_atlas::ship_asteroid.spriteCount];
        int w = rect.width, h = rect.height;
        buffer.drawRect(pos.x.getIntegerPart() + rect.offsetX - w/2 ,pos.y.getIntegerPart() + rect.offsetY - h/2,w,h)->sprite(&atlas,rect.x,rect.y)->blend(RenderCommandBlendMode::add);
    }

    void Asteroid::tick() {
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

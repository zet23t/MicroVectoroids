#include "game_asteroids.h"

namespace Game {

    void Asteroid::init(uint8_t type, int16_t x, int16_t y) {
        this->type = type;
        pos = Fixed2D4(x, y);
        velocity = Fixed2D4(0,0);
    }

    void Asteroid::draw() {
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

}

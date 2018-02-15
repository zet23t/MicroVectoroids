#include "game_level_main.h"
#include "game_ships.h"
#include "game_asteroids.h"
#include "game_common.h"
#include "game_player_stats.h"
#include "game_ui_intermission.h"

static void addWormhole(uint8_t &id, int16_t x, int16_t y, const char *inf, uint8_t dest, bool active) {
    Game::shipManager.ships[id++].init(active ? ShipTypeWormHole : ShipTypeWormHoleInactive,x,y,inf, dest);
}

namespace Game {
    namespace Level {
        void spawnAsteroids(uint8_t n, uint8_t t, int x, int y, int maxRange, int minRange, int maxVel) {
            int min2 = minRange * minRange;
            int max2 = maxRange * maxRange;
            int vel2 = maxVel * maxVel;
            while (n > 0) {
                int x = (Math::randInt() % (maxRange * 2)) - maxRange;
                int y = (Math::randInt() % (maxRange * 2)) - maxRange;
                int d = x*x+y+y;
                if (d > min2 && d < max2) {
                    n--;
                    Asteroid *a = asteroidManager.spawn();
                    a->init(t,x,y);
                    while (maxVel > 0) {
                        int vx = (Math::randInt() % (maxVel * 2)) - maxVel;
                        int vy = (Math::randInt() % (maxVel * 2)) - maxVel;
                        int dv = vx*vx + vy*vy;
                        if (dv <= vel2) {
                            a->velocity.x = vx;
                            a->velocity.y = vy;
                            break;
                        }
                    }
                }
            }
        }


        namespace L03a {
            uint8_t state;
            void init () {
                spawnAsteroids(3, AsteroidType::White,0,0,250,80,0);
                spawnAsteroids(4, AsteroidType::WhiteSmall,0,0,250,80,0);

                state = 0;
                uint8_t id = 1;
                addWormhole(id, -15,20,"W-HOME", DESTINATION_MAIN, PlayerStats::hasVisited(DESTINATION_03a));
            }
            void tick() {
                if (state == 0 && asteroidManager.countType(AsteroidType::White) < 2) {
                    state = 1;
                    for (int i=0;i<3;i+=1) {
                        int x = (Math::randInt() % 512) - 256;
                        int y = (Math::randInt() % 512) - 256;
                        if (x*x+y*y > 1024){

                            shipManager.ships[i+4].init(3,x,y,15,0,0);
                            shipManager.ships[i+4].aiStrength = 3;
                            shipManager.ships[i+4].pos += shipManager.ships[0].pos;
                        }
                    }
                    UI::Intermission::show("INTRUDERS DETECTED", INTERMISSION_TYPE_OVERLAY);
                }
                if (state == 1 && asteroidManager.countAll() < 3) {
                    state = 2;
                    for (int i=0;i<3;i+=1) {
                        int x = (Math::randInt() % 512) - 256;
                        int y = (Math::randInt() % 512) - 256;
                        if (x*x+y*y > 1024){

                            shipManager.ships[i+4].aiStrength = 4;
                            shipManager.ships[i+10].init(3,x,y,15,0,0);
                            shipManager.ships[i+10].pos += shipManager.ships[0].pos;
                        }
                    }
                    UI::Intermission::show("INTRUDERS DETECTED", INTERMISSION_TYPE_OVERLAY);
                }
            }
        }


        namespace L03b {
            uint8_t state;
            void init () {
                spawnAsteroids(3, AsteroidType::White,0,0,250,80,0);
                spawnAsteroids(4, AsteroidType::WhiteSmall,0,0,250,80,0);

                state = 0;
                uint8_t id = 1;
                addWormhole(id, -15,20,"W-HOME", DESTINATION_MAIN, PlayerStats::hasVisited(DESTINATION_03b));
            }
            void tick() {
                if (state == 0 && asteroidManager.countType(AsteroidType::White) < 2) {
                    state = 1;
                    for (int i=0;i<3;i+=1) {
                        int x = (Math::randInt() % 512) - 256;
                        int y = (Math::randInt() % 512) - 256;
                        if (x*x+y*y > 1024){

                            shipManager.ships[i+4].init(3,x,y,15,0,0);
                            shipManager.ships[i+4].aiStrength = 3;
                            shipManager.ships[i+4].pos += shipManager.ships[0].pos;
                        }
                    }
                    UI::Intermission::show("INTRUDERS DETECTED", INTERMISSION_TYPE_OVERLAY);
                }
                if (state == 1 && asteroidManager.countAll() < 3) {
                    state = 2;
                    for (int i=0;i<3;i+=1) {
                        int x = (Math::randInt() % 512) - 256;
                        int y = (Math::randInt() % 512) - 256;
                        if (x*x+y*y > 1024){

                            shipManager.ships[i+4].aiStrength = 4;
                            shipManager.ships[i+10].init(3,x,y,15,0,0);
                            shipManager.ships[i+10].pos += shipManager.ships[0].pos;
                        }
                    }
                    UI::Intermission::show("INTRUDERS DETECTED", INTERMISSION_TYPE_OVERLAY);
                }
            }
        }


        namespace L03c {
            uint8_t state;
            void init () {
                spawnAsteroids(3, AsteroidType::White,0,0,250,80,0);
                spawnAsteroids(4, AsteroidType::WhiteSmall,0,0,250,80,0);

                state = 0;
                uint8_t id = 1;
                addWormhole(id, -15,20,"W-HOME", DESTINATION_MAIN, PlayerStats::hasVisited(DESTINATION_03c));
            }
            void tick() {
                if (state == 0 && asteroidManager.countType(AsteroidType::White) < 2) {
                    state = 1;
                    for (int i=0;i<3;i+=1) {
                        int x = (Math::randInt() % 512) - 256;
                        int y = (Math::randInt() % 512) - 256;
                        if (x*x+y*y > 1024){

                            shipManager.ships[i+4].init(3,x,y,15,0,0);
                            shipManager.ships[i+4].aiStrength = 3;
                            shipManager.ships[i+4].pos += shipManager.ships[0].pos;
                        }
                    }
                    UI::Intermission::show("INTRUDERS DETECTED", INTERMISSION_TYPE_OVERLAY);
                }
                if (state == 1 && asteroidManager.countAll() < 3) {
                    state = 2;
                    for (int i=0;i<3;i+=1) {
                        int x = (Math::randInt() % 512) - 256;
                        int y = (Math::randInt() % 512) - 256;
                        if (x*x+y*y > 1024){

                            shipManager.ships[i+4].aiStrength = 4;
                            shipManager.ships[i+10].init(3,x,y,15,0,0);
                            shipManager.ships[i+10].pos += shipManager.ships[0].pos;
                        }
                    }
                    UI::Intermission::show("INTRUDERS DETECTED", INTERMISSION_TYPE_OVERLAY);
                }
            }
        }


        namespace Main {
            uint8_t textId;
            const char* texts[] = {
                "Here you are!",
                "There are a few sectors that need your attention.",
                "Feel free to clear them as you like.",
                0,

            };


            void init() {
                uint8_t id = 2;
                shipManager.ships[1].init(ShipTypeStation,15,8,15,0,0);
                addWormhole(id, -135,200,"W-03a", DESTINATION_03a, true);
                addWormhole(id, -225,230,"W-03b", DESTINATION_03b, PlayerStats::hasVisited(DESTINATION_03a));
                addWormhole(id, -125,340,"W-03c", DESTINATION_03c, PlayerStats::hasVisited(DESTINATION_03b));                if (PlayerStats::hasVisited(DESTINATION_MAIN)) {
                    textId = 3;
                } else {
                    textId = 0;
                }
            }
            void tick() {
                if (!UI::Intermission::isActive() && texts[textId]) {
                    UI::Intermission::show(texts[textId],INTERMISSION_TYPE_OVERLAY);
                    textId += 1;
                }
            }
        }
    }
}


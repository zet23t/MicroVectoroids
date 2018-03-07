#include "game_projectile.h"
#include "game_particles.h"
#include "game_common.h"
#include "lib_sound.h"

namespace Game {
    ProjectileManager projectileManager;

    void Projectile::draw() {

    }

    void Projectile::impact() {
        uint8_t t = type == ProjectileTypePlayer ? ParticleType::PlayerShotImpact : ParticleType::EnemyShotImpact;
        particleSystem.spawn(t, pos, velocity.right() * Fix4(0,6));
        particleSystem.spawn(t, pos, velocity.left() * Fix4(0,6));
        particleSystem.spawn(t, pos, velocity.left() * Fix4(0,3) - velocity * Fix4(0,3));
        particleSystem.spawn(t, pos, velocity.right() * Fix4(0,3) - velocity * Fix4(0,3));

        static const int8_t b[] = {100,-100,50,-50,25,-25,-12,12,-6,6};
        Sound::playSample(0,b, sizeof(b), 0x20 + (Math::randInt()&0xf) ,0x150,100)->setChange(0x700,-1,-2)->interpolate = 0;
        type = 0;
    }

    void Projectile::tick() {
        if (age > 18) {
            type = 0;
            return;
        }
        prevPos = pos;
        pos += velocity;
        Fixed2D4 diff = pos - prevPos;
        if (diff.x != 0 || diff.y != 0) {
            Fix4 len = diff.length();
            Fixed2D4 dir = diff;
            dir.normalize();
            Fixed2D4 p = pos;
            for (Fix4 step = 0; step < len; step += Fix4(2,0)) {
                p += dir * Fix4(2,0);
                particleSystem.spawn(type == ProjectileTypePlayer ?
                                     ParticleType::PlayerShot :
                                     ParticleType::EnemyShot, p, velocity * Fix4(0,0));
            }
        }
        //particleSystem.spawn(ParticleType::PlayerShot, pos - velocity * Fix4(0,8),  velocity * Fix4(1,0));
        age+=1;
    }

    void Projectile::init(uint8_t t, Fixed2D4 xy, Fixed2D4 v) {
        pos = xy;
        type = t;
        velocity = v;
        age = 0;
        damage = 0;
    }

    Projectile* ProjectileManager::spawn(uint8_t t, Fixed2D4 xy, Fixed2D4 v) {
        static Projectile p;
        for (int i=0;i<ProjectileMaxCount;i+=1) {
            if (projectiles[i].type == 0) {
                projectiles[i].init(t,xy,v);
                return &projectiles[i];
            }
        }
        return &p;
    }

    void ProjectileManager::init() {
        memset(projectiles,0,sizeof(projectiles));
    }

    void ProjectileManager::tick() {
        for (int i=0;i<ProjectileMaxCount;i+=1) {
            if (projectiles[i].type) {
                projectiles[i].tick();
            }
        }
    }

    void ProjectileManager::draw() {
        for (int i=0;i<ProjectileMaxCount;i+=1) {
            if (projectiles[i].type) {
                projectiles[i].draw();
            }
        }
    }
}

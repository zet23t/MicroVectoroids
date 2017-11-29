#include "game_ships.h"
#include "game_common.h"
#include "game_particles.h"
#include "game_projectile.h"
#include "game_asteroids.h"
#include "game_ui_hud.h"
#include "game_collectable.h"

namespace Game {
    ShipManager shipManager;
    bool directionalControls = false;

    void Ship::tickPlayerControls() {
        Fixed2D4 input =  Joystick::getJoystick();
        //if (Joystick::getButton(0)) {
        if (Joystick::getButton(1)) {
            shoot();
        }
        if (input.x !=0 || input.y !=0) {
            if (!directionalControls) {
                input.normalize();
                if (input.dot(direction) > Fix4(0,12)) {
                    velocity += direction * Fix4(0,12);
                }

                Fix4 force = Fix4(0,7);
                Fix4 dot = direction.dot(input);
                if (dot < Fix4(-1,2)) {
                    // enable turnarounds without getting stuck (switch left to right for instance)

                    Fixed2D4 right = input.right();
                    Fixed2D4 left = input.left();
                    Fix4 leftDot = left.dot(direction);
                    Fix4 rightDot = right.dot(direction);
                    input = leftDot > rightDot ? left : right;
                }
                //force = Fix4(3,0);
                direction += input * force;
                direction.normalize();
            }
            else {
                input.normalize();
                direction += direction.left() * input.x * Fix4(0,4);
                direction.normalize();
                if (input.y > 0) input.y = 0;
                velocity += direction * Fix4(0,10) * -input.y;
            }
        }
    }

    void Ship::handleProjectileCollisions(uint8_t ptype) {
        for (int i=0;i<ProjectileMaxCount;i+=1) {
            Projectile* p = &projectileManager.projectiles[i];
            if (p->type == ptype) {
                Fixed2D4 diff = pos - p->pos;
                if (diff.manhattanDistance() < Fix4(10,0) && diff.length() < Fix4(5,0)) {
                    p->impact();
                    takeDamage(8);
                }
            }
        }
    }

    void Ship::tickPlayer() {
        if (takenHitCooldown == 0) {
            tickPlayerControls();
        }
        handleAsteroidsCollisions();
        handleProjectileCollisions(ProjectileTypeEnemy);
    }

    void Ship::tickEnemySmallShip() {
       // printf("%f %f\n",direction.x.asFloat(),direction.y.asFloat());
        //direction += direction.right() * Fix4(0,3);
        Ship const* player = shipManager.ships;
        Fixed2D4 diff = player->pos - pos;
        int mdist = diff.manhattanDistance().getIntegerPart();
        if (mdist > 200) return;
        Fixed2D4 six = -player->direction - player->velocity * Fix4(2,0);
        if (six.x==0 && six.y == 0) six = -player->direction;
        six = six.normalize();
        Fixed2D4 diffNorm = diff;
        diffNorm.normalize();
        bool finalTarget = true;
        //buffer.drawRect((player->pos + six * Fix4(25,0)).x.getIntegerPart(),(player->pos + six * Fix4(25,0)).y.getIntegerPart(),2,2)->filledRect(0xfc0f);
        Fix4 dot= diffNorm.dot(six);
        if (dot > Fix4(0,10)) {
            if (diffNorm.dot(six.right()) < 0) six = six.right();
            else six = six.left();
            finalTarget = false;
        }
        Fixed2D4 target = player->pos + six * Fix4(25,0);
        Fixed2D4 sixdiff = target - pos;
        //if(sixdiff.length() > diff.length() * Fix4(2,0)) {
            //    printf("%f %f\n", sixdiff.manhattanDistance().asFloat(), diff.manhattanDistance().asFloat());
        //    target = player->pos + diffNorm.right() * Fix4(25,0);
        //    finalTarget = false;
        //}
        //buffer.drawRect(target.x.getIntegerPart(),target.y.getIntegerPart(),2,2)->filledRect(0xffff);

        if ((sixdiff.manhattanDistance() > Fix4(10,0)) || !finalTarget) {
            Fixed2D4 turn = sixdiff;
            turn.normalize();
            direction = direction * Fix4(3,5) + turn;
            direction = direction.normalize();
            velocity = velocity * Fix4(0,12);
            velocity += direction * Fix4(1,0);

        } else {
            //printf("reached %d\n",frame);
            velocity = velocity * Fix4(0,12);
            diff.normalize();
            direction = diff + direction * (dot + Fix4(1,0));
            direction = direction.normalize();
        }
        if (dot < Fix4(-1,8) && diffNorm.dot(direction) > Fix4(0,8))
            shoot();

        handleProjectileCollisions(ProjectileTypePlayer);
    }

    void Ship::handleAsteroidsCollisions() {
        Fixed2D4 future = pos + velocity * Fix4(0,6);
        int16_t x = future.x.getIntegerPart();
        int16_t y = future.y.getIntegerPart();
        for (int i=0;i<AsteroidsCount;i+=1) {
            Asteroid *a = &asteroidManager.asteroids[i];
            if (a->type == 0) continue;
            int16_t dx = a->pos.x.getIntegerPart() - x;
            int16_t dy = a->pos.y.getIntegerPart() - y;
            uint8_t rad = asteroidRadiusByType[a->type];
            if (abs(dx) < rad && abs(dy) < rad && dx*dx+dy*dy < asteroidRadiusSqByType[a->type]) {
                Fixed2D4 norm = a->pos - pos;
                norm = norm.normalize();
                Fixed2D4 v = velocity - a->velocity;
                Fixed2D4 change = (v -  norm * (Fix4(2,0) * norm.dot(v))) * Fix4(0,8) - norm * Fix4(0,12);
                a->push(-change);
                velocity = change;
                if (takenHitCooldown == 0)
                    takeDamage(16);
                takenHitCooldown = 8;

                break;
            }
        }
    }

    void Ship::tick() {
        prevPos = pos;
        if (type == 1) {
            tickPlayer();
        }
        if (type == 3) {
            tickEnemySmallShip();
        }

        if (shootCooldown > 0) shootCooldown -= 1;
        if (takenHitCooldown > 0) takenHitCooldown -= 1;
        if (type == 1) {
        }

        pos += velocity * Fix4(0,6);
        if ((frame & 2) == 2 ) {
            velocity = velocity * (Fix4(0,15));
            if (velocity.x.absolute() < Fix4(0,3)) velocity.x = 0;
            if (velocity.y.absolute() < Fix4(0,3)) velocity.y = 0;
        }

        Fixed2D4 dir = pos - prevPos;
        Fix4 dist = (dir).length();
        if (dist > 0 && takenHitCooldown == 0) {
            Fix4 v = Fix4(2,0)/dist;
            dir.scale(v);
            Fixed2D4 p = pos - direction * 6;
            uint8_t exhaustType = type == 1 ? ParticleType::PlayerShipTrail : ParticleType::EnemyShipTrail;
            for (Fix4 f = 0; f<=dist + Fix4(0,8); f+=Fix4(2,0)) {
                particleSystem.spawn(exhaustType,p,Fixed2D4());
                p -= dir;
            }
        }
    }

    uint8_t Ship::maxDamage() {
        switch (type) {
        case ShipTypePlayer: return 255;
        case ShipTypeEnemySmall: return 24;
        default: return 0;
        }
    }

    void Ship::explode() {
        uint8_t t = type == ShipTypePlayer ? ParticleType::PlayerShipExplosion : ParticleType::EnemyShipExplosion;
        for (int i=0;i<20;i+=1) {
            Fixed2D4 r;
            Fixed2D4 p = r.randomCircle(Fix4(8,0));
            Fixed2D4 vel = r.randomCircle(Fix4(1,6));
            if (i < 10 && type != ShipTypePlayer)
                Collectable::spawn(pos, vel * Fix4(3,0));
            particleSystem.spawn(t, pos + p, vel);
        }
        //particleSystem.spawn(t, pos, direction.left() * Fix4(0,6));
        //particleSystem.spawn(t, pos, direction.left() * Fix4(0,3) - direction * Fix4(0,3));
        //particleSystem.spawn(t, pos, direction.right() * Fix4(0,3) - direction * Fix4(0,3));

        type = 0;
    }

    void Ship::takeDamage(uint8_t dmg) {
        if (dmg > maxDamage() - damage) {
            damage = maxDamage();
            explode();
        }
        else damage += dmg;
    }

    int calcDirectionIndex(Fixed2D4 direction) {
        int y = direction.y.getRaw();
        int x = direction.x.getRaw();
        // 16,0 = right
        // 0,16 = bottom = 0
        int idir = 0;
        if (y > 15 && abs(x) < 4) return 0;
        if (x > 15 && abs(y) < 4) return 4;
        if (x < -15 && abs(y) < 4) return 12;
        if (y < -15 && abs(x) < 4) return 8;
        if (x > 14) return y<0 ? 5 : 3;
        if (x > 10) return y<0 ? 6 : 2;
        if (x < -14) return y<0 ? 11 : 13;
        if (x < -10) return y<0 ? 10 : 14;
        if (y > 0) return x<0 ? 15 : 1;
        if (y < 0) return x<0 ? 9 : 7;
        return 0;
    }

    bool Ship::calcParallaxScreenPos() {
        const int cx = buffer.getOffsetX()+48;
        const int cy = buffer.getOffsetY()+32;
        const int x = pos.x.getIntegerPart();
        const int y = pos.y.getIntegerPart();
        const int dx = x - cx;
        const int dy = y - cy;
        const int px = cx + dx * 2 / 3, py = cy + dy * 2 / 3;
        screenPos[0] = (int16_t)px;
        screenPos[1] = (int16_t)py;
        return abs(px - cx) < 64 && abs(py - cy) < 48;
    }
    void Ship::drawOnUI(int8_t x, int8_t y) {
        switch (type) {
        case 1:
        break;
        case ShipTypeStation:
            drawCenteredSprite(x,y,ImageAsset::TextureAtlas_atlas::ship_station.sprites[0])->blend(RenderCommandBlendMode::add)->setDepth(200);
            break;
        case ShipTypeWormHole:
            drawCenteredSprite(x,y,ImageAsset::TextureAtlas_atlas::worm_hole.sprites[frameUnpaused / 4 % ImageAsset::TextureAtlas_atlas::worm_hole.spriteCount])->blend(RenderCommandBlendMode::add)->setDepth(200);
            break;
        case ShipTypeWormHoleInactive:
            for (int i=0;i<4;i+=1) {
                int px = (Math::randInt() + frameUnpaused * 211)%32 - 16;
                int py = (Math::randInt() + frameUnpaused * 371)%32 - 16;
                if (px * px + py*py < (2 + i*4)*16) {
                    drawCenteredSprite(x+px,y+py,ImageAsset::TextureAtlas_atlas::wormhole_particle.sprites[2])->blend(RenderCommandBlendMode::add)->setDepth(200);
                }
            }
            break;

        case ShipTypeEnemySmall:
            drawCenteredSprite(x,y, ImageAsset::TextureAtlas_atlas::ship_enemy_small.sprites[(frameUnpaused / 4) % 16])->blend(RenderCommandBlendMode::add)->setDepth(200);
            break;
        }
    }

    void Ship::draw() {
        switch (type) {
        case 1:
            {
                const int pivotX = -8;
                const int pivotY = -8;

                int idir = calcDirectionIndex(direction);
                int x = pos.x.getIntegerPart(),y = pos.y.getIntegerPart();
                screenPos[0] = (int16_t)x;
                screenPos[1] = (int16_t)y;
                drawCenteredSprite(x,y, ImageAsset::TextureAtlas_atlas::ship_triangle.sprites[(idir + 8) % 16])->blend(RenderCommandBlendMode::add);
                Fixed2D4 dir = (pos + direction * Fix4(15,0));
                drawCenteredSprite(dir.x.getIntegerPart(), dir.y.getIntegerPart(), ImageAsset::TextureAtlas_atlas::ui_crosshair.sprites[0])->blend(RenderCommandBlendMode::add);
                if (takenHitCooldown) {
                    drawCenteredSprite(x,y, ImageAsset::TextureAtlas_atlas::ship_triangle_shield.sprites[takenHitCooldown])->blend(RenderCommandBlendMode::add);
                }

            }
            break;
        case ShipTypeStation:
            {
                calcParallaxScreenPos();
                drawCenteredSprite(screenPos[0],screenPos[1],ImageAsset::TextureAtlas_atlas::ship_station.sprites[(frame>>4)%6])->blend(RenderCommandBlendMode::add);
            }
            break;
        case ShipTypeWormHole:
            {
                calcParallaxScreenPos();
                drawCenteredSprite(screenPos[0],screenPos[1],ImageAsset::TextureAtlas_atlas::worm_hole.sprites[3-(frame>>1)%4])->blend(RenderCommandBlendMode::add);
            }
            break;
        case ShipTypeWormHoleInactive:
            {
                if (calcParallaxScreenPos()) {
                    //if ((frame>>2) *251 % 17 < 5)
                    //    drawCenteredSprite(screenPos[0],screenPos[1],ImageAsset::TextureAtlas_atlas::worm_hole.sprites[3-(frame>>1)%4])->blend(RenderCommandBlendMode::add);
                    for (int i=0;i<4;i+=1) {
                        int px = (Math::randInt() + frame * 211)%32 - 16;
                        int py = (Math::randInt() + frame * 371)%32 - 16;
                        if (px * px + py*py < (2 + i*4)*16) {
                            particleSystem.spawn(ParticleType::WormHoleParticle,
                                                 Fixed2D4((int16_t)(screenPos[0]+px),(int16_t)(screenPos[1]+py)),
                                                 Fixed2D4((int16_t)(-px),(int16_t)(-py)) * Fix4(0,2));
                            //drawCenteredSprite(screenPos[0]+px,screenPos[1]+py,
                            //                   ImageAsset::TextureAtlas_atlas::wormhole_particle.sprites[(frame + i) * 17 % ImageAsset::TextureAtlas_atlas::worm_hole.spriteCount])->blend(RenderCommandBlendMode::add);
                        }
                    }
                }

            }

            break;

        case ShipTypeEnemySmall:
            {

                int idir = calcDirectionIndex(direction);
                int x = pos.x.getIntegerPart(),y = pos.y.getIntegerPart();
                screenPos[0] = x;
                screenPos[1] = y;
                drawCenteredSprite(x,y, ImageAsset::TextureAtlas_atlas::ship_enemy_small.sprites[idir])->blend(RenderCommandBlendMode::add);
            }
            break;
        }
//
//        buffer.drawRect(pos.x.getIntegerPart(),pos.y.getIntegerPart(),4,4)->filledRect(RGB565(255,0,0));
    }
    void Ship::init(int8_t type, int16_t x, int16_t y, int8_t dx, int8_t dy, char *inf) {
        this->type = type;
        info = inf;
        pos.setXY(x,y);
        prevPos = pos;
        damage = 0;
        direction.x = Fix4(0,dx);
        direction.y = Fix4(0,dy);
    }

    void Ship::shoot() {
        if (shootCooldown == 0) {
            shootCooldown = type == 1 ? 8 : 12;
            Fixed2D4 dir = direction;
            if (UI::HUD::targetLock && type == 1) {

                dir = UI::HUD::targetPosition - pos;
                dir += UI::HUD::targetVelocity * Fix4(0,1) * dir.length();
                dir = dir.normalize();
            }

            projectileManager.spawn(type == 1 ? ProjectileTypePlayer : ProjectileTypeEnemy, pos + dir * Fix4(3,0),
                                    dir * Fix4(type == 1 ? 6 : 4,0) + velocity * Fix4(0,6));
        }
    }

    void ShipManager::init() {
        memset(ships,0,sizeof(ships));
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
}

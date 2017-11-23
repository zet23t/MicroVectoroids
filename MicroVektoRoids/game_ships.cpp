#include "game_ships.h"
#include "game_common.h"
#include "game_particles.h"
#include "game_projectile.h"
#include "game_asteroids.h"
#include "game_ui_hud.h"

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
    void Ship::takeDamage(uint8_t dmg) {
        if (dmg > 255 - damage) damage = 255;
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

    void Ship::draw() {
        switch (type) {
        case 1:
            {
                const int pivotX = -8;
                const int pivotY = -8;

                int idir = calcDirectionIndex(direction);
                int x = pos.x.getIntegerPart(),y = pos.y.getIntegerPart();
                drawCenteredSprite(x,y, ImageAsset::TextureAtlas_atlas::ship_triangle.sprites[(idir + 8) % 16])->blend(RenderCommandBlendMode::add);
                Fixed2D4 dir = (pos + direction * Fix4(15,0));
                drawCenteredSprite(dir.x.getIntegerPart(), dir.y.getIntegerPart(), ImageAsset::TextureAtlas_atlas::ui_crosshair.sprites[0])->blend(RenderCommandBlendMode::add);
                if (takenHitCooldown) {
                    drawCenteredSprite(x,y, ImageAsset::TextureAtlas_atlas::ship_triangle_shield.sprites[takenHitCooldown])->blend(RenderCommandBlendMode::add);
                }

            }
            break;
        case 2:
            {
                const int cx = buffer.getOffsetX()+48;
                const int cy = buffer.getOffsetY()+32;
                const int x = pos.x.getIntegerPart();
                const int y = pos.y.getIntegerPart();
                const int dx = x - cx;
                const int dy = y - cy;
                const int px = cx + dx / 3 * 2, py = cy + dy / 3 * 2;
                drawCenteredSprite(px,py,ImageAsset::TextureAtlas_atlas::ship_station.sprites[(frame>>4)%6])->blend(RenderCommandBlendMode::add);
            }
            break;
        case 3:
            {

                int idir = calcDirectionIndex(direction);
                int x = pos.x.getIntegerPart(),y = pos.y.getIntegerPart();
                drawCenteredSprite(x,y, ImageAsset::TextureAtlas_atlas::ship_enemy_small.sprites[idir])->blend(RenderCommandBlendMode::add);
            }
            break;
        }
//
//        buffer.drawRect(pos.x.getIntegerPart(),pos.y.getIntegerPart(),4,4)->filledRect(RGB565(255,0,0));
    }
    void Ship::init(int8_t type, int16_t x, int16_t y, int8_t dx, int8_t dy) {
        this->type = type;
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

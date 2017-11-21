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

    void Ship::tickPlayer() {
        if (takenHitCooldown == 0) {
            tickPlayerControls();
        }
        handleAsteroidsCollisions();
    }

    void Ship::tickEnemySmallShip() {
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
            for (Fix4 f = 0; f<=dist + Fix4(0,8); f+=Fix4(2,0)) {
                particleSystem.spawn(1,p,Fixed2D4());
                p -= dir;
            }
        }
    }
    void Ship::takeDamage(uint8_t dmg) {
        if (dmg > 255 - damage) damage = 255;
        else damage += dmg;
    }
    void Ship::draw() {
        switch (type) {
        case 1:
            {
                const int pivotX = -8;
                const int pivotY = -8;
                FixedNumber16<4> dirModY = direction.y;
                // making up for missing asin transformation (for converting dot to angle) - rotation ain't linear
                if (dirModY < 0 && dirModY.getFractionPart() > 2 && dirModY.getFractionPart() < 7) dirModY += FixedNumber16<4>(0,3);
                if (dirModY < 0 && dirModY.getFractionPart() > 11 && dirModY.getFractionPart() < 13) dirModY -= FixedNumber16<4>(0,1);
                if (dirModY > 0 && dirModY.getFractionPart() > 2 && dirModY.getFractionPart() < 9) dirModY -= FixedNumber16<4>(0,3);
                if (dirModY > 0 && dirModY.getFractionPart() > 8 && dirModY.getFractionPart() < 13) dirModY -= FixedNumber16<4>(0,1);
                int idir = 8-((dirModY + FixedNumber16<4>(1,0)) * FixedNumber16<4>(4,0)).getIntegerPart();
                if (idir < 0) idir = 0;
                if (idir > 8) idir = 8;
                if (direction.x < 0) idir = 16 - idir;

                SpriteSheetRect rect = ImageAsset::TextureAtlas_atlas::ship_triangle.sprites[(idir + 8) % 16];

                int w = rect.width, h = rect.height;
                buffer.drawRect(pos.x.getIntegerPart() + rect.offsetX + pivotX,pos.y.getIntegerPart() + rect.offsetY + pivotY,w,h)->sprite(&atlas,rect.x,rect.y)->blend(RenderCommandBlendMode::add);

                if (takenHitCooldown) {
                    SpriteSheetRect rect = ImageAsset::TextureAtlas_atlas::ship_triangle_shield.sprites[takenHitCooldown];
                    w = rect.width;
                    h = rect.height;
                    buffer.drawRect(pos.x.getIntegerPart() + rect.offsetX - rect.origWidth / 2,
                                    pos.y.getIntegerPart() + rect.offsetY - rect.origHeight / 2,w,h)->sprite(&atlas,rect.x,rect.y)->blend(RenderCommandBlendMode::add);
                }
            }
            break;
        case 2:
            {
                SpriteSheetRect rect = ImageAsset::TextureAtlas_atlas::ship_station.sprites[(frame>>4)%6];

                const int pivotX = -16;
                const int pivotY = -16;
                int w = rect.width, h = rect.height;
                const int cx = buffer.getOffsetX()+48;
                const int cy = buffer.getOffsetY()+32;
                const int x = pos.x.getIntegerPart();
                const int y = pos.y.getIntegerPart();
                const int dx = x - cx;
                const int dy = y - cy;
                buffer.drawRect(cx + dx / 3 * 2 + rect.offsetX + pivotX,cy + dy / 3 * 2 + rect.offsetY + pivotY,w,h)->sprite(&atlas,rect.x,rect.y)->blend(RenderCommandBlendMode::add);
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
            shootCooldown = 8;
            Fixed2D4 dir = direction;
            if (UI::HUD::targetLock) {
                dir = UI::HUD::targetPosition - pos;
                dir = dir.normalize();
            }
            projectileManager.spawn(1, pos + dir * Fix4(3,0), dir * Fix4(6,0) + velocity * Fix4(0,6));
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

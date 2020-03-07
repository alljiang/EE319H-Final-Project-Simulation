//
// Created by Allen on 3/2/2020.
//

#include <cstdlib>
#include <cstdio>
#include "entities.h"
#include "LCD.h"

using namespace std;

void HitboxManager::checkCollisions() {
    for (uint8_t slot = 0; slot < hurtboxSlots; slot++) {
        if(!hurtboxes[slot].active) continue;
        if(p2 != nullptr && hurtboxes[slot].source == 1 && p2->hitbox.isColliding(hurtboxes[slot])) {
            //  hurtbox collision with player 2!
            hurtboxes[slot].active = false;
            p2->collide(&hurtboxes[slot]);
        }
        else if(p2 != nullptr && hurtboxes[slot].source == 2 && p1->hitbox.isColliding(hurtboxes[slot])) {
            //  hurtbox collision with player 1!
            hurtboxes[slot].active = false;
            p1->collide(&hurtboxes[slot]);
        }
        else if(hurtboxes[slot].source == 0) {
            if(p1->hitbox.isColliding(hurtboxes[slot])) {
                p1->collide(&hurtboxes[slot]);
            }
            if(p2 != nullptr && p2->hitbox.isColliding(hurtboxes[slot])) {
                p2->collide(&hurtboxes[slot]);
            }
        }
        else {
            //  update hurtbox frame
            if( hurtboxes[slot].frameLengthCounter++ >= hurtboxes[slot].frameLength
                && !((persistentHurtbox >> slot) & 1)) {
                hurtboxes[slot].frameLengthCounter = 0;
                hurtboxes[slot].currentFrame++;
                if(hurtboxes[slot].currentFrame >= hurtboxes[slot].frames) {
                    hurtboxes[slot].active = false;
                }
            }
        }
    }
}

void HitboxManager::displayHitboxesOverlay() {
    uint32_t hitboxColorsub = 0x555500;
    uint32_t hurtboxColorsub = 0x007777;
//    uint32_t hurtboxColorsub = 0xAA00AA;

    //  first display player hitboxes
    if(p1->hitbox.shape == SHAPE_CIRCLE) {
        LCD_drawOverlayCircle(p1->hitbox.x + p1->hitbox.xOffset,
                p1->hitbox.y + p1->hitbox.yOffset,
                p1->hitbox.radius + p1->hitbox.radiusOffset, hitboxColorsub);
    }
    else if(p1->hitbox.shape == SHAPE_RECTANGLE) {
        LCD_drawOverlayRectangle(p1->hitbox.x + p1->hitbox.xOffset,
                p1->hitbox.y + p1->hitbox.yOffset,
                              p1->hitbox.width, p1->hitbox.height, hitboxColorsub);
    }

    if(p2 != nullptr) {
        if (p2->hitbox.shape == SHAPE_CIRCLE) {
            LCD_drawOverlayCircle(p2->hitbox.x + p2->hitbox.xOffset,
                    p2->hitbox.y + p2->hitbox.yOffset,
                    p2->hitbox.radius + p2->hitbox.radiusOffset, hitboxColorsub);
        } else if (p2->hitbox.shape == SHAPE_RECTANGLE) {
            LCD_drawOverlayRectangle(p2->hitbox.x +  + p2->hitbox.xOffset,
                    p2->hitbox.y + p2->hitbox.yOffset,
                                     p2->hitbox.width, p2->hitbox.height, hitboxColorsub);
        }
    }

    //  next, draw hurtboxes
    for (uint8_t slot = 0; slot < hurtboxSlots; slot++) {
        if(hurtboxes[slot].active) {
            if(hurtboxes[slot].shape == SHAPE_CIRCLE) {
                LCD_drawOverlayCircle(hurtboxes[slot].x + hurtboxes[slot].xOffset,
                        hurtboxes[slot].y + hurtboxes[slot].yOffset,
                        hurtboxes[slot].radius, hurtboxColorsub);
            }
            else if(hurtboxes[slot].shape == SHAPE_RECTANGLE) {
                LCD_drawOverlayRectangle(hurtboxes[slot].x + hurtboxes[slot].xOffset,
                        hurtboxes[slot].y + hurtboxes[slot].yOffset,
                        hurtboxes[slot].width, hurtboxes[slot].height, hurtboxColorsub);
            }
        }
    }
}

void HitboxManager::clearHitboxOverlay() {
    LCD_clearOverlay();
}

void HitboxManager::addHurtbox(double xOffset, double yOffset, bool mirrored,
        class Hurtbox hurtBox, uint8_t playerSource, bool persistent) {
    hurtBox.active = true;
    hurtBox.currentFrame = 0;
    hurtBox.frameLengthCounter = 0;

    if(mirrored) hurtBox.x = xOffset-hurtBox.x;
    else hurtBox.x += xOffset;
    hurtBox.y += yOffset;
    hurtBox.setSource(playerSource);

    //  add the hurtbox to the array, find a slot for it
    uint8_t slot = 0;
    for(slot = 0; slot < hurtboxSlots; slot++) {
        if(!hurtboxes[slot].active) {
            if(persistent) persistentHurtbox |= 1u << slot;
            hurtboxes[slot] = hurtBox;
            break;
        }
    }
    if(slot == hurtboxSlots) return;    //  no slots remaining
}

void HitboxManager::addHurtbox(double xOffset, double yOffset, bool mirrored,
       class Hurtbox hurtBox, uint8_t playerSource) {
    this->addHurtbox(xOffset, yOffset, mirrored, hurtBox, playerSource, false);
}

bool Hitbox::isColliding(class Hurtbox hurtbox) {
    double thisx = x + xOffset;
    double thisy = y + yOffset;
    double thisradius = radius + radiusOffset;

    double hbx = hurtbox.x + hurtbox.xOffset;
    double hby = hurtbox.y + hurtbox.yOffset;
    double hbh = hurtbox.height;
    double hbw = hurtbox.width;
    double hbr = hurtbox.radius;

    switch (this->shape) {
        case SHAPE_CIRCLE:
            if(hurtbox.shape == SHAPE_CIRCLE) {
                /*
                 * Find the distance between the two circles.
                 * If this distance is less than the sum of the radii
                 * of the circles, they're colliding.
                 */
                return abs((thisx - hbx) * (thisx - hbx)
                + (thisy - hby) * (thisy - hby))
                < (thisradius + hbr) * (thisradius + hbr);
            }
            else if(hurtbox.shape == SHAPE_RECTANGLE) {
                double distanceX = abs(thisx - hbx);
                double distanceY = abs(thisy - hby);

                /*
                 * If the distance between the two objects are greater
                 * than the radius and the distance from center to edge combined,
                 * there is definitely no collision
                 */
                if (distanceX > (hbw/2 + thisradius)) { return false; }
                if (distanceY > (hbh/2 + thisradius)) { return false; }

                /*
                 * If the center of the circle is inside the rectangle,
                 * there's a collision.
                 */
                if (distanceX <= (hbw/2)) { return true; }
                if (distanceY <= (hbh/2)) { return true; }

                /*
                 * Check if the circle intersects rectangle corner
                 */
                double cornerDistance_sq = (distanceX - hbw/2)*(distanceX - hbw/2) +
                                            (distanceY - hbh/2)*(distanceY - hbh/2);

                return (cornerDistance_sq <= (this->radius*thisradius));
            }
        case SHAPE_RECTANGLE:
            if(hurtbox.shape == SHAPE_CIRCLE) {
                double distanceX = abs(thisx - hbx);
                double distanceY = abs(thisy - hby);

                /*
                 * If the distance between the two objects are greater
                 * than the radius and the distance from center to edge combined,
                 * there is definitely no collision
                 */
                if (distanceX > (this->width/2 + hbr)) { return false; }
                if (distanceY > (this->height/2 + hbr)) { return false; }

                /*
                 * If the center of the circle is inside the rectangle,
                 * there's a collision.
                 */
                if (distanceX <= (this->width/2)) { return true; }
                if (distanceY <= (this->height/2)) { return true; }

                /*
                 * Check if the circle intersects rectangle corner
                 */
                double cornerDistance_sq =
                        (distanceX - this->width/2)*(distanceX - this->width/2) +
                        (distanceY - this->height/2)*(distanceY - this->height/2);

                return (cornerDistance_sq <= (hbr*hbr));
            }
            else if(hurtbox.shape == SHAPE_RECTANGLE) {
                return  thisx + this->width >= hbx &&     // r1 right edge past r2 left
                        thisx <= hbx + hbw &&             // r1 left edge past r2 right
                        thisy + this->height >= hby &&    // r1 top edge past r2 bottom
                        thisy <= hby + hbh;               // r1 bottom edge past r2 top
            }
    }
    return false;
}

//
// Created by Allen on 2/28/2020.
//

#include <cstdio>
#include "entities.h"
#include "utils.h"
#include "metadata.h"
#include "UART.h"
#include "stage.h"

void Kirby::controlLoop(double joyH, double joyV, bool btnA, bool btnB, bool shield,
                        class Stage *stage, class HitboxManager *hitboxManager) {
    //  assume joystick deadzone filtering is already done

    double dt = 49;
    currentTime += (uint8_t)dt;
    l_time = currentTime;

    if(!l_btnA && btnA) l_btnARise_t = currentTime;
    else if(l_btnA && !btnA) l_btnAFall_t = currentTime;
    if(!l_btnB && btnB) l_btnBRise_t = currentTime;
    else if(l_btnB && !btnB) l_btnBFall_t = currentTime;
    if(!l_shield && shield) l_shieldRise_t = currentTime;
    else if(l_shield && !shield) l_shieldFall_t = currentTime;

    int16_t x_mirroredOffset = 0;

    if(y < 0) {
        y = 120;
        x = 150;
    }

    double yAnimationOffset = 0;
    double xAnimationOffset = 0;

    bool continuous = false;

    this->hitbox.initialize(x+16, y+10, SHAPE_CIRCLE, 12);

    double ceiling = stage->ceil(x + KIRBY_STAGE_OFFSET, y);
    double floor = stage->floor(x + KIRBY_STAGE_OFFSET, y);
    double leftBound = stage->leftBound(x + KIRBY_STAGE_OFFSET, y) - KIRBY_STAGE_OFFSET / 2;
    double rightBound = stage->rightBound(x - KIRBY_STAGE_OFFSET, y) - KIRBY_STAGE_OFFSET;
    double stageVelocity = stage->xVelocity(x, y);
    /* TODO: w h a t   t h e   h e c k */

    //  first, follow up on any currently performing actions
    noJumpsDisabled = jumpsUsed >= 5;

    //  movement
    if(action == KIRBY_ACTION_RUNNING) {

        if(y != floor) {
            action = KIRBY_ACTION_FALLING;
        }
        else if(joyH == 0) {
            action = KIRBY_ACTION_RESTING;
        }
        else {
            //  x position
            x += joyH * groundSpeed;
            if(x > rightBound) x = rightBound;
            else if(x < leftBound) x = leftBound;

            //  mirrored facing left/right
            if (joyH == 0) mirrored = l_mirrored;
            else mirrored = joyH < 0;

            //  decide between slow, walk and run animation
            if (std::abs(joyH) > 0.6) {
                frameExtension = (uint8_t) ((1 - std::abs(0.7 * joyH)));
                if (frameLengthCounter++ > frameExtension) {
                    frameLengthCounter = 0;
                    frameIndex++;
                }
                if (frameIndex >= 8) frameIndex = 0;
                animationIndex = 1;
            } else if(std::abs(joyH) > 0.25){
                frameExtension = (uint8_t) ((-2 * std::abs(joyH)) + 2);
                if (frameLengthCounter++ > frameExtension) {
                    frameLengthCounter = 0;
                    frameIndex++;
                }
                if (frameIndex >= 12) frameIndex = 0;
                animationIndex = 9;
            } else {
                frameExtension = (uint8_t) ((-4 * std::abs(joyH)) + 4);
                if (frameLengthCounter++ > frameExtension) {
                    frameLengthCounter = 0;
                    frameIndex++;
                }
                if (frameIndex >= 5) frameIndex = 0;
                animationIndex = 7;
            }
        }
    }
    else if(action == KIRBY_ACTION_JUMPING) {
        hitbox.offsetY(4);
        hitbox.offsetX(4);
        yVel -= gravityRising;
        x += airSpeed * joyH;
        if(x > rightBound) x = rightBound;
        else if(x < leftBound) x = leftBound;

        mirrored = l_mirrored;
        animationIndex = 4;

        frameExtension = 1;
        if(frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 7) {
            l_action = KIRBY_ACTION_JUMPING;
            action = KIRBY_ACTION_FALLING;
        }
    }
    else if(action == KIRBY_ACTION_MULTIJUMPING) {
        hitbox.offsetY(5);
        hitbox.offsetX(2, mirrored);
        yVel -= gravityRising;
        x += airSpeed * joyH;
        if(x > rightBound) x = rightBound;
        else if(x < leftBound) x = leftBound;

        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        animationIndex = 5;

        frameExtension = 1;
        if(frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 3) {
            l_action = KIRBY_ACTION_MULTIJUMPING;
            action = KIRBY_ACTION_FALLING;
        }
    }
    else if(action == KIRBY_ACTION_CROUCHING) {
        animationIndex = 0;
        frameIndex = 0;

        if(joyV > -0.3) {
            action = KIRBY_ACTION_RESTING;
            lastBlink = currentTime;
        }
    }
    else if(action == KIRBY_ACTION_LEDGEGRAB) {
        animationIndex = 18;
        frameIndex = 0;

        x_mirroredOffset = -2;
        xAnimationOffset = 2;
        yAnimationOffset = -24;
    }
        //  regular attacks, ground
    else if(action == KIRBY_ACTION_JABSINGLE) {
        animationIndex = 10;
        mirrored = l_mirrored;
        x_mirroredOffset = -22;

        frameExtension = 0;
        if(frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 3) {
            disabledFrames = 0;
            l_action = KIRBY_ACTION_JABSINGLE;
            action = KIRBY_ACTION_RESTING;
            x_mirroredOffset = 0;
        }
    }
    else if(action == KIRBY_ACTION_JABDOUBLE) {
        animationIndex = 10;
        mirrored = l_mirrored;
        x_mirroredOffset = -22;

        frameExtension = 1;
        if(frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 5) {
            l_action = KIRBY_ACTION_JABDOUBLE;
            action = KIRBY_ACTION_RESTING;
            x_mirroredOffset = 0;
        }
    }
    else if(action == KIRBY_ACTION_JABREPEATING) {
        animationIndex = 10;
        mirrored = l_mirrored;
        x_mirroredOffset = -22;

        disabledFrames = 2;
        frameExtension = 1;

        //  add hurtbox at beginning of frame
        if(frameLengthCounter == 0) {
            hitboxManager->addHurtbox(x + 16, y, mirrored,
                                      jabRepeating, player);
        }
        if(frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 12) {
            frameIndex = 6;
            frameLengthCounter = 0;
        }
        if(currentTime-l_btnARise_t > 300) {
            l_action = KIRBY_ACTION_JABREPEATING;
            x_mirroredOffset = 0;
            action = KIRBY_ACTION_RESTING;

            disabledFrames = 0;
        }
    }
    else if(action == KIRBY_ACTION_FORWARDTILT) {
        animationIndex = 14;
        mirrored = l_mirrored;
        x_mirroredOffset = 0;

        frameExtension = 0;
        if(frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 8) {
            l_action = KIRBY_ACTION_FORWARDTILT;
            action = KIRBY_ACTION_RESTING;
            disabledFrames = 2;
            x_mirroredOffset = 0;
        }
    }
    else if(action == KIRBY_ACTION_UPTILT) {
        animationIndex = 16;
        mirrored = l_mirrored;
        x_mirroredOffset = 0;

        frameExtension = 0;
        if(frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 7) {
            l_action = KIRBY_ACTION_UPTILT;
            action = KIRBY_ACTION_RESTING;
            disabledFrames = 6;
            x_mirroredOffset = 0;
        }
    }
    else if(action == KIRBY_ACTION_FORWARDSMASHHOLD) {
        animationIndex = 12;
        mirrored = l_mirrored;
        x_mirroredOffset = -26;

        //  release attack
        if((!btnA && currentTime - fsmash_startTime > 150) || currentTime - fsmash_startTime > 3000) {
            action = KIRBY_ACTION_FORWARDSMASH;
            frameIndex = 2;
        }
            //  charging attack
        else {
            disabledFrames = 2;
            frameExtension = 1;
            if (frameLengthCounter++ > frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 3) {
                frameIndex = 0;
            }
        }
    }
    else if(action == KIRBY_ACTION_FORWARDSMASH) {
        animationIndex = 12;
        mirrored = l_mirrored;
        x_mirroredOffset = -20;

        disabledFrames = 2;
        frameExtension = 0;
        if (frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 8) {
            l_action = KIRBY_ACTION_FORWARDSMASH;
            action = KIRBY_ACTION_RESTING;
            disabledFrames = 2;

            //  kirby shifted after animation, adjust x position to match it
            if(mirrored) x -= 16;
            else x += 23;
            x_mirroredOffset = 0;
        }
    }
    else if(action == KIRBY_ACTION_UPSMASHHOLD) {
        animationIndex = 17;
        mirrored = l_mirrored;
        x_mirroredOffset = -6;

        //  release attack
        if((!btnA && currentTime - usmash_startTime > 150) || currentTime - usmash_startTime > 3000) {
            action = KIRBY_ACTION_UPSMASH;
            frameIndex = 2;
        }
            //  charging attack
        else {
            disabledFrames = 2;
            frameExtension = 1;
            if (frameLengthCounter++ > frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 3) {
                frameIndex = 0;
            }
        }
    }
    else if(action == KIRBY_ACTION_UPSMASH) {
        animationIndex = 17;
        mirrored = l_mirrored;
        x_mirroredOffset = -6;

        disabledFrames = 2;
        frameExtension = 0;
        if (frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 8) {
            l_action = KIRBY_ACTION_UPSMASH;
            action = KIRBY_ACTION_RESTING;
            disabledFrames = 2;
        }
    }
    else if(action == KIRBY_ACTION_DOWNSMASHHOLD) {
        animationIndex = 35;
        mirrored = l_mirrored;
        x_mirroredOffset = -8;
        xAnimationOffset = -7;

        //  release attack
        if((!btnA && currentTime - dsmash_startTime > 150) || currentTime - dsmash_startTime > 3000) {
            action = KIRBY_ACTION_DOWNSMASH;
            frameIndex = 2;
        }
            //  charging attack
        else {
            disabledFrames = 2;
            frameExtension = 1;
            if (frameLengthCounter++ > frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 3) {
                frameIndex = 0;
            }
        }
    }
    else if(action == KIRBY_ACTION_DOWNSMASH) {
        animationIndex = 35;
        mirrored = l_mirrored;
        x_mirroredOffset = -8;
        xAnimationOffset = -7;

        disabledFrames = 2;
        frameExtension = 0;
        if (frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 12) {
            l_action = KIRBY_ACTION_DOWNSMASH;
            action = KIRBY_ACTION_RESTING;
            disabledFrames = 2;
        }
    }
    else if(action == KIRBY_ACTION_UPSPECIALINITIAL) {
        animationIndex = 20;
        mirrored = l_mirrored;
        x_mirroredOffset = -23;
        xAnimationOffset = 0;
        yAnimationOffset = 0;
        yVel = 0;
        x += airSpeed * 0.5 * joyH;

        startY = y;

        disabledFrames = 2;

        frameExtension = 0;
        if(frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
            if(frameIndex >= 2) {
                l_action = KIRBY_ACTION_UPSPECIALINITIAL;
                action = KIRBY_ACTION_UPSPECIALRISING;
            }
        }
        switch(frameIndex) {
            case 0:
                x_mirroredOffset = -20;
                xAnimationOffset = 3;
                yAnimationOffset = -15;
                break;
            case 1:
                x_mirroredOffset = -17;
                xAnimationOffset = 6;
                yAnimationOffset = -15;
                break;
            case 2:
                x_mirroredOffset = -20;
                xAnimationOffset = 3;
                yAnimationOffset = -9;
                break;
        }
    }
    else if(action == KIRBY_ACTION_UPSPECIALRISING) {
        animationIndex = 21;
        mirrored = l_mirrored;
        frameIndex = 0;
        x_mirroredOffset = -23;
        x += airSpeed * 0.5 * joyH;

        yVel = 0;
        disabledFrames = 2;
        if(y - startY > 50 || y >= ceiling) {
            l_action = KIRBY_ACTION_UPSPECIALRISING;
            action = KIRBY_ACTION_UPSPECIALTOP;
        }
        else {
            y += 16;
        }
    }
    else if(action == KIRBY_ACTION_UPSPECIALTOP) {

        if(l_action == KIRBY_ACTION_UPSPECIALRISING) frameIndex = 0;

        animationIndex = 22;
        mirrored = l_mirrored;

        yVel = 0;
        disabledFrames = 2;
        x += airSpeed * 0.5 * joyH;

        frameExtension = 0;

        if(frameLengthCounter++ > frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 5 && frameLengthCounter == frameExtension) {
            l_action = action;
            action = KIRBY_ACTION_UPSPECIALFALLING;
        }

        switch(frameIndex) {
            case 0:
                x_mirroredOffset = -23;
                xAnimationOffset = 0;
                yAnimationOffset = 0;
                break;
            case 1:
                x_mirroredOffset = -7;
                xAnimationOffset = -16;
                yAnimationOffset = 0;
                break;
            case 2:
                x_mirroredOffset = 2;
                xAnimationOffset = -25;
                yAnimationOffset = -20;
                break;
            case 3:
                x_mirroredOffset = 2;
                xAnimationOffset = -25;
                yAnimationOffset = -20;
                break;
            case 4:
                x_mirroredOffset = -8;
                xAnimationOffset = -15;
                yAnimationOffset = 0;
                break;
            case 5:
                x_mirroredOffset = -18;
                xAnimationOffset = -5;
                yAnimationOffset = 0;
                break;
        }
    }
    else if(action == KIRBY_ACTION_UPSPECIALFALLING) {

        if(l_action == KIRBY_ACTION_UPSPECIALTOP) frameIndex = 0;

        animationIndex = 23;
        mirrored = l_mirrored;

        yVel = 0;
        disabledFrames = 2;

        if(y <= floor) {
            if(!upb_projectile_active) {
                //  start projectile
                upb_projectile_active = true;
                upb_projectile_mirrored = mirrored;
                upb_projectile_startX = x;
                upb_projectile_startY = y;

                if(upb_projectile_mirrored) {
                    upb_projectile_startX -= 30;
                }
                else {
                    upb_projectile_startX += 30;
                }

                upb_projectile_x = upb_projectile_startX;
            }
            y = floor;
            frameIndex = 1;
            x_mirroredOffset = -23;
            xAnimationOffset = 50;
            yAnimationOffset = 30;
            if(frameLengthCounter++ == 10) {
                l_action = KIRBY_ACTION_UPSPECIALFALLING;
                action = KIRBY_ACTION_RESTING;

                disabledFrames = 2;
            }
        }
        else {
            frameIndex = 0;
            frameLengthCounter = 0;
            x += airSpeed * 0.5 * joyH;
            y -= 16;
        }
        switch(frameIndex) {
            case 0:
                x_mirroredOffset = -17;
                xAnimationOffset = 6;
                yAnimationOffset = 0;
                break;

            case 1:
                x_mirroredOffset = -5;
                xAnimationOffset = -8;
                yAnimationOffset = 0;
                break;
        }
    }
    else if(action == KIRBY_ACTION_NEUTRALAIR) {
        if(y <= floor) {
            action = KIRBY_ACTION_RESTING;
            disabledFrames = 3;
        }
        else {
            animationIndex = 28;
            mirrored = l_mirrored;
            yVel -= gravityFalling * 0.5;
            x += airSpeed * joyH * 0.5;
            x_mirroredOffset = 0;
            xAnimationOffset = 0;
            yAnimationOffset = 0;

            disabledFrames = 2;
            frameExtension = 0;
            if (frameLengthCounter++ > frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 8) {
                l_action = KIRBY_ACTION_NEUTRALAIR;
                action = KIRBY_ACTION_FALLING;
                disabledFrames = 2;
            }
        }
    }
    else if(action == KIRBY_ACTION_FORWARDAIR) {
        if(y <= floor) {
            action = KIRBY_ACTION_RESTING;
            disabledFrames = 3;
        }
        else {
            animationIndex = 26;
            mirrored = l_mirrored;
            yVel -= gravityFalling * 0.7;
            x += airSpeed * joyH * 0.7;

            x_mirroredOffset = -7;
            xAnimationOffset = 4;
            yAnimationOffset = 0;

            disabledFrames = 2;
            frameExtension = 0;
            if (frameLengthCounter++ > frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 8) {
                l_action = KIRBY_ACTION_FORWARDAIR;
                action = KIRBY_ACTION_FALLING;
                disabledFrames = 2;
            }
        }
    }
    else if(action == KIRBY_ACTION_UPAIR) {
        if(y <= floor) {
            action = KIRBY_ACTION_RESTING;
            disabledFrames = 3;
        }
        else {
            animationIndex = 27;
            mirrored = l_mirrored;
            yVel -= gravityFalling * 0.3;
            x += airSpeed * joyH * 0.4;
            if(yVel < -2) yVel = -2;
            else if(yVel > 2) yVel = 2;

            x_mirroredOffset = -4;
            xAnimationOffset = 0;
            yAnimationOffset = 0;

            disabledFrames = 2;
            frameExtension = 0;
            if (frameLengthCounter++ > frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 6) {
                l_action = KIRBY_ACTION_UPAIR;
                action = KIRBY_ACTION_FALLING;
                disabledFrames = 2;
            }
        }
    }
    else if(action == KIRBY_ACTION_DOWNAIR) {
        if (y <= floor) {
            action = KIRBY_ACTION_RESTING;
            disabledFrames = 3;
        } else {
            animationIndex = 25;
            mirrored = l_mirrored;
            yVel -= gravityFalling * 0.3;
            x += airSpeed * joyH * 0.4;
            if (yVel < -1) yVel = -1;
            else if (yVel > 1) yVel = 1;

            x_mirroredOffset = 3;
            xAnimationOffset = 6;
            yAnimationOffset = -10;

            disabledFrames = 2;
            frameExtension = 0;
            if (frameLengthCounter++ > frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
                frameIndex++;
            }
            if (frameIndex >= 11) {
                l_action = KIRBY_ACTION_DOWNAIR;
                action = KIRBY_ACTION_FALLING;
                disabledFrames = 2;
            }
        }
    }

    if(action == KIRBY_ACTION_FALLING) {
        if(y <= floor) {
            y = floor;
            yVel = 0;
            l_action = KIRBY_ACTION_FALLING;
            if(joyH == 0) {
                action = KIRBY_ACTION_RESTING;
                lastBlink = currentTime;
            } else {
                action = KIRBY_ACTION_RUNNING;
            }
        }
        else {
            x_mirroredOffset = 0;
            xAnimationOffset = 0;
            yAnimationOffset = 0;
            x += airSpeed * joyH;
            if(x > rightBound) x = rightBound;
            else if(x < leftBound) x = leftBound;

            yVel -= gravityFalling;

            mirrored = l_mirrored;
            if (noJumpsDisabled) {
                hitbox.offsetY(2);
                if(mirrored) hitbox.offsetX(0, mirrored);
                animationIndex = 3;
            }
            else {
                hitbox.offsetY(2);
                hitbox.offsetX(1, mirrored);
                animationIndex = 2;
            }

            frameExtension = 1;
            if (frameLengthCounter++ > frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 2) frameIndex = 0;
        }
    }
    if(action == KIRBY_ACTION_RESTING) {
        //  standing, resting

        //  mirrored facing left/right
        mirrored = l_mirrored;

        frameExtension = 0;
        animationIndex = 6;
        x_mirroredOffset = 0;
        xAnimationOffset = 0;
        yAnimationOffset = 0;

        if (l_action != KIRBY_ACTION_RESTING || lastBlink == 0) {
            lastBlink = currentTime;
        }

        if (currentTime - lastBlink > blinkPeriod) {
            if (frameLengthCounter++ > frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 7) {
                frameIndex = 0;
                lastBlink = currentTime;
            }
        } else frameIndex = 0;
    }

    l_action = action;

    //  disabled means can interrupt current action and start new action
    if(disabledFrames > 0) disabledFrames--;

    if(x > rightBound) x = rightBound;
    else if(x < leftBound) x = leftBound;



    if(!mirrored) x_mirroredOffset = 0;
    else x_mirroredOffset -= xAnimationOffset;

    SpriteSendable s;
    s.charIndex = charIndex;
    s.animationIndex = animationIndex;
    s.framePeriod = 1;
    s.frame = frameIndex;
    s.persistent = false;
    s.continuous = false;
    s.x = (int16_t) x + x_mirroredOffset + xAnimationOffset;
    s.y = (int16_t) y + yAnimationOffset;
    s.layer = LAYER_CHARACTER;
    s.mirrored = mirrored;

    UART_sendAnimation(s);

    // Up special projectile animation
    if(upb_projectile_active) {
        if(std::abs(upb_projectile_x-upb_projectile_startX) >= 70) {
            upb_projectile_active = false;
        }
        else {
            s.charIndex = charIndex;
            s.animationIndex = 19;
            s.framePeriod = 1;
            s.frame = 0;
            s.persistent = false;
            s.continuous = true;
            s.x = (int16_t) upb_projectile_x;
            s.y = (int16_t) upb_projectile_startY;
            s.layer = LAYER_CHARACTER_PROJECTILE;
            s.mirrored = upb_projectile_mirrored;

            UART_sendAnimation(s);

            if(upb_projectile_mirrored) upb_projectile_x -= 5;
            else upb_projectile_x += 5;
        }
    }

    //  update velocity and positions
    if(yVel < maxFallingVelocity) yVel = maxFallingVelocity;
    y += yVel;
    if(y > ceiling && action != KIRBY_ACTION_LEDGEGRAB) y = ceiling;
    if(y <= floor) {
        y = floor;
        jumpsUsed = 0;
    }

    if(maxHorizontalSpeed < std::abs(xVel)) {
        if(xVel < 0) xVel = -maxHorizontalSpeed;
        else xVel = maxHorizontalSpeed;
    }
    if(xVel != 0) {
        if(std::abs(xVel) < airResistance) xVel = 0;

        else if(xVel > 0) xVel -= airResistance;
        else if(xVel < 0) xVel += airResistance;
    }
    x += xVel;

    //  start any new sequences

    //  single jab
    if(disabledFrames == 0 && currentTime - l_singleJabTime > 400 && std::abs(joyH) < 0.15 && std::abs(joyV) < 0.15 &&
       currentTime - l_doubleJabTime > 400 &&
       (action == KIRBY_ACTION_RESTING) && currentTime - l_btnARise_t == 0) {
        action = KIRBY_ACTION_JABSINGLE;
        disabledFrames = 3;
        frameIndex = 0;
        frameLengthCounter = 0;
        l_singleJabTime = currentTime;

        hitboxManager->addHurtbox(x + 16, y, mirrored,
                                  jabSingle, player);
    }
        //  double jab
    else if(disabledFrames == 0 && (currentTime - l_singleJabTime < 400)
            && currentTime - l_btnARise_t == 0) {
        action = KIRBY_ACTION_JABDOUBLE;
        disabledFrames = 4;
        frameIndex = 3;
        frameLengthCounter = 0;
        l_singleJabTime = 0;
        l_doubleJabTime = currentTime;
        hitboxManager->addHurtbox(x + 16, y, mirrored,
                                  jabDouble, player);
    }
        //  repeating jab
    else if(disabledFrames == 0 && action != KIRBY_ACTION_JABREPEATING &&
            (currentTime - l_doubleJabTime < 400) && currentTime - l_btnARise_t == 0) {
        action = KIRBY_ACTION_JABREPEATING;
        disabledFrames = 2;
        frameIndex = 5;
        frameLengthCounter = 0;
    }
        //  forward smash
    else if(disabledFrames == 0 && y == floor && (action == KIRBY_ACTION_RUNNING || action == KIRBY_ACTION_RESTING)
            && currentTime-l_btnARise_t == 0 && std::abs(joyH - l_joyH) > 0.5) {
        action = KIRBY_ACTION_FORWARDSMASHHOLD;
        mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        fsmash_startTime = currentTime;
    }
        //  down smash
    else if(disabledFrames == 0 && y == floor && (action == KIRBY_ACTION_RUNNING || action == KIRBY_ACTION_RESTING)
            && currentTime-l_btnARise_t == 0 && joyV - l_joyV < -0.5) {
        action = KIRBY_ACTION_DOWNSMASHHOLD;
        mirrored = l_mirrored;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        dsmash_startTime = currentTime;
    }
        //  up smash
    else if(disabledFrames == 0 && y == floor && (action == KIRBY_ACTION_RUNNING || action == KIRBY_ACTION_RESTING)
            && currentTime-l_btnARise_t == 0 && joyV - l_joyV > 0.5) {
        action = KIRBY_ACTION_UPSMASHHOLD;
        mirrored = l_mirrored;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        usmash_startTime = currentTime;
    }
        //  neutral air
    else if(disabledFrames == 0 && y > floor
            && (action == KIRBY_ACTION_FALLING || action == KIRBY_ACTION_JUMPING
                || action == KIRBY_ACTION_MULTIJUMPING)
            && currentTime - l_btnARise_t == 0
            && std::abs(joyH) < 0.3 && std::abs(joyV) < 0.3) {
        action = KIRBY_ACTION_NEUTRALAIR;
        mirrored = l_mirrored;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  down air
    else if(disabledFrames == 0 && y > floor
            && (action == KIRBY_ACTION_FALLING || action == KIRBY_ACTION_JUMPING
                || action == KIRBY_ACTION_MULTIJUMPING)
            && currentTime - l_btnARise_t == 0 && joyV < -0.3) {
        action = KIRBY_ACTION_DOWNAIR;
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  up air
    else if(disabledFrames == 0 && y > floor
            && (action == KIRBY_ACTION_FALLING || action == KIRBY_ACTION_JUMPING
                || action == KIRBY_ACTION_MULTIJUMPING)
            && currentTime - l_btnARise_t == 0 && joyV > 0.3) {
        action = KIRBY_ACTION_UPAIR;
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  forward air
    else if(disabledFrames == 0 && y > floor
            && (action == KIRBY_ACTION_FALLING || action == KIRBY_ACTION_JUMPING
                || action == KIRBY_ACTION_MULTIJUMPING)
            && currentTime - l_btnARise_t == 0 && std::abs(joyH) > 0.3) {
        action = KIRBY_ACTION_FORWARDAIR;
        mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  up tilt
    else if(disabledFrames == 0 && y == floor && (action == KIRBY_ACTION_RUNNING || action == KIRBY_ACTION_RESTING)
            && currentTime - l_btnARise_t == 0 && joyV > 0) {
        action = KIRBY_ACTION_UPTILT;
        disabledFrames = 10;
        frameIndex = 0;
        frameLengthCounter = 0;

//        hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                  jabSingle, player);
    }
        //  forward tilt
    else if(disabledFrames == 0 && y == floor && (action == KIRBY_ACTION_RUNNING || action == KIRBY_ACTION_RESTING)
            && currentTime - l_btnARise_t == 0 && std::abs(joyH) < 0.6 && std::abs(joyH) > 0) {
        action = KIRBY_ACTION_FORWARDTILT;
        disabledFrames = 10;
        frameIndex = 0;
        frameLengthCounter = 0;

//        hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                  jabSingle, player);
    }
        //  up special
    else if(
            ( (action == KIRBY_ACTION_FALLING || action == KIRBY_ACTION_JUMPING  ||
               action == KIRBY_ACTION_MULTIJUMPING) ||
              (y == floor && (action == KIRBY_ACTION_RESTING || action == KIRBY_ACTION_RUNNING)) ) &&
            currentTime-l_btnBRise_t == 0 && joyV > 0.3) {
        action = KIRBY_ACTION_UPSPECIALINITIAL;
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }

        //  movement
        //  jumping
    else if((disabledFrames == 0 &&
            (action == KIRBY_ACTION_RESTING || action == KIRBY_ACTION_CROUCHING || action == KIRBY_ACTION_RUNNING)
            && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1 && y == floor)
            || (action == KIRBY_ACTION_LEDGEGRAB && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1
                    && disabledFrames == 0)) {
        jumpsUsed = 0;
        disabledFrames = 4;
        yVel = initialJumpSpeed;
        action = KIRBY_ACTION_JUMPING;
        frameIndex = 0;
        ledgeGrabTime = currentTime;
    }
        //  multijump
    else if( ( (disabledFrames == 0
        && (action == KIRBY_ACTION_JUMPING || action == KIRBY_ACTION_FALLING
            || action == KIRBY_ACTION_MULTIJUMPING) ) || (action == KIRBY_ACTION_FORWARDAIR) )
            && jumpsUsed < 5 && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1) {
        jumpsUsed++;
        yVel = repeatedJumpSpeed;
        action = KIRBY_ACTION_MULTIJUMPING;
        frameIndex = 0;
    }
        //  running/walking
    else if(disabledFrames == 0 && (action == KIRBY_ACTION_RESTING)
            && std::abs(joyH) > 0) {
        action = KIRBY_ACTION_RUNNING;
    }
        //  crouching
    else if(disabledFrames == 0 &&
            (action == KIRBY_ACTION_RESTING || action == KIRBY_ACTION_RUNNING) &&
            joyV <= -0.3 && y == floor) {
        action = KIRBY_ACTION_CROUCHING;
    }
        //  resting
    else if(disabledFrames == 0 &&
            joyH == 0 && joyV == 0 && action == KIRBY_ACTION_FALLING) {
        if(y == floor) action = KIRBY_ACTION_RESTING;
        else action = KIRBY_ACTION_FALLING;
    }
        //  drop down
    else if(disabledFrames == 0 && (action == KIRBY_ACTION_LEDGEGRAB) &&
        joyV < -0.3) {
        action = KIRBY_ACTION_FALLING;
        y -= 30;
        ledgeGrabTime = currentTime;
    }

    updateLastValues(joyH, joyV, btnA, btnB, shield);
}

void Kirby::updateLastValues(double joyH, double joyV, bool btnA, bool btnB, bool shield) {
    l_joyH = joyH;
    l_joyV = joyV;
    l_btnA = btnA;
    l_btnB = btnB;
    l_shield = shield;

    l_mirrored = mirrored;
}

void Kirby::collide(Hurtbox *hurtbox) {
    if(hurtbox->source == 0) {
        if(this->hitbox.y < hurtbox->y
            && currentTime - ledgeGrabTime > 1000
            && yVel <= 0) {
            action = KIRBY_ACTION_LEDGEGRAB;
            mirrored = hurtbox->damage != 0;
            yVel = 0;
            xVel = 0;
            x = hurtbox->x;
            y = hurtbox->y;
            disabledFrames = 6;
            jumpsUsed = 0;
        }
        return;
    }
}

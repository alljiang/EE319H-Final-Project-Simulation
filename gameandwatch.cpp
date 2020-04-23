//
// Created by Allen on 4/21/2020.
//

#include "entities.h"
#include "utils.h"
#include "metadata.h"
#include "UART.h"
#include "stage.h"

void GameandWatch::controlLoop(double joyH, double joyV, bool btnA, bool btnB, bool shield,
                        class Stage *stage, class HitboxManager *hitboxManager) {
    //  assume joystick deadzone filtering is already done

    double dt = 49;
    currentTime += (uint8_t)dt;

    SpriteSendable s;

    //  check if dead
    if(dead) {
        if(deathTime == 0) deathTime = currentTime;
        else if(currentTime - deathTime > 1000) {
            //  respawn

            //  reset
            l_joyH = 0;
            l_joyV = 0;
            l_btnA = 0;
            l_btnB = 0;
            l_shield = 0;
            l_mirrored = 0;
            frameIndex = 0;
            frameLengthCounter = 0;
            deathTime = 0;
            jumpsUsed = 0;

            y = 240;
            x = 159;
            invulnerableFrames = 20 * 3;

            dead = false;
            action = GAW_ACTION_FALLING;
        }
        return;
    }

    if(!l_btnA && btnA) l_btnARise_t = currentTime;
    else if(l_btnA && !btnA) l_btnAFall_t = currentTime;
    if(!l_btnB && btnB) l_btnBRise_t = currentTime;
    else if(l_btnB && !btnB) l_btnBFall_t = currentTime;
    if(!l_shield && shield) l_shieldRise_t = currentTime;
    else if(l_shield && !shield) l_shieldFall_t = currentTime;

    int16_t x_mirroredOffset = 0;

    double yAnimationOffset = 0;
    double xAnimationOffset = 0;

    bool continuous = false;

    hitbox.offsetY(0);
    hitbox.offsetX(0);
    hitbox.offsetRadius(0);

    double ceiling = stage->ceil(x + GAW_STAGE_OFFSET, y);
    double floor = stage->floor(x + GAW_STAGE_OFFSET, y);
    double leftBound = stage->leftBound(x + GAW_STAGE_OFFSET, y) - GAW_STAGE_OFFSET / 2;
    double rightBound = stage->rightBound(x - GAW_STAGE_OFFSET, y) - GAW_STAGE_OFFSET;
    double stageVelocity = stage->xVelocity(x, y);
    double gravityScale = 1;

    //  first, follow up on any currently performing actions
    noJumpsDisabled = jumpsUsed >= 5;

    //  movement
    if(action == GAW_ACTION_JUMPING) {
        hitbox.offsetY(4);
        hitbox.offsetX(4);
        gravityScale = 1;
        x += airSpeed * joyH;
        if(x > rightBound) x = rightBound;
        else if(x < leftBound) x = leftBound;

        mirrored = l_mirrored;
        animationIndex = 3;

        frameExtension = 3;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 4) {
            l_action = GAW_ACTION_JUMPING;
            action = GAW_ACTION_FALLING;
        }
    }
    else if(action == GAW_ACTION_DOUBLEJUMPING) {
        gravityScale = 1;
        x += airSpeed * joyH;
        //  give bonus speed for fighting knockback
        if((xVel < 0 && joyH > 0) ||  (xVel > 0 && joyH < 0)) { x+= airSpeed * joyH; }
        if(x > rightBound) x = rightBound;
        else if(x < leftBound) x = leftBound;
        animationIndex = 4;

        hitbox.offsetY(3);
        hitbox.offsetX(0, mirrored);

        frameExtension = 3;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 4) {
            l_action = GAW_ACTION_DOUBLEJUMPING;
            action = GAW_ACTION_FALLING;
        }
    }
    else if(action == GAW_ACTION_LEDGEGRAB) {
        animationIndex = 42;
        frameIndex = 0;

        x_mirroredOffset = -2;
        xAnimationOffset = 2;
        yAnimationOffset = -24;

        yVel = 0;

        hitbox.offsetY(-17);
        hitbox.offsetRadius(2);
        if(mirrored) hitbox.offsetX(4, mirrored);
        else hitbox.offsetX(-3, mirrored);
    }
    else if(action == GAW_ACTION_NEUTRALATTACK) {
        animationIndex = 17;
        mirrored = l_mirrored;
        x_mirroredOffset = -22;

        disabledFrames = 2;
        frameExtension = 2;

        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(0, mirrored);
        else hitbox.offsetX(-2, mirrored);

        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 2) {
            frameIndex = 0;
            frameLengthCounter = 0;
        }
        if(currentTime-l_btnARise_t > 300) {
            l_action = GAW_ACTION_NEUTRALATTACK;
            x_mirroredOffset = 0;
            action = GAW_ACTION_RESTING;

            disabledFrames = 2;
        }

//            if(frameIndex == 6 || frameIndex == 7 || frameIndex == 8 || frameIndex == 10) {
//                hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                          jabRepeating0, player);
//            }
//            else if(frameIndex == 9) {
//                hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                          jabRepeating1, player);
//            }
//            else if(frameIndex == 11) {
//                hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                          jabRepeating2, player);
    }
    else if(action == GAW_ACTION_DASHATTACK) {
        animationIndex = 13;
        mirrored = l_mirrored;
        x_mirroredOffset = 0;
        xAnimationOffset = 0;
        yAnimationOffset = 0;
        disabledFrames = 2;
        frameIndex = 0;

        double baseSpeed = 4;
        frameExtension = 3;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            if(!mirrored) x += baseSpeed * (1 + frameExtension);
            else x -= baseSpeed * (1 + frameExtension);
        }

        gravityScale = 0;
        yVel = 0;

        hitbox.offsetY(0);
        hitbox.offsetX(0);

        if(currentTime - dashAttackStartTime > 700) {
            l_action = GAW_ACTION_DASHATTACK;
            if(absVal(joyH) > 0.1) action = GAW_ACTION_RUNNING;
            else action = GAW_ACTION_RESTING;
            disabledFrames = 10;
        }
        else {
//            if(frameIndex > 1) {
//                hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                          dashAttack, player);
//            }
        }
    }
    else if(action == GAW_ACTION_DOWNTILT) {
        animationIndex = 12;
        mirrored = l_mirrored;
        x_mirroredOffset = -20;
        disabledFrames = 2;

        hitbox.offsetY(-6);
        hitbox.offsetX(2);
        hitbox.offsetRadius(-3);

        frameExtension = 3;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 4) {
            l_action = GAW_ACTION_DOWNTILT;
            if(joyV < -0.25) {
                action = GAW_ACTION_CROUCHING;
            }
            else action = GAW_ACTION_RESTING;
            frameIndex = 0;
            disabledFrames = 4;
            x_mirroredOffset = 0;
        }
        else {
//            hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                      downTilt, player);
        }
    }
    else if(action == GAW_ACTION_FORWARDTILT) {
        animationIndex = 10;
        mirrored = l_mirrored;
        x_mirroredOffset = -20;

        hitbox.offsetY(0);
        hitbox.offsetX(-3, mirrored);

        if(frameIndex == 0) frameExtension = 4;
        else if(frameIndex == 1) frameExtension = 7;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 2) {
            l_action = GAW_ACTION_FORWARDTILT;
            action = GAW_ACTION_RESTING;
            disabledFrames = 2;
            x_mirroredOffset = 0;
        }
        else {
//            if(frameIndex < 3)
//                hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                          forwardTilt, player);
        }
    }
    else if(action == GAW_ACTION_UPTILT) {
        animationIndex = 11;
        mirrored = l_mirrored;
        x_mirroredOffset = -12;
        xAnimationOffset = -16;

        hitbox.offsetY(4);
        hitbox.offsetX(2, mirrored);
//        if(mirrored) hitbox.offsetX(0, mirrored);
//        else hitbox.offsetX(0, mirrored);

        frameExtension = 3;
        if(frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 3) {
            l_action = GAW_ACTION_UPTILT;
            action = GAW_ACTION_RESTING;
            disabledFrames = 6;
            x_mirroredOffset = 0;
        }
        else {
//            if(frameIndex < 4)
//                hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                          upTilt, player);
        }
    }
    else if(action == GAW_ACTION_FORWARDSMASHHOLD) {
        animationIndex = 14;
        mirrored = l_mirrored;
        x_mirroredOffset = -20;

        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(0, mirrored);
        else hitbox.offsetX(-3, mirrored);

        //  release attack
        if((!btnA && currentTime - fsmash_startTime > 300) || currentTime - fsmash_startTime > 3000) {
            action = GAW_ACTION_FORWARDSMASH;
            frameIndex = 2;
        }
            //  charging attack
        else {
            disabledFrames = 2;
            frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 2) {
                frameIndex = 0;
            }
        }
    }
    else if(action == GAW_ACTION_FORWARDSMASH) {
        animationIndex = 14;
        mirrored = l_mirrored;
        x_mirroredOffset = -20;

        disabledFrames = 2;
        frameExtension = 6;
        if (frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 4) {
            l_action = GAW_ACTION_FORWARDSMASH;
            action = GAW_ACTION_RESTING;
            disabledFrames = 5;

//            if(mirrored) x -= 16;
//            else x += 23;
//            x_mirroredOffset = 0;
        }
        else {
//            if(frameIndex > 6) {
//                double chargeScale = (currentTime - fsmash_startTime) / 3000. * 0.6 + 1;
//                hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                          forwardSmash, player, chargeScale);
//            }
        }
    }
    else if(action == GAW_ACTION_UPSMASHHOLD) {
        animationIndex = 16;
        mirrored = l_mirrored;
        x_mirroredOffset = -10;
        xAnimationOffset = -12;

        hitbox.offsetY(-3);
        hitbox.offsetX(0);

        //  release attack
        if((!btnA && currentTime - usmash_startTime > 300) || currentTime - usmash_startTime > 3000) {
            action = GAW_ACTION_UPSMASH;
            frameIndex = 2;
        }
            //  charging attack
        else {
            disabledFrames = 2;
            frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 2) {
                frameIndex = 0;
            }
        }
    }
    else if(action == GAW_ACTION_UPSMASH) {
        animationIndex = 16;
        mirrored = l_mirrored;
        x_mirroredOffset = -10;
        xAnimationOffset = -12;

        hitbox.offsetY(3);
        hitbox.offsetX(0);

        disabledFrames = 2;
        frameExtension = 4;
        if(frameIndex == 3) frameExtension = 8;
        if (frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 4) {
            l_action = GAW_ACTION_UPSMASH;
            action = GAW_ACTION_RESTING;
            disabledFrames = 2;
        }
        else {
            double chargeScale = (currentTime - usmash_startTime) / 3000. * 0.6 + 1;
//            if(frameIndex > 4 && frameIndex < 7) {
//                hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                          upSmash, player, chargeScale);
//            }
        }
    }
    else if(action == GAW_ACTION_DOWNSMASHHOLD) {
        animationIndex = 15;
        mirrored = l_mirrored;
        x_mirroredOffset = -17;
        xAnimationOffset = -17;

        hitbox.offsetY(3);
        hitbox.offsetX(0);

        //  release attack
        if((!btnA && currentTime - dsmash_startTime > 300) || currentTime - dsmash_startTime > 3000) {
            action = GAW_ACTION_DOWNSMASH;
            frameIndex = 2;
        }
            //  charging attack
        else {
            disabledFrames = 2;
            frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 2) {
                frameIndex = 0;
            }
        }
    }
    else if(action == GAW_ACTION_DOWNSMASH) {
        animationIndex = 15;
        mirrored = l_mirrored;
        x_mirroredOffset = -17;
        xAnimationOffset = -17;

        hitbox.offsetY(-3);
        hitbox.offsetX(0);

        disabledFrames = 2;
        frameExtension = 7;
        if (frameLengthCounter++ >= frameExtension) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 3) {
            l_action = GAW_ACTION_DOWNSMASH;
            action = GAW_ACTION_RESTING;
            disabledFrames = 5;
        }
        else {
//            if(frameIndex > 1) {
//                double chargeScale = (currentTime - dsmash_startTime) / 3000. * 0.6 + 1;
//                hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                          downSmash, player, chargeScale);
//            }
        }
    }
    else if(action == GAW_ACTION_FORWARDAIR) {
        if(y <= floor) {
            action = GAW_ACTION_RESTING;
            disabledFrames = 4;
        }
        else {
            animationIndex = 21;
            mirrored = l_mirrored;
            gravityScale = 1.0;
            x += airSpeed * joyH * 0.7;

            x_mirroredOffset = -14;
            xAnimationOffset = -1;
            yAnimationOffset = 0;

            hitbox.offsetY(6);
            if(mirrored) hitbox.offsetX(0);
            else hitbox.offsetX(2);

            disabledFrames = 2;
            if(frameIndex == 1) frameExtension = 5;
            else frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 3) {
                l_action = GAW_ACTION_FORWARDAIR;
                action = GAW_ACTION_FALLING;
                disabledFrames = 4;
            }
            else {
//                if(frameIndex > 1) {
//                    hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                              forwardAir, player);
//                }
            }
        }
    }
    else if(action == GAW_ACTION_BACKAIR) {
        if(y <= floor) {
            action = GAW_ACTION_RESTING;
            disabledFrames = 4;
        }
        else {
            animationIndex = 22;
            mirrored = l_mirrored;
            gravityScale = 0.8;
            x += airSpeed * joyH * 0.7;

            x_mirroredOffset = 0;
            xAnimationOffset = -23;
            yAnimationOffset = 0;

            hitbox.offsetY(3);
            if(mirrored) hitbox.offsetX(-1);
            else hitbox.offsetX(0);

            disabledFrames = 2;
            frameExtension = 3;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 3) {
                l_action = GAW_ACTION_BACKAIR;
                action = GAW_ACTION_FALLING;
                disabledFrames = 0;
            }
            else {
//                if(frameIndex > 1 && frameIndex < 5) {
//                    hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                              backAir, player);
//                }
            }
        }
    }
    else if(action == GAW_ACTION_UPAIR) {
        if(y <= floor) {
            action = GAW_ACTION_RESTING;
            disabledFrames = 4;
        }
        else {
            animationIndex = 24;
            mirrored = l_mirrored;
            gravityScale = 1.0;
            x += airSpeed * joyH * 0.4;
            disabledFrames = 2;

            x_mirroredOffset = -4;
            xAnimationOffset = 0;
            yAnimationOffset = 0;

            hitbox.offsetY(9);
            if(mirrored) hitbox.offsetX(3);
            else hitbox.offsetX(2);

            frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 5) {
                l_action = GAW_ACTION_UPAIR;
                action = GAW_ACTION_FALLING;
                disabledFrames = 5;
            }
            else {
//                if(frameIndex > 1 && frameIndex < 5) {
//                    hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                              upAir, player);
//                }
            }
        }
    }
    else if(action == GAW_ACTION_DOWNAIR) {
        animationIndex = 23;
        mirrored = l_mirrored;
        x_mirroredOffset = 0;
        xAnimationOffset = 0;
        yAnimationOffset = 0;

        disabledFrames = 2;

        if(frameIndex == 0) {
            frameExtension = 1;
            gravityScale = 0;
            yVel = 0;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
        }
        if(frameIndex == 1) {
            if(y <= floor) {
                y = floor;
                frameIndex++;
                frameLengthCounter = 0;
            }
            else {
                gravityScale = 6.0;
                if(yVel < -12) yVel = -12;
                overrideMaxVelocity = true;
                xVel = 0;
//                hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                          neutralAir, player);
            }
        }
        if(frameIndex == 2) {
            frameExtension = 7;
            xAnimationOffset = -12;
            x_mirroredOffset = 10;
            if(frameLengthCounter++ >= frameExtension) {
                action = GAW_ACTION_RESTING;
                frameIndex++;
                disabledFrames = 1;
            }
        }
        if(frameIndex < 3) {}
        else {
            hitbox.offsetY(6);
            if(mirrored) hitbox.offsetX(2);
            else hitbox.offsetX(1);
        }
    }
    else if(action == GAW_ACTION_NEUTRALAIR) {
        if(y <= floor) {
            action = GAW_ACTION_RESTING;
            disabledFrames = 4;
        }
        else {
            animationIndex = 20;
            mirrored = l_mirrored;
            gravityScale = 0.5;
            x += airSpeed * joyH * 0.5;
            x_mirroredOffset = -20;
            xAnimationOffset = -17;
            yAnimationOffset = 0;

            hitbox.offsetY(6);
            if(mirrored) hitbox.offsetX(2);
            else hitbox.offsetX(1);

            disabledFrames = 2;
            frameExtension = 3;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 3) {
                l_action = GAW_ACTION_NEUTRALAIR;
                action = GAW_ACTION_FALLING;
                disabledFrames = 2;
            }
            else {
//                hitboxManager->addHurtbox(x + 16, y, mirrored,
//                                          neutralAir, player);
            }
        }
    }
    if(action == GAW_ACTION_CROUCHING) {

        animationIndex = 6;
        frameIndex = 0;

        hitbox.offsetY(-6);
        hitbox.offsetX(0, mirrored);
        hitbox.offsetRadius(-2);

        if(joyV > -0.3) {
            action = GAW_ACTION_RESTING;
            lastBlink = currentTime;
        }
    }
    if(action == GAW_ACTION_FALLING) {
        if(y <= floor) {
            y = floor;
            yVel = 0;
            l_action = GAW_ACTION_FALLING;
            if(joyH == 0) {
                action = GAW_ACTION_RESTING;
                lastBlink = currentTime;
            } else {
                action = GAW_ACTION_RUNNING;
            }
        }
        else {
            x_mirroredOffset = 0;
            xAnimationOffset = 0;
            yAnimationOffset = 0;
            x += airSpeed * joyH;
            if(x > rightBound) x = rightBound;
            else if(x < leftBound) x = leftBound;

            gravityScale = 1;

            mirrored = l_mirrored;
            hitbox.offsetY(2);
            if(mirrored) hitbox.offsetX(1, mirrored);
            else hitbox.offsetX(0, mirrored);
            animationIndex = 5;

            frameExtension = 2;
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 2) frameIndex = 0;
        }
    }
    if(action == GAW_ACTION_RESTING) {
        //  standing, resting

        //  mirrored facing left/right
        mirrored = l_mirrored;

        frameExtension = 8;
        animationIndex = 0;
        x_mirroredOffset = 0;
        xAnimationOffset = 0;
        yAnimationOffset = 0;

        hitbox.offsetY(0);
        if(mirrored) hitbox.offsetX(1, mirrored);
        else hitbox.offsetX(0, mirrored);

        if (l_action != GAW_ACTION_RESTING || lastBlink == 0) {
            lastBlink = currentTime;
        }

        if (currentTime - lastBlink > blinkPeriod) {
            if (frameLengthCounter++ >= frameExtension) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 2) {
                frameIndex = 0;
                lastBlink = currentTime;
            }
        } else frameIndex = 0;
    }
    if(action == GAW_ACTION_RUNNING) {
        if(y != floor) {
            action = GAW_ACTION_FALLING;
        }
        else if(joyH == 0) {
            action = GAW_ACTION_RESTING;
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
            //  dash
            if (absVal(joyH) > 0.6) {
                frameExtension = (uint8_t) ((4 - absVal(0.7 * joyH)));
                if (frameLengthCounter++ > frameExtension) {
                    frameLengthCounter = 0;
                    frameIndex++;
                }
                if (frameIndex >= 2) frameIndex = 0;
                animationIndex = 2;
                hitbox.offsetY(2);
                hitbox.offsetX(0, mirrored);

                xAnimationOffset = 0;
                yAnimationOffset = 0;
                x_mirroredOffset = 0;
                // walk
            } else {
                frameExtension = (uint8_t) ((-2 * absVal(joyH)) + 5);
                if (frameLengthCounter++ > frameExtension) {
                    frameLengthCounter = 0;
                    frameIndex++;
                }
                if (frameIndex >= 3) frameIndex = 1;
                animationIndex = 1;
                hitbox.offsetY(1);
                hitbox.offsetX(-1, mirrored);

                xAnimationOffset = 0;
                yAnimationOffset = 0;
                x_mirroredOffset = 0;
            }
        }
    }

    l_action = action;

    //  disabled means can interrupt current action and start new action
    if(disabledFrames > 0) disabledFrames--;
    if(invulnerableFrames > 0) invulnerableFrames--;
    if(disabledFrames == -1) {
        //  knockback stun, remove stun when falling or on floor
        if(y == floor || yVel < 0) {
            if(y > floor) action = GAW_ACTION_FALLING;
            disabledFrames = 0;
        }
    }

    if(x > rightBound) x = rightBound;
    else if(x < leftBound) x = leftBound;

    this->hitbox.initialize(x+16, y+10, SHAPE_CIRCLE, 12);

    if(!mirrored) x_mirroredOffset = 0;
    else x_mirroredOffset -= xAnimationOffset;

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

    //  update velocity and positions
    if(!overrideMaxVelocity && yVel < maxFallingVelocity) yVel = maxFallingVelocity;
    overrideMaxVelocity = false;

    y += yVel;
    if(y > ceiling && action != GAW_ACTION_LEDGEGRAB) y = ceiling;
    if(y <= floor) {
        y = floor;
        jumpsUsed = 0;
    }

    if(yVel > 0) yVel -= gravityRising * gravityScale;
    else yVel -= gravityFalling * gravityScale;

    if(maxHorizontalSpeed < absVal(xVel)) {
        if(xVel < 0) xVel = -maxHorizontalSpeed;
        else xVel = maxHorizontalSpeed;
    }
    if(xVel != 0) {
        if(x == floor) xVel *= groundFriction;

        if(absVal(xVel) < airResistance) xVel = 0;

        else if(xVel > 0) xVel -= airResistance;
        else if(xVel < 0) xVel += airResistance;
    }
    x += xVel;
    //  start any new sequences
    //  neutral attack
    if(disabledFrames == 0 && absVal(joyH) < 0.15 && absVal(joyV) < 0.15 &&
       (action == GAW_ACTION_RESTING) && currentTime - l_btnARise_t == 0) {
        action = GAW_ACTION_NEUTRALATTACK;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;

    }
        //  forward smash
    else if(disabledFrames == 0 && y == floor && (action == GAW_ACTION_RUNNING || action == GAW_ACTION_RESTING)
            && currentTime-l_btnARise_t == 0 && absVal(joyH - l_joyH) > 0.5) {
        action = GAW_ACTION_FORWARDSMASHHOLD;
        mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        fsmash_startTime = currentTime;
    }
        //  down smash
    else if(disabledFrames == 0 && y == floor && (action == GAW_ACTION_RUNNING || action == GAW_ACTION_RESTING)
            && currentTime-l_btnARise_t == 0 && joyV - l_joyV < -0.5) {
        action = GAW_ACTION_DOWNSMASHHOLD;
        mirrored = l_mirrored;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        dsmash_startTime = currentTime;
    }
        //  up smash
    else if(disabledFrames == 0 && y == floor && (action == GAW_ACTION_RUNNING || action == GAW_ACTION_RESTING)
            && currentTime-l_btnARise_t == 0 && joyV - l_joyV > 0.5) {
        action = GAW_ACTION_UPSMASHHOLD;
        mirrored = l_mirrored;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        usmash_startTime = currentTime;
    }
        //  down tilt
    else if(disabledFrames == 0 && y == floor && (GAW_ACTION_CROUCHING || action == GAW_ACTION_RESTING)
            && currentTime - l_btnARise_t == 0 && joyV < -0.3) {
        action = GAW_ACTION_DOWNTILT;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  neutral air
    else if(disabledFrames == 0 && y > floor
            && (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING
                || action == GAW_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0
            && absVal(joyH) < 0.3 && absVal(joyV) < 0.3) {
        action = GAW_ACTION_NEUTRALAIR;
        mirrored = l_mirrored;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  down air
    else if(disabledFrames == 0 && y > floor
            && (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING
                || action == GAW_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0 && joyV < -0.3) {
        action = GAW_ACTION_DOWNAIR;
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  up air
    else if(disabledFrames == 0 && y > floor
            && (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING
                || action == GAW_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0 && joyV > 0.3) {
        action = GAW_ACTION_UPAIR;
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  forward air
    else if(disabledFrames == 0 && y > floor
            && (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING
                || action == GAW_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0
            && ( (joyH > 0.3 && !mirrored) || (joyH < -0.3 && mirrored) ) ) {
        action = GAW_ACTION_FORWARDAIR;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  back air
    else if(disabledFrames == 0 && y > floor
            && (action == GAW_ACTION_FALLING || action == GAW_ACTION_JUMPING
                || action == GAW_ACTION_DOUBLEJUMPING)
            && currentTime - l_btnARise_t == 0
            && ( (joyH < -0.3 && !mirrored) || (joyH > 0.3 && mirrored) ) ) {
        action = GAW_ACTION_BACKAIR;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  forward tilt
    else if(disabledFrames == 0 && y == floor && (action == GAW_ACTION_RUNNING || action == GAW_ACTION_RESTING)
            && currentTime - l_btnARise_t == 0 && absVal(joyH) < 0.6 && absVal(joyH) > 0.1) {
        action = GAW_ACTION_FORWARDTILT;
        mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  up tilt
    else if(disabledFrames == 0 && y == floor && (action == GAW_ACTION_RUNNING || action == GAW_ACTION_RESTING)
            && currentTime - l_btnARise_t == 0 && joyV > 0) {
        action = GAW_ACTION_UPTILT;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
    }
        //  dash attack
    else if(disabledFrames == 0 && y == floor && action == GAW_ACTION_RUNNING
            && currentTime - l_btnARise_t == 0 && absVal(joyH) > 0.4) {
        action = GAW_ACTION_DASHATTACK;
        mirrored = joyH < 0;
        disabledFrames = 2;
        frameIndex = 0;
        frameLengthCounter = 0;
        dashAttackStartTime = currentTime;
    }

        //  movement
        //  jumping
    else if((disabledFrames == 0 &&
             (action == GAW_ACTION_RESTING || action == GAW_ACTION_CROUCHING || action == GAW_ACTION_RUNNING
              || action == GAW_ACTION_HURT)
             && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1 && y == floor)
            || (action == GAW_ACTION_LEDGEGRAB && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1
                && disabledFrames == 0)) {
        jumpsUsed = 0;
        disabledFrames = 4;
        yVel = initialJumpSpeed;
        l_action = action;
        action = GAW_ACTION_JUMPING;
        frameIndex = 0;
        ledgeGrabTime = currentTime;
    }
        //  double jump
    else if( disabledFrames == 0
             && ( (action == GAW_ACTION_JUMPING || action == GAW_ACTION_FALLING
                   || action == GAW_ACTION_DOUBLEJUMPING
                   || (action == GAW_ACTION_FORWARDAIR || action == GAW_ACTION_BACKAIR|| action == GAW_ACTION_UPAIR) )
                  && jumpsUsed == 0 && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1) ) {
        jumpsUsed++;
        yVel = repeatedJumpSpeed;
        l_action = action;
        action = GAW_ACTION_DOUBLEJUMPING;
        frameIndex = 0;

        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
    }
        //  running/walking
    else if(disabledFrames == 0 && (action == GAW_ACTION_RESTING || action == GAW_ACTION_HURT)
            && absVal(joyH) > 0) {
        l_action = action;
        action = GAW_ACTION_RUNNING;
    }
        //  crouching
    else if(disabledFrames == 0 &&
            (action == GAW_ACTION_RESTING || action == GAW_ACTION_RUNNING || action == GAW_ACTION_HURT) &&
            joyV <= -0.3 && y == floor) {
        l_action = action;
        action = GAW_ACTION_CROUCHING;
    }
        //  resting
    else if(disabledFrames == 0 &&
            joyH == 0 && joyV == 0 && (action == GAW_ACTION_FALLING || action == GAW_ACTION_HURT)) {
        l_action = action;
        if(y == floor) action = GAW_ACTION_RESTING;
        else action = GAW_ACTION_FALLING;
    }

    updateLastValues(joyH, joyV, btnA, btnB, shield);
}

void GameandWatch::updateLastValues(double joyH, double joyV, bool btnA, bool btnB, bool shield) {
    l_joyH = joyH;
    l_joyV = joyV;
    l_btnA = btnA;
    l_btnB = btnB;
    l_shield = shield;

    l_mirrored = mirrored;
}

void GameandWatch::collide(Hurtbox *hurtbox, Player *otherPlayer) {
    //  ledge grab
    if(hurtbox->source == 0) {
        if(this->hitbox.y < hurtbox->y
           && currentTime - ledgeGrabTime > 1000
           && yVel <= 0) {
            action = GAW_ACTION_LEDGEGRAB;
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

        // only knockback if not currently knocked back
    else if(disabledFrames != -1 && invulnerableFrames == 0) {
//        disabledFrames = hurtbox->stunFrames;
//        damage += hurtbox->damage;
//
//        double knockbackMultiplier = damage / 200. + 1.0;
////        printf("%0.1f\n", damage);
//
//        if (otherPlayer->x < x) xVel = hurtbox->xKnockback * knockbackMultiplier;
//        else xVel = -hurtbox->xKnockback * knockbackMultiplier;
//        yVel = hurtbox->yKnockback * knockbackMultiplier;
//
//        action = GAW_ACTION_HURT;
    }
}

void GameandWatch::reset() {
    yVel = 0;
    xVel = 0;
    currentTime = 0;
    damage = 0;
    action = 0;
    l_action = 0;
    frameIndex = 0;
    frameExtension = 1;
    frameLengthCounter = 0;
    deathTime = 0;
    disabledFrames = 0;
    invulnerableFrames = 0;
    noJumpsDisabled = false;

    l_joyV = 0;
    l_joyH = 0;
    l_btnA = false;
    l_btnARise_t = -1;
    l_btnAFall_t = -1;
    l_btnB = false;
    l_btnBRise_t = -1;
    l_btnBFall_t = -1;
    l_shield = false;
    l_shieldRise_t = -1;
    l_shieldFall_t = -1;

    fsmash_startTime = 0;
    dsmash_startTime = 0;
    usmash_startTime = 0;
    ledgeGrabTime = 0;
    jumpsUsed = 0;

    dead = false;
}



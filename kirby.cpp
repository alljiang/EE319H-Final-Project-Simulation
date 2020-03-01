//
// Created by Allen on 2/28/2020.
//

#include <cstdio>
#include "entities.h"
#include "utils.h"
#include "metadata.h"
#include "UART.h"

void Kirby::controlLoop(double joyH, double joyV, bool btnA, bool btnB, bool shield) {
    //  assume joystick deadzone filtering is already done

    long long currentTime = millis();
    double dt = millis() - l_time;
    l_time = currentTime;

    if(!l_btnA && btnA) l_btnARise_t = currentTime;
    else if(l_btnA && !btnA) l_btnAFall_t = currentTime;
    if(!l_btnB && btnB) l_btnBRise_t = currentTime;
    else if(l_btnB && !btnB) l_btnBFall_t = currentTime;
    if(!l_shield && shield) l_shieldRise_t = currentTime;
    else if(l_shield && !shield) l_shieldFall_t = currentTime;

    int16_t x_mirroredOffset = 0;

    //  first, follow up on any currently performing actions

    //  movement
    if(action == ACTION_RUNNING) {

        //  x position
        x += joyH * groundSpeed / dt;

        //  mirrored facing left/right
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;

        if(std::abs(joyH) > 0.6) {
            framePeriod = (uint8_t) ((3-std::abs(2*joyH) ));
            if(frameLengthCounter++ > framePeriod) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 8) frameIndex = 0;
            animationIndex = 1;
        }
        else {
            framePeriod = (uint8_t) ((-8*std::abs(joyH)) + 8);
            if(frameLengthCounter++ > framePeriod) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 12) frameIndex = 0;
            animationIndex = 9;
        }
        continuous = false;
    }
    else if(action == ACTION_FALLING) {
        if(y <= 0) {
            y = 0;
            l_action = ACTION_FALLING;
            action = ACTION_RESTING;
            lastBlink = currentTime;
            yVel = 0;
        }
        else {
            x += airSpeed * joyH;

            noJumpsDisabled = jumpsUsed >= 5;
            yVel -= gravityFalling;

            mirrored = l_mirrored;
            if (noJumpsDisabled) animationIndex = 3;
            else animationIndex = 2;

            framePeriod = 3;
            if (frameLengthCounter++ > framePeriod) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if (frameIndex >= 2) frameIndex = 0;
        }
    }
    else if(action == ACTION_JUMPING) {
        yVel -= gravityRising;
        x += airSpeed * joyH;

        mirrored = l_mirrored;
        animationIndex = 4;

        framePeriod = 3;
        if(frameLengthCounter++ > framePeriod) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 7) {
            l_action = ACTION_JUMPING;
            action = ACTION_FALLING;
        }
    }
    else if(action == ACTION_MULTIJUMPING) {
        yVel -= gravityRising;
        x += airSpeed * joyH;

        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;
        animationIndex = 5;

        framePeriod = 3;
        if(frameLengthCounter++ > framePeriod) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 3) {
            l_action = ACTION_MULTIJUMPING;
            action = ACTION_FALLING;
        }
    }
    else if(action == ACTION_CROUCHING) {
        animationIndex = 0;
        frameIndex = 0;

        if(joyV > -0.3) {
            action = ACTION_RESTING;
            lastBlink = currentTime;
        }
    }
    //  regular attacks, ground
    else if(action == ACTION_JABSINGLE) {
        animationIndex = 10;
        mirrored = l_mirrored;
        x_mirroredOffset = -22;

        framePeriod = 3;
        if(frameLengthCounter++ > framePeriod) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 3) {
            l_action = ACTION_JABSINGLE;
            action = ACTION_RESTING;
            x_mirroredOffset = 0;
        }
    }
    else if(action == ACTION_JABDOUBLE) {
        animationIndex = 10;
        mirrored = l_mirrored;
        x_mirroredOffset = -22;

        framePeriod = 4;
        if(frameLengthCounter++ > framePeriod) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 5) {
            l_action = ACTION_JABDOUBLE;
            action = ACTION_RESTING;
            x_mirroredOffset = 0;
        }
    }
    else if(action == ACTION_JABREPEATING) {
        animationIndex = 10;
        mirrored = l_mirrored;
        x_mirroredOffset = -22;

        disabledFrames = 2;
        framePeriod = 3;
        if(frameLengthCounter++ > framePeriod) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 12) {
            frameIndex = 6;
            frameLengthCounter = 0;
        }
        if(currentTime-l_btnARise_t > 500) {
            l_action = ACTION_JABREPEATING;
            x_mirroredOffset = 0;
            action = ACTION_RESTING;
            l_repeatJab = currentTime;

            disabledFrames = 0;
        }

    }
    //  special attacks, ground
    //  regular attacks, air
    //  special attacks, air
    if(action == ACTION_RESTING) {
        //  standing, resting

        //  mirrored facing left/right
        mirrored = l_mirrored;

        framePeriod = 1;
        continuous = false;
        animationIndex = 6;

        if (l_action != ACTION_RESTING || lastBlink == 0) {
            lastBlink = currentTime;
        }

        if (currentTime - lastBlink > blinkPeriod) {
            if (frameLengthCounter++ > framePeriod) {
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

    //  start any new sequences
    //  attacks
    //  single jab

    if(disabledFrames == 0 && currentTime - l_singleJab > 300 &&
    currentTime - l_doubleJab > 300 &&
    (action == ACTION_RESTING) && currentTime - l_btnARise_t == 0) {
        action = ACTION_JABSINGLE;
        disabledFrames = 9;
        frameIndex = 0;
        frameLengthCounter = 0;
        l_singleJab = currentTime;
    }
    //  double jab
    else if(disabledFrames == 0 && (currentTime - l_doubleJab > 300) &&
    (currentTime - l_singleJab < 300) && currentTime - l_btnARise_t == 0) {
        action = ACTION_JABDOUBLE;
        disabledFrames = 8;
        frameIndex = 3;
        frameLengthCounter = 0;
        l_doubleJab = currentTime;
    }
    //  repeating jab
    else if(disabledFrames == 0 && action != ACTION_JABREPEATING &&
    (currentTime - l_doubleJab < 400) && currentTime - l_btnARise_t == 0) {
        action = ACTION_JABREPEATING;
        printf("repeat\n");
        disabledFrames = 18;
        frameIndex = 5;
        frameLengthCounter = 0;
        l_repeatJab = currentTime;
    }

    //  movement
    //  jumping
    else if(disabledFrames == 0 &&
    (action == ACTION_RESTING || action == ACTION_CROUCHING || action == ACTION_RUNNING)
    && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1) {
        jumpsUsed = 0;
        yVel = initialJumpSpeed;
        action = ACTION_JUMPING;
        frameIndex = 0;
    }
    //  multijump
    else if(disabledFrames == 0 &&
    (action == ACTION_JUMPING ||  action == ACTION_FALLING || action == ACTION_MULTIJUMPING)
            && jumpsUsed < 5 && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1) {
        jumpsUsed++;
        yVel = repeatedJumpSpeed;
        action = ACTION_MULTIJUMPING;
        frameIndex = 0;
    }
    //  running/walking
    else if(disabledFrames == 0 && (action == ACTION_RESTING)
        && std::abs(joyH) > 0) {
        action = ACTION_RUNNING;
    }
    //  crouching
    else if(disabledFrames == 0 &&
    (action == ACTION_RESTING || action == ACTION_RUNNING) &&
        joyV <= -0.3 && y == 0) {
        action = ACTION_CROUCHING;
    }
    //  resting
    else if(disabledFrames == 0 &&
            joyH == 0 && joyV == 0 && y == 0) {
        action = ACTION_RESTING;
    }

    if(yVel < maxFallingVelocity) yVel = maxFallingVelocity;
    y += yVel;

    if(!mirrored) x_mirroredOffset = 0;

    SpriteSendable s;
    s.charIndex = charIndex;
    s.animationIndex = animationIndex;
    s.framePeriod = 1;
    s.frame = frameIndex;
    s.persistent = false;
    s.x = (int16_t) x + x_mirroredOffset;
    s.y = (int16_t) y;
    s.layer = LAYER_CHARACTER;
    s.mirrored = mirrored;

    UART_sendAnimation(s);

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
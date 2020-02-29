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

    //  first, follow up on any currently performing actions

    //  disabled means can interrupt current action and start new action
    if(disabledFrames > 0) disabledFrames--;

    //  movement
    if(walking) {

        //  x position
        x += joyH * groundSpeed / dt;

        //  mirrored facing left/right
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;

        //  if just started walking or on last frame, reset frame to 0
        framePeriod = (uint8_t) ((-8*std::abs(joyH)) + 8);
        if(frameLengthCounter++ > framePeriod) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 12 || !l_walking) frameIndex = 0;
        continuous = false;
        animationIndex = 9;
    }
    else if(running) {

        //  x position
        x += joyH * groundSpeed / dt;

        //  mirrored facing left/right
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;

        //  if just started walking or on last frame, reset frame to 0
        framePeriod = (uint8_t) ((3-std::abs(2*joyH) ));
        if(frameLengthCounter++ > framePeriod) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 8 || !l_running) frameIndex = 0;
        continuous = false;
        animationIndex = 1;
    }
    else if(falling) {
        if(y <= 0) {
            y = 0;
            falling = false;
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
    else if(jumping) {
        yVel -= gravityRising;
        x += airSpeed * joyH;

        mirrored = l_mirrored;
        animationIndex = 4;

        framePeriod = 1;
        if(frameLengthCounter++ > framePeriod) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 7) {
            jumping = false;
            falling = true;
        }
    }
    else if(multijumping) {
        yVel -= gravityRising;
        x += airSpeed * joyH;

        mirrored = l_mirrored;
        animationIndex = 5;

        framePeriod = 1;
        if(frameLengthCounter++ > framePeriod) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 3) {
            multijumping = false;
            falling = true;
        }
    }
    else if(crouching) {
        animationIndex = 0;

        if(joyV >= -0.3) crouching = false;
    }
    //  regular attacks, ground
    else if(jabbingInitial) {
    }
    else if(jabbingRepeating) {
    }
    //  special attacks, ground
    //  regular attacks, air
    //  special attacks, air
    else {
        //  standing, resting

        //  mirrored facing left/right
        mirrored = l_mirrored;

        framePeriod = 1;
        continuous = false;
        animationIndex = 6;

        if(currentTime - lastBlink > blinkPeriod) {
            if(frameLengthCounter++ > framePeriod) {
                frameLengthCounter = 0;
                frameIndex++;
            }
            if(frameIndex >= 7) {
                frameIndex = 0;
                lastBlink = currentTime;
            }
        }
        else frameIndex = 0;
    }

    //  start any new sequences

    //  attacks
    if(millis() - l_btnARise_t == 0) {}

    //  movement
    //  jumping
    else if(!jumping && !multijumping && yVel == 0 && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1) {
        jumpsUsed = 0;
        yVel = initialJumpSpeed;
        jumping = true;
        multijumping = false;
        running = false;
        walking = false;
        frameIndex = 0;
    }
    //  multijump
    else if((jumping || multijumping || falling) && jumpsUsed < 5
            && (joyV - l_joyV) > joystickJumpSpeed && l_joyV > -0.1) {
        jumpsUsed++;
        yVel = repeatedJumpSpeed;
        multijumping = true;
        falling = false;
        jumping = false;
        frameIndex = 0;
    }
    //  running/walking
    else if(!jumping && !multijumping && !falling && std::abs(joyH) > 0) {
        if(std::abs(joyH) > 0.6) {
            running = true;
            walking = false;
        }
        else if(std::abs(joyH) > 0.1) {
            walking = true;
            running = false;
        }
    }
    //  crouching
    else if(joyV <= -0.3 && y == 0) {
        crouching = true;
    }
    //  standing
    else if(joyH == 0 && joyV == 0 && yVel == 0) {
        running = false;
        walking = false;
        if(l_running || l_walking) {
            lastBlink = currentTime;
        }
    }

    y += yVel;

    SpriteSendable s;
    s.charIndex = charIndex;
    s.animationIndex = animationIndex;
    s.framePeriod = 1;
    s.frame = frameIndex;
    s.persistent = false;
    s.x = (int16_t) x;
    s.y = (int16_t) y;
    s.layer = LAYER_CHARACTER;
    s.mirrored = mirrored;

    UART_sendAnimation(s);

    updateLastValues(joyH, joyV, btnA, btnB, shield);
    printf("%d\n", s.animationIndex);
}

void Kirby::updateLastValues(double joyH, double joyV, bool btnA, bool btnB, bool shield) {
    l_joyH = joyH;
    l_joyV = joyV;
    l_btnA = btnA;
    l_btnB = btnB;
    l_shield = shield;

    l_walking = walking;
    l_running = running;
    l_jumping = jumping;
    l_crouching = crouching;

    l_mirrored = mirrored;
}
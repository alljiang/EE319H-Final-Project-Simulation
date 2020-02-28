//
// Created by Allen on 2/28/2020.
//

#include <cstdio>
#include "entities.h"
#include "utils.h"
#include "metadata.h"
#include "UART.h"

void Kirby::jump() {
}

void Kirby::run(bool facingRight) {

}

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
    bool disabled;

    //  movement
    if(walking) {
        disabled = false;

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
        disabled = false;

        //  x position
        x += joyH * groundSpeed / dt;

        //  mirrored facing left/right
        if(joyH == 0) mirrored = l_mirrored;
        else mirrored = joyH < 0;

        //  if just started walking or on last frame, reset frame to 0
        framePeriod = (uint8_t) ((3-std::abs(2*joyH) ));
        printf("%d\n", framePeriod);
        if(frameLengthCounter++ > framePeriod) {
            frameLengthCounter = 0;
            frameIndex++;
        }
        if(frameIndex >= 8 || !l_running) frameIndex = 0;
        continuous = false;
        animationIndex = 1;
    }
    else if(jumping) {
        disabled = false;
    }
    else if(falling) {
        disabled = jumpsUsed >= 5;
    }
    else if(crouching) {
        disabled = false;
    }
    //  regular attacks, ground
    else if(jabbingInitial) {
        disabled = true;
    }
    else if(jabbingRepeating) {
        disabled = true;
    }
    //  special attacks, ground
    //  regular attacks, air
    //  special attacks, air
    else {
        //  standing, resting
        disabled = false;

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
    else if(std::abs(joyH) > 0) {
        if(std::abs(joyH) > 0.6) {
            running = true;
            walking = false;
        }
        else if(std::abs(joyH) > 0.1) {
            walking = true;
            running = false;
        }
    }
    else if(joyH == 0 && joyV == 0) {
        running = false;
        walking = false;
        if(l_running || l_walking) {
            lastBlink = currentTime;
        }
    }

    SpriteSendable s;
    s.charIndex = charIndex;
    s.animationIndex = animationIndex;
    s.framePeriod = 1;
    s.frame = frameIndex;
    s.persistent = false;
    s.x = (int16_t) x;
    s.y = (int16_t) y;
//    s.y = (uint8_t) 50;
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

    l_facingRight = facingRight;
    l_walking = walking;
    l_running = running;
    l_jumping = jumping;
    l_crouching = crouching;

    l_mirrored = mirrored;
}
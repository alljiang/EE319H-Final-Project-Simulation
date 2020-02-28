//
// Created by Allen on 2/28/2020.
//

#include <cstdio>
#include "entities.h"

void kirby::jump() {

}

void kirby::run(bool facingRight) {

}

void kirby::controlLoop(double joyH, double joyV, bool btnA, bool btnB, bool shield) {
    //  assume joystick deadzone filtering is already done

    //  first, follow up on any currently performing actions

    //  movement
    if(walking) {

    }
    else if(running) {

    }
    else if(jumping) {

    }
    else if(falling) {

    }
    else if(crouching) {

    }

    updateLastValues(joyH, joyV, btnA, btnB, shield);
}

void kirby::updateLastValues(double joyH, double joyV, bool btnA, bool btnB, bool shield) {
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
}

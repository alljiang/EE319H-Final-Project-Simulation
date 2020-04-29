//
// Created by Allen on 2/29/2020.
//

#include "stage.h"
#include "metadata.h"
#include "entities.h"
#include "UART.h"

//  if damage == 0, corner is left. else, it's right corner
const Hurtbox FD_leftCorner = Hurtbox(true, 38, 103,
                                      SHAPE_CIRCLE, 8,
                                      1, 0,0, 0, 0);
const Hurtbox FD_rightCorner = Hurtbox(
        true, 275, 103, SHAPE_CIRCLE,8,
        1, 1,1, 0, 0);

const Hurtbox BF_leftCorner = Hurtbox(true, 52, 85,
                                      SHAPE_CIRCLE, 8,
                                      1, 0,0, 0, 0);
const Hurtbox BF_rightCorner = Hurtbox(
        true, 268, 85, SHAPE_CIRCLE,8,
        1, 1,1, 0, 0);

const Hurtbox SV_leftCorner = Hurtbox(true, 12, 63,
                                      SHAPE_CIRCLE, 8,
                                      1, 0,0, 0, 0);
const Hurtbox SV_rightCorner = Hurtbox(
        true, 287, 61, SHAPE_CIRCLE,8,
        1, 1,1, 0, 0);

double Stage::ceil(double x, double y) {

    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            if(x <= 38 || x >= 288 || y >= 104) return CEIL_MAX;
            else if(x <= 159) return -0.6198 * x + 93.55;
            else return .6198 * x -103.55;

        case STAGE_TOWER:
            return CEIL_MAX;

        case STAGE_BATTLEFIELD:
            if(x <= 52 || x >= 268 || y >= 86) return CEIL_MAX;
            else if(x <= 161) return -.5505* x + 83.624;
            else return .5607 * x -95.28;

        case STAGE_SMASHVILLE:
            if(x <= 35 || x >= 295 || y >= 65) return CEIL_MAX;
            else if(x <= 47) return -3.5 * x + 187.5;
            else if(x >= 295) return 2 * x -525;
            else return 23;
    }
    return CEIL_MAX;
}

double Stage::floor(double x, double y) {

    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            if(x >= 38 && x <= 288 && y >= 104) return 104;
            else return FLOOR_MAX;

        case STAGE_TOWER:
            return 1;

        case STAGE_BATTLEFIELD:
            if(y >= 151 && x >= 135 && x <= 185) return 151;
            if(y >= 119 && x >= 76 && x <= 128) return 119;
            if(y >= 119 && x >= 193 && x <= 244) return 119;
            if(x >= 52 && x <= 268 && y >= 86) return 86;
            else return FLOOR_MAX;

        case STAGE_SMASHVILLE:
            if(x >= sv_platform_x && x <= sv_platform_x + SMASHVILLE_PLATFORM_LENGTH
                && y >= sv_platform_y + SMASHVILLE_PLATFORM_HEIGHT) return sv_platform_y + SMASHVILLE_PLATFORM_HEIGHT;
            if(x >= 35 && x <= 295 && y >= 65) return 65;
            else return FLOOR_MAX;
    }
    return FLOOR_MAX;
}

double Stage::rightBound(double x, double y) {
    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            if(x < 38 && y < 104 && y > 70) return 38;
            return RIGHT_MAX;

        case STAGE_TOWER:
            return RIGHT_MAX;

        case STAGE_BATTLEFIELD:
            if(x < 52 && y < 86 && y > 55) return 52;
            return RIGHT_MAX;

        case STAGE_SMASHVILLE:
            return RIGHT_MAX;
    }
    return RIGHT_MAX;
}

double Stage::leftBound(double x, double y) {
    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            if(x > 280 && y < 104 && y > 70) return 280;
            return LEFT_MAX;

        case STAGE_TOWER:
            return LEFT_MAX;

        case STAGE_BATTLEFIELD:
            if(x > 268 && y < 86 && y > 55) return 268;
            return LEFT_MAX;

        case STAGE_SMASHVILLE:
            return LEFT_MAX;
    }
    return LEFT_MAX;
}

double Stage::getStartX(uint8_t player) {
    switch(stageIndex) {
        case STAGE_FINALDESTINATION:
            if(player == 1) return 60;
            else return 232;
        case STAGE_TOWER:
            if(player == 1) return 60;
            else return 232;

        case STAGE_BATTLEFIELD:
            if(player == 1) return 60;
            else return 232;

        case STAGE_SMASHVILLE:
            if(player == 1) return 60;
            else return 232;
    }
    return 0;
}

double Stage::getStartY(uint8_t player) {
    switch(stageIndex) {
        case STAGE_FINALDESTINATION:
            return 104;

        case STAGE_TOWER:
            return 0;

        case STAGE_BATTLEFIELD:
            return 86;

        case STAGE_SMASHVILLE:
            return 65;
    }
    return 0;
}

bool Stage::onPlatform(double x, double y) {
    switch(stageIndex) {
        case STAGE_FINALDESTINATION:
            return false;

        case STAGE_TOWER:
            return false;

        case STAGE_BATTLEFIELD:
            if(y == 119 && x >= 76 && x <= 128) return true;
            if(y == 151 && x >= 135 && x <= 185) return true;
            if(y == 119 && x >= 193 && x <= 244) return true;

        case STAGE_SMASHVILLE:
            if(x >= sv_platform_x && x <= sv_platform_x + SMASHVILLE_PLATFORM_LENGTH
               && y == sv_platform_y + SMASHVILLE_PLATFORM_HEIGHT) return true;
            return false;
    }
    return false;
}

double Stage::xVelocity(double x, double y) {
    switch(stageIndex) {
        case STAGE_FINALDESTINATION:
            return 0;

        case STAGE_TOWER:
            return 0;

        case STAGE_BATTLEFIELD:
            return 0;

        case STAGE_SMASHVILLE:
            if(onPlatform(x, y)) return sv_platform_xvel;
            return 0;
    }
    return 0;
}

void Stage::update() {
    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            break;

        case STAGE_TOWER:
            break;

        case STAGE_BATTLEFIELD:
            break;

        case STAGE_SMASHVILLE:
            SpriteSendable s;
            s.charIndex = 3;
            s.animationIndex = 20;
            s.framePeriod = 1;
            s.frame = 0;
            s.persistent = false;
            s.continuous = false;
            s.x = (int16_t) sv_platform_x;
            s.y = (int16_t) sv_platform_y;
            s.layer = LAYER_NAMETAG;
            s.mirrored = false;
            UART_sendAnimation(s);

            if(sv_platform_goingRight) {
                if(sv_platform_x > SMASHVILLE_PLATFORM_DECELPOINTRIGHT) sv_platform_xvel -= SMASHVILLE_PLATFORM_ACCELERATON;
                else sv_platform_xvel += SMASHVILLE_PLATFORM_ACCELERATON;
                if(sv_platform_xvel > SMASHVILLE_PLATFORM_MAXSPEED) sv_platform_xvel = SMASHVILLE_PLATFORM_MAXSPEED;
                else if(sv_platform_xvel < 0) {
                    sv_platform_goingRight = false;
                    sv_platform_xvel = 0;
                }
                sv_platform_x += sv_platform_xvel;

            } else {
                if(sv_platform_x < SMASHVILLE_PLATFORM_DECELPOINTLEFT) sv_platform_xvel += SMASHVILLE_PLATFORM_ACCELERATON;
                else sv_platform_xvel -= SMASHVILLE_PLATFORM_ACCELERATON;
                if(sv_platform_xvel < -SMASHVILLE_PLATFORM_MAXSPEED) sv_platform_xvel = -SMASHVILLE_PLATFORM_MAXSPEED;
                else if(sv_platform_xvel > 0) {
                    sv_platform_goingRight = true;
                    sv_platform_xvel = 0;
                }
                sv_platform_x += sv_platform_xvel;
            }
            break;
    }
}

void Stage::initialize(uint8_t index, HitboxManager *hitboxManager) {
    stageIndex = index;

    if(stageIndex == STAGE_FINALDESTINATION) {
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            FD_leftCorner, 0, true);
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            FD_rightCorner, 0, true);
    }
    else if(stageIndex == STAGE_TOWER) {}
    else if(stageIndex == STAGE_BATTLEFIELD) {
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            BF_leftCorner, 0, true);
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            BF_rightCorner, 0, true);
    }
    else if(stageIndex == STAGE_SMASHVILLE) {
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            SV_leftCorner, 0, true);
        hitboxManager->addHurtboxFullConfig(0, 0, false,
                                            SV_rightCorner, 0, true);
        sv_platform_goingRight = true;
        sv_platform_x = 20;
        sv_platform_y = 115;
        sv_platform_xvel = 0;
    }
}
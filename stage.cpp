//
// Created by Allen on 2/29/2020.
//

#include "stage.h"
#include "animator.h"

double Stage::ceil(double x, double y) {

    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            if(x <= 38 || x >= 288 || y >= 104) return CEIL_MAX;
            else if(x <= 159) return -0.6198 * x + 93.55;
            else return .6198 * x -103.55;
            break;

        case STAGE_TOWER:
            return CEIL_MAX;
            break;

        case STAGE_BATTLEFIELD:
            break;
    }
    return CEIL_MAX;
}

double Stage::floor(double x, double y) {

    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            if(x >= 38 && x <= 288 && y >= 104) return 104;
            else return FLOOR_MAX;
            break;

        case STAGE_TOWER:
            return FLOOR_MAX;
            break;

        case STAGE_BATTLEFIELD:
            break;
    }
    return FLOOR_MAX;
}

double Stage::rightBound(double x, double y) {
    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            if(x < 38 && y < 104 && y > 70) return 38;
            return RIGHT_MAX;
            break;

        case STAGE_TOWER:
            return RIGHT_MAX;
            break;

        case STAGE_BATTLEFIELD:
            return RIGHT_MAX;
            break;
    }
    return RIGHT_MAX;
}

double Stage::leftBound(double x, double y) {
    switch(stageIndex) {

        case STAGE_FINALDESTINATION:
            if(x > 280 && y < 104 && y > 70) return 280;
            return LEFT_MAX;
            break;

        case STAGE_TOWER:
            return LEFT_MAX;
            break;

        case STAGE_BATTLEFIELD:
            return LEFT_MAX;
            break;
    }
    return LEFT_MAX;
}

double Stage::getStartX(uint8_t player) {
    switch(stageIndex) {
        case STAGE_FINALDESTINATION:
            if(player == 1) return 60;
            else return 250;
            break;

        case STAGE_TOWER:
            return 0;
            break;

        case STAGE_BATTLEFIELD:
            return 0;
            break;
    }
    return 0;
}

double Stage::getStartY(uint8_t player) {
    switch(stageIndex) {
        case STAGE_FINALDESTINATION:
            if(player == 1) return 104;
            else return 104;
            break;

        case STAGE_TOWER:
            return 0;
            break;

        case STAGE_BATTLEFIELD:
            return 0;
            break;
    }
    return 0;
}

double Stage::xVelocity(double x, double y) {
    switch(stageIndex) {
        case STAGE_FINALDESTINATION:
            return 0;
            break;

        case STAGE_TOWER:
            return 0;
            break;

        case STAGE_BATTLEFIELD:
            return 0;
            break;
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
    }
}

void Stage::initialize(uint8_t index) {
    stageIndex = index;

    animator_readPersistentSprite(stageNames[index], 0, 0);
}
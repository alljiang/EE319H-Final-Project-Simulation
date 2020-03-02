//
// Created by Allen on 3/2/2020.
//

#ifndef EE319H_FINAL_PROJECT_SIMULATION_COLLIDER_H
#define EE319H_FINAL_PROJECT_SIMULATION_COLLIDER_H

class hitboxManager {

};

class collider {

#define SHAPE_CIRCLE 0
#define SHAPE_RECTANGLE 1

    double x, y;
    uint8_t shape;

    collider(double x, double y, uint8_t shape) {
        this->shape = shape;
    }

};

class hurtBox:public collider {

    double damage;
    int8_t source;     //  player who created this hitbox, will not damage this player

    hitbox(double cX, double cY, uint8_t boxShape, double damage, int8_t source) {
         x = cX;
         y = cY;
         shape = boxShape;
         this->damage = damage;
         this->source = source;
    }

};

class hitBox:public collider {

    Player *p;

};


#endif //EE319H_FINAL_PROJECT_SIMULATION_COLLIDER_H

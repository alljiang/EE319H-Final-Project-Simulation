//
// Created by Allen on 2/28/2020.
//

#include <cstdint>
#include <list>

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H


class Player;

class Collider {

#define SHAPE_CIRCLE 0
#define SHAPE_RECTANGLE 1

public:
    double x, y;
    uint8_t shape;
    double height=0, width=0, radius=0;

    Collider(double x, double y, uint8_t shape, double radius) {
        this->x = x;
        this->y = y;
        this->shape = shape;
        this->radius = radius;
    }

    Collider(double x, double y, uint8_t shape, double length, double width) {
        this->x = x;
        this->y = y;
        this->shape = shape;
        this->height = length;
        this->width = width;
    }

};

class Hurtbox: public Collider {

public:
    double damage, knockback;
    int8_t source;     //  player who created this hitbox, will not damage this player
    int8_t frames, frameLength, currentFrame, frameLengthCounter;
    bool active{false};

    double xVel, yVel;

    Hurtbox() : Collider(0,0,0,0) {}

    Hurtbox(bool circle, double cX, double cY,
            uint8_t boxShape, double radius,
            int8_t frames=1, int8_t frameLength=1,
            double damage=0, double knockback=0,
            double xVelocity=0, double yVelocity=0)
            : Collider(cX, cY, boxShape, radius) {
        shape = boxShape;
        this->damage = damage;
        this->knockback = knockback;
        this->source = source;
        this->frames = frames;
        this->frameLength = frameLength;
        this->xVel = xVelocity;
        this->yVel = yVelocity;
    }

    Hurtbox(double cX, double cY, uint8_t boxShape, double length, double width,
            int8_t frames=1, int8_t frameLength=1,
            double damage=0, double knockback=0,
            double xVelocity=0, double yVelocity=0)
            : Collider(cX, cY, boxShape, length, width) {
        shape = boxShape;
        this->damage = damage;
        this->knockback = knockback;
        this->source = source;
        this->frames = frames;
        this->frameLength = frameLength;
        this->xVel = xVelocity;
        this->yVel = yVelocity;
    }

    void setSource(uint8_t playerSource) { this->source = playerSource; }
};

class Hitbox: public Collider {

public:
    Hitbox(double cX, double cY, uint8_t boxShape, double radius)
        : Collider(cX, cY, boxShape, radius) {}
    Hitbox(double cX, double cY, uint8_t boxShape, double length, double width)
        : Collider(cX, cY, boxShape, length, width) {}

    void offsetY(double yOffset) { y += yOffset; }
    void offsetX(double xOffset) { x += xOffset; }
    void offsetX(double xOffset, bool mirrored) { if(mirrored) offsetX(-xOffset); else offsetX(xOffset); }

    void offsetRadius(double radius) { this->radius += radius; };

    void initialize(double cX, double cY, uint8_t boxShape, double radius) {
        x = cX;
        y = cY;
        shape = boxShape;
        this->radius = radius;
    }

    bool isColliding(class Hurtbox hurtbox);
};

class HitboxManager {

#define hurtboxSlots 10

protected:
    Player *p1, *p2;
    Hurtbox hurtboxes[hurtboxSlots];

public:

    void initialize(class Player* player1, class Player* player2 = nullptr) {
        p1 = player1;
        if(player2 != nullptr) p2 = player2;
        else p2 = nullptr;
    }

    void checkCollisions();
    void addHurtbox(double xOffset, double yOffset, bool mirrored,
            class Hurtbox hurtBox, uint8_t playerSource);
    void displayHitboxesOverlay();
    void clearHitboxOverlay();
};

class Entity {

protected:
    double x; //  [0, 320]
    double y;  //  [0, 240]
    double yVel;    //  pps
    double xVel;

    long long l_time;   //  last loop time millis

public:
    void setX(double newX) { x = newX; }
    void setY(double newY) { y = newY; }

};

class Player: public Entity {

protected:
    const double joystickJumpSpeed = 0.4;   //  joystick must change by this much to activate a jump

    uint8_t player;             //  1 or 2
    uint16_t damage;            //  percentage between 0% and 999%
    int16_t action, l_action;

    uint8_t animationIndex;     //  index of animation
    uint8_t frameIndex{0};      //  current frame index of current animation
    uint8_t framePeriod{1}, frameLengthCounter{0};  //  counts current frame height
    bool continuous{false};
    bool mirrored, l_mirrored;

    long long disabledFrames {0};   //  frames before making a new move
    bool noJumpsDisabled;           //  disabled until landing because of running out of jumps

    double l_joyH;              //  last joystick horizontal value
    double l_joyV;              //  last joystick vertical value
    bool l_btnA;                //  last button A value
    long long l_btnARise_t;     //  last button A pressed time in millis
    long long l_btnAFall_t;     //  last button A release time in millis
    bool l_btnB;                //  last button B value
    long long l_btnBRise_t;     //  last button B pressed time in millis
    long long l_btnBFall_t;     //  last button B release time in millis
    bool l_shield;              //  last shield value
    long long l_shieldRise_t;   //  last shield pressed time in millis
    long long l_shieldFall_t;   //  last shield release time in millis

    //  jumping
    uint8_t jumpsUsed;  // midair only

public:
    Player() {}

    Hitbox hitbox = Hitbox(0, 0, 0, 0);

    void setPlayer(uint8_t p) { player = p; }

    virtual void controlLoop(double joyH, double joyV, bool btnA, bool btnB, bool shield, class Stage* stage,
            class HitboxManager* hitboxManager) = 0; //  called every update
    virtual void updateLastValues(double joyH, double joyV, bool btnA, bool btnB, bool shield) = 0;
    virtual void collide(class Hurtbox hurtbox) = 0;
};


class Kirby: public Player {

#define ACTION_RESTING 0
#define ACTION_RUNNING 1
#define ACTION_FALLING 2
#define ACTION_JUMPING 3
#define ACTION_MULTIJUMPING 4
#define ACTION_CROUCHING 5
#define ACTION_JABSINGLE 6
#define ACTION_JABDOUBLE 7
#define ACTION_JABREPEATING 8

#define STAGE_X_OFFSET 18

protected:
    //  animation config
    const uint8_t charIndex = 0;
    const uint16_t blinkPeriod = 2000;      //  how often kirby blinks when standing still

    //  physics config
    const double groundSpeed = 30;  // pps
    const double airSpeed = 1;

    const double initialJumpSpeed = 1.8;
    const double repeatedJumpSpeed = 1.55;
    const double gravityRising = 0.07;
    const double gravityFalling = 0.1;
    const double maxFallingVelocity = -2.3;

    const double airResistance = 0.02;
    const double maxHorizontalSpeed = 20;

    //  standing, resting
    long long lastBlink{0};

    //  jabbing
    long long l_singleJabTime, l_doubleJabTime, l_repeatJabTime;

public:
    Hurtbox jabSingle = Hurtbox(true,14, 11, SHAPE_CIRCLE,
            8, 3, 3);
    Hurtbox jabDouble = Hurtbox(true,15, 12, SHAPE_CIRCLE,
            9, 3, 4);
    Hurtbox jabRepeating = Hurtbox(true,28, 14, SHAPE_CIRCLE,
            15, 1, 3);


    Kirby() {}

    //  general control loop
    void controlLoop(double joyH, double joyV, bool btnA, bool btnB, bool shield, class Stage* stage,
                     class HitboxManager* hitboxManager) override; //  called every update

    void updateLastValues(double joyH, double joyV, bool btnA, bool btnB, bool shield) override ;

    void collide(class Hurtbox hurtbox);
};

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H

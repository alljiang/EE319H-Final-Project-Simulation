//
// Created by Allen on 2/28/2020.
//

#include <cstdint>

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H


class Entity {

protected:
    double x; //  [0, 320]
    double y;  //  [0, 240]
    double yVel;    //  pps

    long long l_time;   //  last loop time millis
};

class Player: public Entity {

protected:
    const double joystickJumpSpeed = 0.4;   //  joystick must change by this much to activate a jump

    uint8_t player;             //  1 or 2
    uint16_t damage;            //  percentage between 0% and 999%

    uint8_t animationIndex;     //  index of animation
    uint8_t frameIndex{0};      //  current frame index of current animation
    uint8_t framePeriod{1};
    uint8_t frameLengthCounter{0};  //  counts current frame length
    bool continuous{false};
    bool mirrored;
    bool l_mirrored;

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

    //  movement

    //  running
    bool l_walking;
    bool walking;
    bool l_running;
    bool running;

    //  jumping
    bool l_jumping;
    bool jumping;
    bool multijumping;
    bool falling;
    uint8_t jumpsUsed;  // midair only

    //  crouching
    bool l_crouching;
    bool crouching;

public:
    void setPlayer(uint8_t p) { player = p; }

    //  general control loop
    virtual void controlLoop(double joyH, double joyV, bool btnA, bool btnB, bool shield) = 0; //  called every update
    virtual void updateLastValues(double joyH, double joyV, bool btnA, bool btnB, bool shield) = 0;

};


class Kirby: public Player {

protected:
    //  animation config
    const uint8_t charIndex = 0;
    const uint16_t blinkPeriod = 2000;      //  how often kirby blinks when standing still

    //  physics config
    const double groundSpeed = 30;  // pps
    const double airSpeed = 1;
    const double initialJumpSpeed = 3;
    const double repeatedJumpSpeed = 2.5;
    const double gravityRising = 0.15;
    const double gravityFalling = 0.2;

    //  standing, resting
    long long lastBlink{0};

    //  regular attacks, ground
    bool jabbingInitial;
    bool jabbingRepeating;

    //  special attacks, ground

    //  regular attacks, air

    //  special attacks, air

public:
    //  general control loop
    void controlLoop(double joyH, double joyV, bool btnA, bool btnB, bool shield) override; //  called every update
    void updateLastValues(double joyH, double joyV, bool btnA, bool btnB, bool shield) override ;
};

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H

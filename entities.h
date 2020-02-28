//
// Created by Allen on 2/28/2020.
//

#include <cstdint>

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H


class entity {

protected:
    uint16_t x; //  [0, 320]
    uint8_t y;  //  [0, 240]

};

class player: public entity {

protected:
    uint8_t player;     //  1 or 2

    uint16_t damage;    //  percentage between 0% and 999%

    double l_joyH;      //  last joystick horizontal value
    double l_joyV;      //  last joystick vertical value
    bool l_btnA;        //  last button A value
    bool l_btnB;        //  last button B value
    bool l_shield;      //  last shield value

    bool l_facingRight;
    bool facingRight;   //  true if facing right side, false if facing left side

    //  running
    bool l_walking;
    bool walking;
    bool l_running;
    bool running;

    //  jumping
    bool l_jumping;
    bool jumping;
    bool falling;
    uint8_t jumpsUsed;

    //  crouching
    bool l_crouching;
    bool crouching;

public:
    //  general control loop
    virtual void controlLoop(double joyH, double joyV, bool btnA, bool btnB, bool shield) = 0; //  called every update
    virtual void updateLastValues(double joyH, double joyV, bool btnA, bool btnB, bool shield) = 0;

    //  running
    virtual void run(bool facingRight) = 0;

    //  jumping
    virtual void jump() = 0;

};


class kirby: public player {

protected:
    //  physics config
    double walkSpeed = 10;  // pixels per second
    double runSpeed = 20;   // pixels per second

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

    //  running
    void run(bool facingRight) override;

    //  jumping
    void jump() override;
};

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_ENTITIES_H

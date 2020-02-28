//
// Created by Allen on 2/5/2020.
//

#include <cstdint>
#include "stdio.h"
#include "SDL.h"
#include "controller.h"

double joystick1_h = 0;  // A, D, Left, Right
double joystick1_v = 0;  // W, S, Up, Down
bool btn1_a;    // V
bool btn1_b;    // B
bool btn1_start;    // Space
bool btn1_l;    // LShift
bool btn1_r;    // RShift

double joystick2_h = 0;  //  J, L
double joystick2_v = 0;  // I, K
bool btn2_a;    //  .
bool btn2_b;    // /
bool btn2_start;    // ,
bool btn2_l;    // n
bool btn2_r;    // ;

void controller_updateController() {
    joystick1_h = 0;
    joystick1_v = 0;
    btn1_a = false;
    btn1_b = false;
    btn1_start = false;
    btn1_l = false;
    btn1_r = false;

    joystick2_h = 0;
    joystick2_v = 0;
    btn2_a = false;
    btn2_b = false;
    btn2_start = false;
    btn2_l = false;
    btn2_r = false;

    const uint8_t *keys = SDL_GetKeyboardState(NULL);

    if(keys[SDL_SCANCODE_LEFT])
        joystick1_h += -0.3;
    if(keys[SDL_SCANCODE_RIGHT])
        joystick1_h += 0.3;
    if(keys[SDL_SCANCODE_UP])
        joystick1_v += 0.3;
    if(keys[SDL_SCANCODE_DOWN])
        joystick1_v -= 0.3;
    if(keys[SDL_SCANCODE_W])
        joystick1_v += 1.0;
    if(keys[SDL_SCANCODE_S])
        joystick1_v += -1.0;
    if(keys[SDL_SCANCODE_A])
        joystick1_h += -1.0;
    if(keys[SDL_SCANCODE_D])
        joystick1_h += 1.0;
    if(keys[SDL_SCANCODE_V])
        btn1_a = true;
    if(keys[SDL_SCANCODE_B])
        btn1_b = true;
    if(keys[SDL_SCANCODE_LSHIFT])
        btn1_l = true;
    if(keys[SDL_SCANCODE_RSHIFT])
        btn1_r = true;
    if(keys[SDL_SCANCODE_SPACE])
        btn1_start = true;

    if(keys[SDL_SCANCODE_I])
        joystick2_v += 1.0;
    if(keys[SDL_SCANCODE_K])
        joystick2_v += -1.0;
    if(keys[SDL_SCANCODE_J])
        joystick2_h += -1.0;
    if(keys[SDL_SCANCODE_L])
        joystick2_h += 1.0;
    if(keys[SDL_SCANCODE_PERIOD])
        btn2_a = true;
    if(keys[SDL_SCANCODE_SLASH])
        btn2_b = true;
    if(keys[SDL_SCANCODE_N])
        btn2_l = true;
    if(keys[SDL_SCANCODE_SEMICOLON])
        btn2_r = true;
    if(keys[SDL_SCANCODE_M])
        btn2_start = true;
}

double getJoystick_h(uint8_t player) {
    if(player == 2) return joystick2_h;
    else return joystick1_h;
}

double getJoystick_v(uint8_t player) {
    if(player == 2) return joystick2_v;
    else return joystick1_v;
}

bool getBtn_a(uint8_t player) {
    if(player == 2) return btn2_a;
    else return btn1_a;
}

bool getBtn_b(uint8_t player) {
    if(player == 2) return btn2_b;
    else return btn1_b;
}

bool getBtn_l(uint8_t player) {
    if(player == 2) return btn2_l;
    else return btn1_l;
}

bool getBtn_r(uint8_t player) {
    if(player == 2) return btn2_r;
    else return btn1_r;
}

bool getBtn_start(uint8_t player) {
    if(player == 2) return btn2_start;
    else return btn1_start;
}
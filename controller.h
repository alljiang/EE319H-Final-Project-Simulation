//
// Created by Allen on 2/5/2020.
//

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_CONTROLLER_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_CONTROLLER_H

extern void controller_updateController();
extern float getJoystick_h(uint8_t player);
extern float getJoystick_v(uint8_t player);
extern bool getBtn_a(uint8_t player);
extern bool getBtn_b(uint8_t player);
extern bool getBtn_l(uint8_t player);
extern bool getBtn_r(uint8_t player);
extern bool getBtn_start(uint8_t player);

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_CONTROLLER_H

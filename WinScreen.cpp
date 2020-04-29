//
// Created by Allen on 4/29/2020.
//

#include "WinScreen.h"
#include "metadata.h"
#include "colors_winp1gaw.h"
#include "colors_winp2gaw.h"
#include "colors_winp1kirby.h"
#include "colors_winp2kirby.h"
#include "colors_winp1val.h"
#include "colors_winp2val.h"
#include "animator.h"

void WinScreen::start(int8_t winner, int8_t character) {
    reset();

    if(winner == 1) {
        if(character == CHARACTER_KIRBY) {
            animator_setBackgroundColors(colors_winp1kirby);
            animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P1_KIRBY], 0, 0);
        }
        else if(character == CHARACTER_GAMEANDWATCH) {
            animator_setBackgroundColors(colors_winp1gaw);
            animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P1_GAMEANDWATCH], 0, 0);
        }
        else {
            animator_setBackgroundColors(colors_winp1val);
            animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P1_VALVANO], 0, 0);
        }
    }
    else {
        if(character == CHARACTER_KIRBY) {
            animator_setBackgroundColors(colors_winp2kirby);
            animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P2_KIRBY], 0, 0);
        }
        else if(character == CHARACTER_GAMEANDWATCH) {
            animator_setBackgroundColors(colors_winp2gaw);
            animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P2_GAMEANDWATCH], 0, 0);
        }
        else {
            animator_setBackgroundColors(colors_winp2val);
            animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P2_VALVANO], 0, 0);
        }
    }
}

void WinScreen::loop(void (*transitionCall)(void)) {
    if(++loopCount >= 20 * WINSCREEN_TIME_SECONDS) {
        (*transitionCall)();
    }
}

void WinScreen::reset() {
    loopCount = 0;
}

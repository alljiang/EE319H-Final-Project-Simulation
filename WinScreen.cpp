//
// Created by Allen on 4/29/2020.
//

#include "WinScreen.h"
#include "metadata.h"
#include "colors_w1gw.h"
#include "colors_w1gwc.h"
#include "colors_w1kb.h"
#include "colors_w1kbc.h"
#include "colors_w1va.h"
#include "colors_w1vac.h"
#include "colors_w2gw.h"
#include "colors_w2gwc.h"
#include "colors_w2kb.h"
#include "colors_w2kbc.h"
#include "colors_w2va.h"
#include "colors_w2vac.h"
#include "animator.h"

void WinScreen::start(int8_t winner, int8_t character, bool english) {
    reset();

    if(winner == 1) {
        if(character == CHARACTER_KIRBY) {
            if(english) {
                animator_setBackgroundColors(colors_w1kb);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P1_KIRBY], 0, 0);
            }
            else {
                animator_setBackgroundColors(colors_w1kbc);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P1_KIRBY_CHINESE], 0, 0);
            }
        }
        else if(character == CHARACTER_GAMEANDWATCH) {
            if(english) {
                animator_setBackgroundColors(colors_w1gw);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P1_GAMEANDWATCH], 0, 0);
            }
            else {
                animator_setBackgroundColors(colors_w1gwc);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P1_GAMEANDWATCH_CHINESE], 0, 0);
            }
        }
        else {
            if(english) {
                animator_setBackgroundColors(colors_w1va);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P1_VALVANO], 0, 0);
            }
            else {
                animator_setBackgroundColors(colors_w1vac);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P1_VALVANO_CHINESE], 0, 0);
            }
        }
    }
    else {
        if(character == CHARACTER_KIRBY) {
            if(english) {
                animator_setBackgroundColors(colors_w2kb);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P2_KIRBY], 0, 0);
            }
            else {
                animator_setBackgroundColors(colors_w2kbc);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P2_KIRBY_CHINESE], 0, 0);
            }
        }
        else if(character == CHARACTER_GAMEANDWATCH) {
            if(english) {
                animator_setBackgroundColors(colors_w2gw);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P2_GAMEANDWATCH], 0, 0);
            }
            else {
                animator_setBackgroundColors(colors_w2gwc);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P2_GAMEANDWATCH_CHINESE], 0, 0);
            }
        }
        else {
            if(english) {
                animator_setBackgroundColors(colors_w2va);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P2_VALVANO], 0, 0);
            }
            else {
                animator_setBackgroundColors(colors_w2vac);
                animator_readPersistentSprite(persistentSprites[BACKGROUND_WIN_P2_VALVANO_CHINESE], 0, 0);
            }
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

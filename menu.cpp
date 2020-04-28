//
// Created by Allen on 4/28/2020.
//

#include "menu.h"
#include "UART.h"
#include "metadata.h"
#include "colors_charmenu.h"

void Menu::start() {
    reset();
    animator_setBackgroundColors(colors_charmenu);
    animator_readPersistentSprite(persistentSprites[BACKGROUND_MENU], 0, 0);
    animator_readCharacterSDCard(4);
}

void Menu::loop(double joyH1, double joyV1, double joyH2, double joyV2,
                bool btnA1, bool btnA2, bool btnB1, bool btnB2) {
    double dt = 49;
    currentTime += (uint8_t)dt;

    SpriteSendable s;
    s.charIndex = charIndex;
    s.framePeriod = 1;
    s.persistent = false;
    s.continuous = false;
    s.mirrored = false;

    //  P1 btn A press
    if(!l_btnA1 && btnA1) {
        if(!p1Selected && getCharacter(p1CursorX, p1CursorY) != -1) {
            p1Selected = true;
        }

    }
    //  P1 btn B press
    else if(!l_btnB1 && btnB1) {
        if(p1Selected) p1Selected = false;
    }

    //  P2 btn A press
    if(!l_btnA2 && btnA2) {

    }

    if(!p1Selected) {
        p1CursorX += MENU_CURSORSPEED * joyH1;
        p1CursorY += MENU_CURSORSPEED * joyV1;

        int p1CursorSmall_xAnimationOffset = 0;
        int p1CursorSmall_yAnimationOffset = 0;

        s.animationIndex = 0;
        s.frame = 0;
        s.x = (int16_t) p1CursorX + p1CursorSmall_xAnimationOffset;
        s.y = (int16_t) p1CursorY + p1CursorSmall_yAnimationOffset;
        s.layer = LAYER_CHARACTER;

        UART_sendAnimation(s);
    }
    else {
        int p1CursorBig_xAnimationOffset = -3;
        int p1CursorBig_yAnimationOffset = -3;

        s.animationIndex = 1;
        s.frame = 0;
        s.x = (int16_t) p1CursorX + p1CursorBig_xAnimationOffset;
        s.y = (int16_t) p1CursorY + p1CursorBig_yAnimationOffset;
        s.layer = LAYER_CHARACTER_PROJECTILE;

        UART_sendAnimation(s);

        int xCharPreviewOffset, yCharPreviewOffset;
        int8_t selectedCharacter = getCharacter(p1CursorX, p1CursorY);

        //  kirby
        if(selectedCharacter == 0) {
            s.animationIndex = 10;
            xCharPreviewOffset = 5;
            yCharPreviewOffset = 0;
        }
        //  valvano
        else if(selectedCharacter == 2) {
            s.animationIndex = 11;
            xCharPreviewOffset = 0;
            yCharPreviewOffset = -5;
        }
        //  game and watch
        else if(selectedCharacter == 1) {
            s.animationIndex = 12;
            xCharPreviewOffset = -15;
            yCharPreviewOffset = -1;
        }

        s.x = 10 + xCharPreviewOffset;
        s.y = 5 + yCharPreviewOffset;
        s.layer = LAYER_CHARACTER;

        UART_sendAnimation(s);
    }

    if(!p2Selected) {
        p2CursorX += MENU_CURSORSPEED * joyH2;
        p2CursorY += MENU_CURSORSPEED * joyV2;

    }
    else {

    }

    UART_commandUpdate();
}

void Menu::updateLastValues(double joyH1, double joyV1, double joyH2, double joyV2,
                            bool btnA1, bool btnA2, bool btnB1, bool btnB2) {
    l_joyH1 = joyH1;
    l_joyV1 = joyV1;
    l_btnA1 = btnA1;
    l_btnB1 = btnB1;
    l_joyH2 = joyH2;
    l_joyV2 = joyV2;
    l_btnA2 = btnA2;
    l_btnB2 = btnB2;
}

void Menu::reset() {
    p1Selected = p2Selected = false;
    currentTime = 0;
    l_joyH1 = 0;
    l_joyV1 = 0;
    l_btnA1 = false;
    l_btnB1 = false;
    l_joyH2 = 0;
    l_joyV2 = 0;
    l_btnA2 = false;
    l_btnB2 = false;
    p1CursorX = 70;
    p2CursorX = 250;
    p1CursorY = p2CursorY = 50;
}

int8_t Menu::getCharacter(double x, double y) {
//    printf("%0.0f\t%0.0f\n", x, y);

    if(y > 197 || y < 80 || x < 25 || x > 270) return -1;

    if(x < 107) return 0;   //  kirby
    else if(x < 194) return 2;  //  valvano
    else return 1;  //  game and watch
}
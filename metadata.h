//
// Created by Allen on 2/5/2020.
//

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_METADATA_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_METADATA_H

/*
 *  Note: if the sprite is persistent, then the layer will be ignored when painting the background, so send them in the right order!
 */

const uint8_t LAYER_NULL = 0;
const uint8_t LAYER_BOTTOM = 1;
const uint8_t LAYER_BACKGROUND = 10;
const uint8_t LAYER_BACKGROUND_ANIMATION = 20;
const uint8_t LAYER_CHARACTER = 30;
const uint8_t LAYER_CHARACTER_ATTACK = 40;
const uint8_t LAYER_NAMETAG = 50;
const uint8_t LAYER_CHARACTER_PROJECTILE = 60;
const uint8_t LAYER_OVERLAY = 100;
const uint8_t LAYER_PERCENTAGE = 110;
const uint8_t LAYER_TOP = 255;

const uint32_t colors[12] = {4294967295,9987429,14326448,16234959,16633064,4215765,15132390,3552822,13123886,16340024,9643805,16776960};

const char characterNames[4][15] = {
                                        {"kirby"},
                                        {"marth"},
                                        {"jigglypuff"},
                                        {"gameandwatch"},
                                       };

const char animations[4][64][10] = {
                                        {
                                            {"crouch"},
                                            {"dash"},
                                            {"fall"},
                                            {"fallnojmp"},
                                            {"firstjmp"},
                                            {"multijmp"},
                                            {"rest"},
                                            {"slow"},
                                            {"slowstop"},
                                            {"walk"}
                                        },
                                        {

                                        },
                                        {

                                        },
                                        {

                                        },
                                      };
const uint16_t numberOfAnimations = 100;

struct Animation {
    public:
        uint32_t memLocation;
        uint8_t width;
        uint8_t height;
        uint8_t frames;
        uint8_t characterIndex;
        uint8_t animationIndex;
};

struct SpriteSendable {
    public:
        int16_t x;
        int16_t y;
        uint8_t charIndex;
        uint8_t animationIndex;
        uint8_t frame;      //  current frame of animation
        bool persistent;        //  if the animation is persistent,
        uint8_t layer;       //  priority for layering over other sprites. high layer value = top layer
        bool continuous;    //  continue to next frame of animation
        uint8_t framePeriod;    //  how many updates each frame lasts
        uint8_t currentframeLength;
        bool mirrored;
};

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_METADATA_H

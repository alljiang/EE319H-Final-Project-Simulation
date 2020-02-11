//
// Created by Allen on 2/5/2020.
//

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_METADATA_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_METADATA_H

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
        uint16_t memLocation;
        uint8_t width;
        uint8_t height;
        uint8_t frames;
        uint8_t characterIndex;
        uint8_t animationIndex;
};

struct SpriteSendable {
    public:
        uint16_t x;
        uint8_t y;
        uint8_t charIndex;
        uint8_t animationIndex;
        uint8_t frame;      //  current frame of animation
        bool persistent;        //  if the animation is persistent,
};

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_METADATA_H

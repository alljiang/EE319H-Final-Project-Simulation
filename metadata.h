//
// Created by Allen on 2/5/2020.
//

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_METADATA_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_METADATA_H

const int32_t colors[20] = {
        -1,9987429,14326448,16234959,16633064,4215765,15132390,3552822,13123886,16340024,9643805,16776960
};

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

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_METADATA_H

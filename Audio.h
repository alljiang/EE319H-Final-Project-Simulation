
#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>

const char soundNames[100][10] = {
        {"menu"},
        {"321go"},
        {"smash"},
        {"sine"},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {},
        {}
};

struct AudioParams {
    uint16_t soundIndex;
    uint32_t startIndex;
    int32_t endIndex;   // in frames. if endIndex == -1, play entire song
    uint32_t frames;
    int32_t FIFO_size;   // -1 means not set
    float volume;
    bool loop;
    FILE* file;
};

extern void ReadSDFIFO();
extern void Audio_init();
extern void Audio_initSD();
extern void Audio_closeSD();
extern int8_t Audio_playAudio(struct AudioParams sendable);
extern int8_t Audio_play(uint16_t soundIndex, float volume, uint32_t startIndex=0, int32_t endIndex=-1, bool loop=false);
extern void Audio_destroyAudio(int8_t slotID);
extern void Audio_destroyAllAudio();
extern void Audio_DAC_write(uint16_t mapping);
extern void Audio_initParams(struct AudioParams* params);

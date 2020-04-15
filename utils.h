//
// Created by Allen on 2/6/2020.
//

#include <chrono>
#include <cstdlib>
#include <cstdio>

#ifndef EE319K_FINAL_PROJECT_INITIAL_TESTING_UTILS_H
#define EE319K_FINAL_PROJECT_INITIAL_TESTING_UTILS_H

#define     println(s)          printf("%s\n", s)
#define     printchar(c)        printf("%c\n", c)
#define     printint(i)         printf("%d\n", i)
#define     printfloat(f)       printf("%0.3f\n", f)
#define     millis()            (unsigned long) std::chrono::duration_cast<std::chrono::milliseconds> \
                                (std::chrono::system_clock::now().time_since_epoch()).count()

extern void sleep(uint32_t ms);

extern double absVal(double d);

#endif //EE319K_FINAL_PROJECT_INITIAL_TESTING_UTILS_H

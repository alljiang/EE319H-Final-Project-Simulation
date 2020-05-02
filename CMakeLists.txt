cmake_minimum_required(VERSION 3.14)
project(EE319K_Final_Project_Initial_Testing)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 14)

add_executable(EE319K_Final_Project_Initial_Testing main.cpp Flash.cpp Flash.h SD.cpp SD.h animator.cpp animator.h
        ILI9341.cpp ILI9341.h controller.cpp controller.h metadata.h utils.h UART.cpp UART.h utils.h utils.cpp
        gameandwatch.cpp kirby.cpp entities.h stage.cpp stage.h collider.cpp colors_fdst.h colors_tower.h Audio.cpp
        Audio.h charactermenu.cpp charactermenu.h colors_menu.h colors_charmenu.h colors_smashville.h colors_stagemenu.h
        colors_battlefield.h stagemenu.cpp stagemenu.h colors_eer.h colors_gregory.h
        WinScreen.cpp WinScreen.h valvano.cpp colors_w1gw.h colors_w1gwc.h colors_w1kb.h colors_w1kbc.h colors_w1va.h
        colors_w1vac.h colors_w2gw.h colors_w2gwc.h colors_w2kb.h colors_w2kbc.h colors_w2va.h colors_w2vac.h
        )
target_link_libraries(EE319K_Final_Project_Initial_Testing ${SDL2_LIBRARY} -lmingw32 -lSDL2main -lSDL2 -mwindows)
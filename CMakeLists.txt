cmake_minimum_required(VERSION 3.14)
project(EE319K_Final_Project_Initial_Testing)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 14)

add_executable(EE319K_Final_Project_Initial_Testing main.cpp Flash.cpp Flash.h SD.cpp SD.h animator.cpp animator.h ILI9341.cpp ILI9341.h controller.cpp controller.h metadata.h utils.h UART.cpp UART.h utils.h utils.cpp kirby.cpp entities.h stage.cpp stage.h collider.cpp colors_fdst.h colors_tower.h Audio.cpp Audio.h gameandwatch.cpp)
target_link_libraries(EE319K_Final_Project_Initial_Testing ${SDL2_LIBRARY} -lmingw32 -lSDL2main -lSDL2 -mwindows)
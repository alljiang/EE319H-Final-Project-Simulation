cmake_minimum_required(VERSION 3.14)
project(EE319K_Final_Project_Initial_Testing)

set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake)

find_package(SDL2 REQUIRED)

include_directories(${SDL2_INCLUDE_DIR})

set(CMAKE_CXX_STANDARD 14)

add_executable(EE319K_Final_Project_Initial_Testing main.cpp SRAM.cpp SRAM.h SDCard.cpp SDCard.h animator.cpp animator.h LCD.cpp LCD.h controller.cpp controller.h metadata.h)
target_link_libraries(EE319K_Final_Project_Initial_Testing ${SDL2_LIBRARY} -lmingw32 -lSDL2main -lSDL2 -mwindows)
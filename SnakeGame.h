#pragma once
#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <vector>
#include <ctime>
#include<iostream> //0
#include<ncurses.h>//0
#include<cstdlib>//0
#include<clocale>
#include <unistd.h>//0
#include<string>
#include<time.h>

using namespace std;

class SnakeGame {
public:
    SnakeGame(int width, int height);
    void run();

private:
    enum GameState {
        PLAYING,
        GAME_OVER
    };
    int map_width;
    int map_height;
    GameState gameState;
    int snake_length;
    vector<int> snake_x;
    vector<int> snake_y;
    int snake_dir_x;
    int snake_dir_y;

    int food_x;
    int food_y;
    int poison_x;
    int poison_y;
    bool poison_active;
    
    time_t food_time;
    time_t poison_time;
    time_t gate_time; 

    int foodCount;
    int poisonCount; 
    int gateCount;

    void initializeGame();
    void placeFood();
    void placePoison();
    void gameLoop();
    void checkCollisions();
    void updateScoreBoard(); 
    void placeGates(); 
    void teleportSnake();

    pair<int, int> gate1; // 첫 번째 게이트 위치
    pair<int, int> gate2; // 두 번째 게이트 위치
    bool gates_active; // 게이트 활성화 상태
    bool teleporting;

};

#endif

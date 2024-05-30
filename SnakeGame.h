#pragma once
#ifndef SNAKEGAME_H
#define SNAKEGAME_H

#include <vector>
#include <ctime>
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

    void initializeGame();
    void placeFood();
    void placePoison();
    void gameLoop();
    void checkCollisions();
};

#endif

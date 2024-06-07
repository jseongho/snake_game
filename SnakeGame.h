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
    SnakeGame(int level);
    

private:
    enum GameState {
        PLAYING,
        GAME_OVER,
        CLEAR
    };

    int map_width;  //맵 크기
    int map_height;
    int offset_x;
    int wait;   //게임 속도
    int speedChange;
    GameState gameState;    //게임 상태
    int snake_length;   //몸 길이
    vector<int> snake_x;
    vector<int> snake_y;
    int snake_dir_x;    //머리 방향
    int snake_dir_y;
    int level; 

    int food_x; //먹이 위치
    int food_y;
    int poison_x;   //독 위치
    int poison_y;
    
    time_t food_time;  //먹이, 독 등 갱신 시간
    time_t poison_time;
    time_t gate_time; 

    int foodCount;  //먹이, 독 등 횟수
    int poisonCount; 
    int gateCount;

    int lenMission;  //미션 조건
    int foodMission; 
    int poisonMission;
    int gateMission;
    bool MissionComplete;

    char foodComplete; //미션 성공 여부
    char poisonComplete;
    char gateComplete;

    void run(int level);    //게임 init
    void MapInit(int width, int height);    //맵 생성
    void gameLoop();    //게임 루프
    void placeFood();   //음식, 독, 게이트 생성
    void placePoison();
    void placeGates(); 
    void checkCollisions();     //충돌 확인
    void updateScoreBoard();    //스코어보드 갱신
    void missionComplete();
    void teleportSnake();       //게이트 진입시 텔레포트
    void updateMissionBoard();  //미션보드 갱신

    pair<int, int> gate1; // 첫 번째 게이트 위치
    pair<int, int> gate2; // 두 번째 게이트 위치
    bool gates_active; // 게이트 활성화 상태
    bool teleporting;

};

#endif

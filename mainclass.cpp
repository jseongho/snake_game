#include <iostream>
#include "SnakeGame.h"
#include <ncurses.h>
#include <unistd.h>
#include <ctime>// 랜덤 시드 설정을 위한 헤더

using namespace std;



SnakeGame::SnakeGame(int a) : gameState(PLAYING) {
    level = a;
    run(level); //게임 initial
}

void SnakeGame::MapInit(int width, int height) {
    map_height = height;  //맵 크기
    map_width = width;
    offset_x = map_width + 2;
}

void SnakeGame::run(int level) {
    if (level == 1) {  //레벨 생성
    MapInit(21,21);
    foodMission = 2; //미션 조건
    poisonMission = 1;
    gateMission = 0;
    wait = 250000; //게임 속도
    }
    if (level == 2) {
    MapInit(35,21);
    foodMission = 4; //미션 조건
    poisonMission = 3;
    gateMission = 1;
    wait = 200000;
    }
    if (level == 3) {
    MapInit(41,21);
    foodMission = 7; //미션 조건
    poisonMission = 5;
    gateMission = 2;
    wait = 150000;
    }
    if (level == 4) {
    MapInit(51,21);
    foodMission = 9; //미션 조건
    poisonMission = 7;
    gateMission = 4;
    wait = 100000;
    }
    speedChange = 20000;
    srand(time(nullptr));   // 랜덤 시드 설정

        // 뱀 초기화
    snake_length = 3;
    snake_x.resize(snake_length);
    snake_y.resize(snake_length);
    for (int i = 0; i < snake_length; ++i) {
        snake_x[i] = map_width / 2 - i;
        snake_y[i] = map_height / 2;
    }
    snake_dir_x = 1;
    snake_dir_y = 0;

    // 먹이 초기화
    placeFood();
    // 독 초기화
    placePoison();
    // 게이트 초기화
    placeGates();
    foodCount = 0;
    poisonCount = 0;

    gate_time = time(nullptr);

    gameLoop();


    if (gameState == GAME_OVER) {
        
        mvprintw(15,offset_x,"G A M E O V E R...");
        refresh();
        usleep(2300000);
    }
    if (gameState == CLEAR) {
        mvprintw(15,offset_x,"LEVEL %d CLEAR...",level);
        refresh();
        level += 1;
        if (level >= 4) {
            usleep(2300000);
            mvprintw(15,offset_x,"GAME CLEAR!");
            goto jump;
        }
        gameState = PLAYING;
        usleep(3300000);
        run(level);
    }
    jump :
        endwin(); // ncurses 종료
}

void SnakeGame::placeFood() {
    bool onSnake;
    do {
        onSnake = false;
        food_x = rand() % (map_width - 2) + 1;
        food_y = rand() % (map_height - 2) + 1;
        for (int i = 0; i < snake_length; ++i) {
            if (snake_x[i] == food_x && snake_y[i] == food_y) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);
    food_time = time(nullptr); // food 생성 시간 기록
}

void SnakeGame::placePoison() {
    bool onSnake;
    do {
        onSnake = false;
        poison_x = rand() % (map_width - 2) + 1;
        poison_y = rand() % (map_height - 2) + 1;
        for (int i = 0; i < snake_length; ++i) {
            if (snake_x[i] == poison_x && snake_y[i] == poison_y) {
                onSnake = true;
                break;
            }
        }
    } while (onSnake);

    poison_time = time(nullptr); // poison 생성 시간 기록
}

void SnakeGame::placeGates() {

    // 게이트 위치 설정
    do {
        int side = rand() % 4;  // 0: 상단, 1: 하단, 2: 좌측, 3: 우측

        if (side == 0) {  // 상단 벽
            gate1 = {rand() % (map_width - 2) + 1, 0};
        } else if (side == 1) {  // 하단 벽
            gate1 = {rand() % (map_width - 2) + 1, map_height - 1};
        } else if (side == 2) {  // 좌측 벽
            gate1 = {0, rand() % (map_height - 2) + 1};
        } else if (side == 3) {  // 우측 벽
            gate1 = {map_width - 1, rand() % (map_height - 2) + 1};
        }

        side = rand() % 4;

        if (side == 0) {
            gate2 = {rand() % (map_width - 2) + 1, 0};
        } else if (side == 1) {
            gate2 = {rand() % (map_width - 2) + 1, map_height - 1};
        } else if (side == 2) {
            gate2 = {0, rand() % (map_height - 2) + 1};
        } else if (side == 3) {
            gate2 = {map_width - 1, rand() % (map_height - 2) + 1};
        }

    } while (gate1 == gate2);
    gates_active = true;
    gate_time = time(nullptr);
}


void SnakeGame::teleportSnake() {
    teleporting = false;

    // 첫 번째 게이트를 통과했을 때
    if (snake_x[0] == gate1.first && snake_y[0] == gate1.second) {
        snake_x[0] = gate2.first;
        snake_y[0] = gate2.second;

        // 방향 조정
        if (gate2.first == 0) {
            snake_dir_x = 1;
            snake_dir_y = 0;
        } else if (gate2.first == map_width - 1) {
            snake_dir_x = -1;
            snake_dir_y = 0;
        } else if (gate2.second == 0) {
            snake_dir_x = 0;
            snake_dir_y = 1;
        } else if (gate2.second == map_height - 1) {
            snake_dir_x = 0;
            snake_dir_y = -1;
        }
        teleporting = true;
    }
    // 두 번째 게이트를 통과했을 때
    else if (snake_x[0] == gate2.first && snake_y[0] == gate2.second) {
        snake_x[0] = gate1.first;
        snake_y[0] = gate1.second;

        // 방향 조정
        if (gate1.first == 0) {
            snake_dir_x = 1;
            snake_dir_y = 0;
        } else if (gate1.first == map_width - 1) {
            snake_dir_x = -1;
            snake_dir_y = 0;
        } else if (gate1.second == 0) {
            snake_dir_x = 0;
            snake_dir_y = 1;
        } else if (gate1.second == map_height - 1) {
            snake_dir_x = 0;
            snake_dir_y = -1;
        }
        teleporting = true;
    }

    // 뱀의 머리 위치를 새 방향으로 이동
    if (teleporting) {
        snake_x[0] += snake_dir_x;
        snake_y[0] += snake_dir_y;
    }
}


void SnakeGame::gameLoop() {
    // ncurses 초기화
    initscr(); 
    noecho(); 
    curs_set(0); 
    keypad(stdscr, TRUE); 
    nodelay(stdscr, TRUE); 
    resize_term(140,80);
    foodCount = 0;  //미션 조건 초기화
    poisonCount = 0;
    gateCount = 0;
    foodComplete = ' ';
    poisonComplete = ' ';
    gateComplete = ' ';
    MissionComplete = false;
    bool moveStatus = false;
    
    while (gameState == PLAYING) {

        clear();
        
        // 맵의 벽 그리기
        mvprintw(0,0,"M");
        mvprintw(0,map_width-1,"M");
        mvprintw(map_height-1,0,"M");
        mvprintw(map_height-1,map_width-1,"M");

        for (int i = 1; i < map_height-1; ++i) {
            mvprintw(i, 0, "X");
            mvprintw(i, map_width - 1, "X");
        }
        for (int j = 1; j < map_width-1; ++j) {
            mvprintw(0, j, "X");
            mvprintw(map_height - 1, j, "X");
        }

        // 먹이 그리기
        mvprintw(food_y, food_x, "F");

        // 독 그리기
        mvprintw(poison_y, poison_x, "P");
        

        // 뱀 그리기
        mvprintw(snake_y[0], snake_x[0], "H");
        for (int i = 1; i < snake_length; ++i) {
            mvprintw(snake_y[i], snake_x[i], "O");
        }

        // 게이트 그리기
        mvprintw(gate1.second, gate1.first, "G");
        mvprintw(gate2.second, gate2.first, "G");
        
        //스코어, 미션보드 업데이트
        updateScoreBoard();
        updateMissionBoard();
        // 화면 업데이트
        refresh();
        usleep(wait);

        int ch = getch();
        if(ch == KEY_UP || ch == KEY_DOWN || ch == KEY_RIGHT || ch == KEY_LEFT){moveStatus = true;}
        switch (ch) {
            case KEY_UP:
                // 뱀의 방향을 위쪽으로 변경
                if (snake_dir_y == 1) {
                gameState = GAME_OVER;
                }
                if (snake_dir_y != 1 && snake_dir_y != -1) {
                    snake_dir_x = 0;
                    snake_dir_y = -1;
                }
                break;
            case KEY_DOWN:
                if (snake_dir_y == -1) {
                gameState = GAME_OVER;
                }
                // 뱀의 방향을 아래쪽으로 변경
                if (snake_dir_y != 1 && snake_dir_y != -1) {
                    snake_dir_x = 0;
                    snake_dir_y = 1;
                }
                break;
            case KEY_LEFT:
                if (snake_dir_x == 1) {
                gameState = GAME_OVER;
                }
                // 뱀의 방향을 왼쪽으로 변경
                if (snake_dir_x != -1 && snake_dir_x != 1) { 
                    snake_dir_x = -1;
                    snake_dir_y = 0;
                }
                break;
            case KEY_RIGHT:
                if (snake_dir_x == -1) {
                gameState = GAME_OVER;
                }
                // 뱀의 방향을 오른쪽으로 변경
                if (snake_dir_x != 1 && snake_dir_x != -1) { 
                    snake_dir_x = 1;
                    snake_dir_y = 0;
                }
                break;
        }

        // 뱀 이동
        for (int i = snake_length - 1; i > 0; --i) {
            snake_x[i] = snake_x[i - 1];
            snake_y[i] = snake_y[i - 1];
        }
        snake_x[0] += snake_dir_x;
        snake_y[0] += snake_dir_y;

        // 충돌 확인
        checkCollisions();
        if (difftime(time(nullptr), food_time) >= 10) {
            placeFood();
        }
        if (difftime(time(nullptr), poison_time) >= 10) {
            placePoison();
        }
        if (!teleporting && (difftime(time(nullptr), gate_time) >= 20)) {
            placeGates();
        }

        if (MissionComplete == true) { //클리어시 게임종료
        gameState = CLEAR;
        }
    }
    endwin();
}

void SnakeGame::updateScoreBoard() {
    mvprintw(1, offset_x, "Score Board");
    mvprintw(2, offset_x, "-----------");
    mvprintw(3, offset_x, "Length: %d", snake_length);
    mvprintw(4, offset_x, "Food: %d", foodCount);
    mvprintw(5, offset_x, "Poison: %d", poisonCount);
    mvprintw(6, offset_x, "Gate: %d", gateCount);
}
void SnakeGame::missionComplete() {
    if (foodCount >= foodMission) {
        foodComplete = 'V';
    } 
    if (poisonCount >= poisonMission) {
        poisonComplete = 'V';
    }
    if (gateCount >= gateMission) {
        gateComplete = 'V';
    }
    if (gateCount >= gateMission && poisonCount >= poisonMission && foodCount >= foodMission) {
        MissionComplete = true;
    }
}
void SnakeGame::updateMissionBoard() { 
    missionComplete();
    mvprintw(9, offset_x, "Mission Board");
    mvprintw(10, offset_x, "-------------");
    mvprintw(11, offset_x, "Food:%d (%c)",foodMission, foodComplete);
    mvprintw(12, offset_x, "Poison:%d (%c)",poisonMission, poisonComplete);
    mvprintw(13, offset_x, "Gate:%d (%c)",gateMission, gateComplete);
}

void SnakeGame::checkCollisions() {
    // 뱀의 머리가 자신의 몸과 충돌하는지 확인
    for (int i = 1; i < snake_length; ++i) {
        if (snake_x[0] == snake_x[i] && snake_y[0] == snake_y[i]) {

            gameState = GAME_OVER;
            return;
        }
    }

    // 벽에 부딪혔을 때 게임 종료
    if (!((snake_x[0] == gate1.first && snake_y[0] == gate1.second) || 
        (snake_x[0] == gate2.first && snake_y[0] == gate2.second))) {
        if (snake_x[0] <= 0 || snake_x[0] >= map_width - 1 || snake_y[0] <= 0 || snake_y[0] >= map_height - 1) {
            gameState = GAME_OVER;
        }
    }

    // 먹이를 먹었을 때
    if (snake_x[0] == food_x && snake_y[0] == food_y) {
        placeFood();
        snake_length++;
        foodCount++;
        snake_x.push_back(snake_x[snake_length - 2]);
        snake_y.push_back(snake_y[snake_length - 2]);
        wait -= speedChange;
    }

    // 독을 먹었을 때
    if (snake_x[0] == poison_x && snake_y[0] == poison_y) {
        placePoison();
        snake_length--;
        poisonCount++;
        snake_x.pop_back();
        snake_y.pop_back();
        wait += speedChange;
        
    }
    // 게이트에 닿았을때
    if (gates_active&&(snake_x[0] == gate1.first && snake_y[0] == gate1.second) ||
        (snake_x[0] == gate2.first && snake_y[0] == gate2.second)) {
        gateCount++;
        teleportSnake();
    }
    if (snake_length < 3) {
            gameState = GAME_OVER;
        }
}

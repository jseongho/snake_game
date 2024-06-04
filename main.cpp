#include <iostream>
#include "SnakeGame.h"
#include <ncurses.h>
#include <unistd.h>
#include <ctime>// 랜덤 시드 설정을 위한 헤더

using namespace std;



SnakeGame::SnakeGame(int width, int height)
    : map_width(width), map_height(height), gameState(PLAYING) {
    initializeGame();
}

void SnakeGame::run() {
    gameLoop();
    endwin(); // ncurses 종료
    if (gameState == GAME_OVER) {
        cout << "Game Over" << endl;
    }
}

void SnakeGame::initializeGame() {
        // 랜덤 시드 설정
    srand(time(nullptr));

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
    foodCount = 0;
    poisonCount = 0;

    gates_active = false;
    gate_time = time(nullptr);
    //placeGates();
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
    poison_active = true;
    poison_time = time(nullptr); // poison 생성 시간 기록
}

void SnakeGame::placeGates() {
    gates_active = false;

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
    initscr(); // 화면 초기화
    noecho(); // 키 입력 출력 안함
    curs_set(0); // 커서 숨김
    keypad(stdscr, TRUE); // 화면에서 특수 키(화살표 등) 입력 가능하도록 설정
    nodelay(stdscr, TRUE); // 비동기 모드 설정 (키 입력을 기다리지 않음)
    resize_term(140,80);

    // 게임 루프
    while (gameState == PLAYING) {

        clear();
        
        // 맵의 벽 그리기
        for (int i = 0; i < map_height; ++i) {
            mvprintw(i, 0, "x");
            mvprintw(i, map_width - 1, "x");
        }
        for (int j = 0; j < map_width; ++j) {
            mvprintw(0, j, "x");
            mvprintw(map_height - 1, j, "x");
        }

        // 먹이 그리기
        mvprintw(food_y, food_x, "X");

        // 독 그리기
        if (poison_active) {
            mvprintw(poison_y, poison_x, "P");
        }

        // 뱀 그리기
        mvprintw(snake_y[0], snake_x[0], "H");
        for (int i = 1; i < snake_length; ++i) {
            mvprintw(snake_y[i], snake_x[i], "O");
        }

        if (gates_active) {
            mvprintw(gate1.second, gate1.first, "G");
            mvprintw(gate2.second, gate2.first, "G");
        }

        updateScoreBoard();
        // 화면 업데이트
        refresh();
        usleep(300000);
        // 사용자 입력 처리
        int ch = getch();
        switch (ch) {
            case KEY_UP:
                // 뱀의 방향을 위쪽으로 변경
                if (snake_dir_y == 1) {
                gameState = GAME_OVER;
                }
                if (snake_dir_y != 1) {
                    snake_dir_x = 0;
                    snake_dir_y = -1;
                }
                break;
            case KEY_DOWN:
                if (snake_dir_y == -1) {
                gameState = GAME_OVER;
                }
                // 뱀의 방향을 아래쪽으로 변경
                if (snake_dir_y != -1) {
                    snake_dir_x = 0;
                    snake_dir_y = 1;
                }
                break;
            case KEY_LEFT:
                if (snake_dir_x == 1) {
                gameState = GAME_OVER;
                }
                // 뱀의 방향을 왼쪽으로 변경
                if (snake_dir_x != 1) {
                    snake_dir_x = -1;
                    snake_dir_y = 0;
                }
                break;
            case KEY_RIGHT:
                if (snake_dir_x == -1) {
                gameState = GAME_OVER;
                }
                // 뱀의 방향을 오른쪽으로 변경
                if (snake_dir_x != -1) {
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
        if (difftime(time(nullptr), food_time) >= 5) {
            placeFood();
        }
        if (poison_active && difftime(time(nullptr), poison_time) >= 5) {
            placePoison();
        }
        if (!teleporting && (!gates_active || difftime(time(nullptr), gate_time) >= 5)) {
            placeGates();
        }
    }
    endwin();
}

void SnakeGame::updateScoreBoard() {
    int offset_x = map_width + 2;
    mvprintw(1, offset_x, "Score Board");
    mvprintw(2, offset_x, "-----------");
    mvprintw(3, offset_x, "Length: %d", snake_length);
    mvprintw(4, offset_x, "Food: %d", foodCount);
    mvprintw(5, offset_x, "Poison: %d", poisonCount);
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
    if (!(gates_active && 
        ((snake_x[0] == gate1.first && snake_y[0] == gate1.second) || 
         (snake_x[0] == gate2.first && snake_y[0] == gate2.second)))) {
        if (snake_x[0] <= 0 || snake_x[0] >= map_width - 1 || snake_y[0] <= 0 || snake_y[0] >= map_height - 1) {
            gameState = GAME_OVER;
        }
    }

    // 먹이를 먹었을 때
    if (snake_x[0] == food_x && snake_y[0] == food_y) {
        placeFood();
        snake_length++;
        snake_x.push_back(snake_x[snake_length - 2]);
        snake_y.push_back(snake_y[snake_length - 2]);
        foodCount++;
    }

    // 독을 먹었을 때
    if (poison_active && snake_x[0] == poison_x && snake_y[0] == poison_y) {
        placePoison();
        snake_length--;
        poisonCount++;
        
    }
    if (gates_active && (snake_x[0] == gate1.first && snake_y[0] == gate1.second) ||
        (snake_x[0] == gate2.first && snake_y[0] == gate2.second)) {
        teleportSnake();
    }
    if (snake_length < 3) {
            gameState = GAME_OVER;
        }
}

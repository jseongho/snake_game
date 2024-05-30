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

void SnakeGame::gameLoop() {
    // ncurses 초기화
    initscr(); // 화면 초기화
    noecho(); // 키 입력 출력 안함
    curs_set(0); // 커서 숨김
    keypad(stdscr, TRUE); // 화면에서 특수 키(화살표 등) 입력 가능하도록 설정
    nodelay(stdscr, TRUE); // 비동기 모드 설정 (키 입력을 기다리지 않음)

    // 게임 루프
    while (gameState == PLAYING) {
        // 화면 초기화
        clear();
        
        // 맵의 벽 그리기
        for (int i = 0; i < map_height; ++i) {
            mvprintw(i, 0, "|");
            mvprintw(i, map_width - 1, "|");
        }
        for (int j = 0; j < map_width; ++j) {
            mvprintw(0, j, "-");
            mvprintw(map_height - 1, j, "-");
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
        // 화면 업데이트
        refresh();
        // 0.5초 대기
        usleep(500000);
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
    }
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
    if (snake_x[0] <= 0 || snake_x[0] >= map_width - 1 || snake_y[0] <= 0 || snake_y[0] >= map_height - 1) {
        gameState = GAME_OVER;
        return;
    }

    // 먹이를 먹었을 때
    if (snake_x[0] == food_x && snake_y[0] == food_y) {
        placeFood();
        snake_length++;
        snake_x.push_back(snake_x[snake_length - 2]);
        snake_y.push_back(snake_y[snake_length - 2]);
    }

    // 독을 먹었을 때
    if (poison_active && snake_x[0] == poison_x && snake_y[0] == poison_y) {
        placePoison();
        snake_length--;

        if (snake_length < 3) {
            gameState = GAME_OVER;
        }
    }
}
int main() {
SnakeGame game(21, 21);
game.run();
return 0;
}

#include "SnakeGame.h"


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

#include <3ds.h>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <atomic>

#define SCREEN_WIDTH 50
#define SCREEN_HEIGHT 20
#define SCREEN_PIXEL_WIDTH 400
#define SCREEN_PIXEL_HEIGHT 240
#define MOVE_DELAY 5

const int CHARACTER_WIDTH = 8;
const int CHARACTER_HEIGHT = 16;
PrintConsole topScreen, bottomScreen;

struct PacMan {
    int x, y;
    char direction;
    int score;
};

char gameMaze[SCREEN_HEIGHT][SCREEN_WIDTH + 1];
PacMan pacman = {1, 16, 'R', 0};
int timerDuration = 0;
bool gameRunning = false;
std::atomic<bool> timerRunning(false);

void initializeGameMaze() {
    strcpy(gameMaze[0], "#################################################");
    strcpy(gameMaze[1], "# ............................................. #");
    strcpy(gameMaze[2], "# .###. .#### . #### . . . #### . ####. . ### . #");
    strcpy(gameMaze[3], "# .###. .#### . #### . ## . . . . ####. . ### . #");
    strcpy(gameMaze[4], "# . . . .#### . #### . ## . . . . . . . . . . . #");
    strcpy(gameMaze[5], "####### . . . . #### . ## . . . . . . . . . . . #");
    strcpy(gameMaze[6], "####### .#### . #### . ##  ## . . ### . . ### . #");
    strcpy(gameMaze[7], "# . . . .#### . #### . ##  ## . . ### . . ### . #");
    strcpy(gameMaze[8], "# . . . . . . . . . . . . . . . . . . . . . . . #");
    strcpy(gameMaze[9], "####### . ######################### . ###########");
    strcpy(gameMaze[10], "# . . . . ### . ### . . # . . . . . . . . . . . #");
    strcpy(gameMaze[11], "# . . . . ### . ### . . #  ####  #### . . ####. #");
    strcpy(gameMaze[12], "# . . . . . . . . . . . . . . . . . . . . . . . #");
    strcpy(gameMaze[13], "####### .#### . ### . # . ### . ####. . .#### . #");
    strcpy(gameMaze[14], "####### .#### . ### . # . . . . . . . . . . . . #");
    strcpy(gameMaze[15], "####### .#### . ### . # . . . . . . . . . . . . #");
    strcpy(gameMaze[16], "#       .#### . ### . # . ### . ### . . . ### . #");
    strcpy(gameMaze[17], "#     # . . . . . . . . . . . . . . . . . . . . #");
    strcpy(gameMaze[18], "#################################################");

    pacman.x = 1;
    pacman.y = 17;
    pacman.score = 0;
}

void drawMaze() {
    consoleSelect(&topScreen);
    consoleClear();
    
    printf("\x1b[0;0HScore: %d  Time: %d\n", pacman.score, timerDuration);
    
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (x == pacman.x && y == pacman.y) {
                printf("C");
            } else {
                printf("%c", gameMaze[y][x]);
            }
        }
        printf("\n");
    }
}

void movePacMan() {
    int newX = pacman.x;
    int newY = pacman.y;

    hidScanInput();
    u32 kHeld = hidKeysHeld();
    
    if (kHeld & KEY_UP) {
        newY--;
        pacman.direction = 'U';
    }
    if (kHeld & KEY_DOWN) {
        newY++;
        pacman.direction = 'D';
    }
    if (kHeld & KEY_LEFT) {
        newX--;
        pacman.direction = 'L';
    }
    if (kHeld & KEY_RIGHT) {
        newX++;
        pacman.direction = 'R';
    }

    if (newX >= 0 && newX < SCREEN_WIDTH && newY >= 0 && newY < SCREEN_HEIGHT && gameMaze[newY][newX] != '#') {
        if (gameMaze[newY][newX] == '.') {
            pacman.score += 10;
            gameMaze[newY][newX] = ' ';
        }
        pacman.x = newX;
        pacman.y = newY;
    }
}

bool allDotsCollected() {
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (gameMaze[y][x] == '.') return false;
        }
    }
    return true;
}

void chooseDifficulty() {
    consoleSelect(&bottomScreen);
    consoleClear();
    printf("\x1b[0;0HChoose Difficulty:\n");
    printf("Press A for Easy (4 min)\n");
    printf("Press B for Medium (2.5 min)\n");
    printf("Press X for Hard (2 min)\n");

    while (true) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_A) {
            timerDuration = 240;
            break;
        } else if (kDown & KEY_B) {
            timerDuration = 150;
            break;
        } else if (kDown & KEY_X) {
            timerDuration = 120;
            break;
        }
    }
}

int main() {
    gfxInitDefault();
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);

    consoleSelect(&bottomScreen);
    printf("Pac-Man on 3DS\n");
    printf("Press A to start the game.\n");
    printf("Press START to exit.\n");

    initializeGameMaze();
    u64 lastTime = osGetTime();

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;

        if (kDown & KEY_A && !gameRunning) {
            consoleClear();
            initializeGameMaze();
            chooseDifficulty();
            gameRunning = true;
            timerRunning = true;
            lastTime = osGetTime();
        }

        if (gameRunning) {
            movePacMan();
            
            u64 currentTime = osGetTime();
            if (currentTime - lastTime >= 1000) {
                if (timerDuration > 0) {
                    timerDuration--;
                }
                lastTime = currentTime;
            }

            if (timerDuration <= 0) {
                gameRunning = false;
                consoleSelect(&bottomScreen);
                printf("\x1b[10;10HGame Over! Press A to play again.");
            } else if (allDotsCollected()) {
                gameRunning = false;
                consoleSelect(&bottomScreen);
                printf("\x1b[10;10HYou Win! Press A to play again.");
            }

            drawMaze();
        }

        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
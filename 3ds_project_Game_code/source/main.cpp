#include <3ds.h>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <thread>

#define SCREEN_WIDTH 50  
#define SCREEN_HEIGHT 20 
#define SCREEN_PIXEL_WIDTH 400 
#define SCREEN_PIXEL_HEIGHT 240 
#define MOVE_DELAY 5 

#define PADDING_LEFT ((SCREEN_PIXEL_WIDTH - (SCREEN_WIDTH * CHARACTER_WIDTH)) / 2) 
#define PADDING_TOP ((SCREEN_PIXEL_HEIGHT - (SCREEN_HEIGHT * CHARACTER_HEIGHT)) / 2)

const int CHARACTER_WIDTH = 8;  
const int CHARACTER_HEIGHT = 16;

struct PacMan {
    int x, y;
    char direction;
    int score;
};

char gameMaze[SCREEN_HEIGHT][SCREEN_WIDTH + 1]; // Copy for gameplay
PacMan pacman = {1, 16, 'R', 0};

PrintConsole topScreen, bottomScreen; 
int remainingTime = 0; // Variable to track the remaining time

void initializeMaze() {
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
    
    pacman.score = 0; 
    pacman.x = 1; 
    pacman.y = 16;
}

void drawMaze() {
    consoleSelect(&topScreen); 
    consoleClear(); 
    
    printf("Score: %d\n", pacman.score); 
    printf("Time: %d seconds\n", remainingTime); // Show remaining time

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (x == pacman.x && y == pacman.y) {
                printf("P");
            } else {
                printf("%c", gameMaze[y][x]); 
            }
        }
        printf("\n"); 
    }

    gfxFlushBuffers(); 
    gfxSwapBuffers(); 
    gspWaitForVBlank(); 
}

void renderPauseMenu() {
    consoleSelect(&bottomScreen);
    printf("\x1b[10;10H--- PAUSE MENU ---");
    printf("\x1b[12;10HPress A to Resume");
    printf("\x1b[14;10HPress START to Quit");
}

void movePacMan() {
    int newX = pacman.x;
    int newY = pacman.y;

    switch (pacman.direction) {
        case 'U': newY--; break;
        case 'D': newY++; break;
        case 'L': newX--; break;
        case 'R': newX++; break;
    }

    if (newX >= 0 && newX < SCREEN_WIDTH && newY >= 0 && newY < SCREEN_HEIGHT && gameMaze[newY][newX] != '#') {
        pacman.x = newX;
        pacman.y = newY;

        if (gameMaze[newY][newX] == '.') { 
            gameMaze[newY][newX] = ' '; 
            pacman.score += 10; 
        }
    }
}

bool allDotsCollected() {
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (gameMaze[y][x] == '.') {
                return false; 
            }
        }
    }
    return true; 
}

// Function to choose difficulty
void chooseDifficulty() {
    consoleSelect(&bottomScreen);
    printf("Choose Difficulty:\n");
    printf("1. Easy (4 min)\n");
    printf("2. Medium (2.5 min)\n");
    printf("3. Hard (2 min)\n");

    while (true) {
        hidScanInput();
        u32 kDown = hidKeysDown();
        if (kDown & KEY_A) { 
            remainingTime = 240; // Easy: 4 min
            break; 
        } else if (kDown & KEY_B) {
            remainingTime = 150; // Medium: 2.5 min
            break;
        } else if (kDown & KEY_X) {
            remainingTime = 120; // Hard: 2 min
            break;
        }
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
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

    gfxSetDoubleBuffering(GFX_BOTTOM, false);
    gfxSetDoubleBuffering(GFX_TOP, true); // Enable double buffering on top screen
    initializeMaze(); // Initialize the maze with the layout

    bool gameRunning = false; 
    int moveCounter = 0; 

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;

        if (kDown & KEY_A && !gameRunning) {
            chooseDifficulty(); // Select difficulty before starting the game
            gameRunning = true; 
            consoleClear(); 
            consoleSelect(&bottomScreen);
            printf("Game started! Use arrows to move Pac-Man.\n");
        }

        if (gameRunning) {
            if (remainingTime > 0) {
                // Check for directional inputs
                if (kDown & KEY_UP) pacman.direction = 'U';
                if (kDown & KEY_DOWN) pacman.direction = 'D';
                if (kDown & KEY_LEFT) pacman.direction = 'L';
                if (kDown & KEY_RIGHT) pacman.direction = 'R';

                moveCounter++;
                if (moveCounter >= MOVE_DELAY) {
                    movePacMan(); 
                    moveCounter = 0; 
                }

                // Decrement timer every second (60 frames ~= 1 second)
                static int frameCount = 0;
                frameCount++;
                if (frameCount >= 60) { // Every 60 frames
                    remainingTime--;
                    frameCount = 0; 
                }

                // Pause game if SELECT button is pressed
                if (kDown & KEY_SELECT) {
                    renderPauseMenu();
                    bool inPauseMenu = true; 
                    while (inPauseMenu) {
                        hidScanInput(); 
                        u32 pauseInput = hidKeysDown();
                        renderPauseMenu(); 

                        if (pauseInput & KEY_START) {
                            gameRunning = false; 
                            break;
                        }

                        if (pauseInput & KEY_A) {
                            consoleClear();
                            inPauseMenu = false; 
                        }

                        gfxFlushBuffers();
                        gfxSwapBuffers();
                        gspWaitForVBlank();
                    }
                }

                if (allDotsCollected()) {
                    consoleSelect(&bottomScreen);
                    printf("Congratulations! All dots collected!\n");
                    initializeMaze(); // Reset the maze
                    printf("Press A to start the game again.\n");
                    gameRunning = false; 
                } else {
                    drawMaze();
                }
            } else {
                consoleSelect(&bottomScreen);
                printf("Game Over! Time's up!\n");
                printf("Press A to restart.\n");
                gameRunning = false; // Reset game state
            }
        }
    }

    gfxExit(); 
    return 0; 
}
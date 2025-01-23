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

#define CHARACTER_WIDTH 8  
#define CHARACTER_HEIGHT 16

class Game;

class PacMan {
public:
    int x, y;
    char direction;
    int score;

    PacMan() : x(1), y(16), direction('R'), score(0) {}

    void move(Game& game);  // Pass game reference to allow access to gameMaze

private:
    bool isValidMove(int newX, int newY, Game& game);
};

class Game {
public:
    Game() : remainingTime(0), moveCounter(0), gameRunning(false) {
        initializeMaze();
    }

    void run() {
        while (aptMainLoop()) {
            hidScanInput();
            u32 kDown = hidKeysDown();

            if (kDown & KEY_START) break;

            if (kDown & KEY_A && !gameRunning) {
                chooseDifficulty();
                gameRunning = true;
                consoleClear(); 
                consoleSelect(&bottomScreen);
                printf("Game started! Use arrows to move Pac-Man.\n");
            }

            if (gameRunning) {
                handleInput(kDown);
                updateGame();
                drawMaze();
            }
        }
        gfxExit(); 
    }

    friend class PacMan;  // Allow PacMan access to private members

private:
    char gameMaze[SCREEN_HEIGHT][SCREEN_WIDTH + 1];
    PacMan pacman;
    int remainingTime;
    int moveCounter;
    bool gameRunning;

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
        printf("Time: %d seconds\n", remainingTime); 

        for (int y = 0; y < SCREEN_HEIGHT; ++y) {
            printf("%s\n", gameMaze[y]);
        }

        // Draw Pac-Man at the current position
        if (pacman.x >= 0 && pacman.x < SCREEN_WIDTH && pacman.y >= 0 && pacman.y < SCREEN_HEIGHT) {
            printf("\x1b[%d;%dHP", pacman.y + 2, pacman.x + 1);
        }

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    void handleInput(u32 kDown) {
        if (remainingTime > 0) {
            if (kDown & KEY_UP) pacman.direction = 'U';
            if (kDown & KEY_DOWN) pacman.direction = 'D';
            if (kDown & KEY_LEFT) pacman.direction = 'L';
            if (kDown & KEY_RIGHT) pacman.direction = 'R';

            moveCounter++;
            if (moveCounter >= MOVE_DELAY) {
                pacman.move(*this); // Pass the current game instance
                moveCounter = 0;
            }

            static int frameCount = 0;
            frameCount++;
            if (frameCount >= 60) { // Every 60 frames
                remainingTime--;
                frameCount = 0; 
            }
        }
    }

    void chooseDifficulty() {
        consoleSelect(&bottomScreen);
        printf("Choose Difficulty:\n");
        printf("A Easy (4 min)\n");
        printf("B Medium (2.5 min)\n");
        printf("X Hard (2 min)\n");

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

    void updateGame() {
        // Update game logic here if necessary
    }
};

void PacMan::move(Game& game) {
    int newX = x;
    int newY = y;

    switch (direction) {
        case 'U': newY--; break;
        case 'D': newY++; break;
        case 'L': newX--; break;
        case 'R': newX++; break;
    }

    // Check for valid move
    if (isValidMove(newX, newY, game)) {
        x = newX;
        y = newY;

        if (game.gameMaze[newY][newX] == '.') { 
            game.gameMaze[newY][newX] = ' '; 
            score += 10; 
        }
    }
}

bool PacMan::isValidMove(int newX, int newY, Game& game) {
    return (newX >= 0 && newX < SCREEN_WIDTH && newY >= 0 && newY < SCREEN_HEIGHT && game.gameMaze[newY][newX] != '#');
}

int main() {
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL); // Use NULL for default console
    consoleInit(GFX_BOTTOM, NULL); // Use NULL for default console

    Game pacmanGame;
    pacmanGame.run();

    return 0; 
}
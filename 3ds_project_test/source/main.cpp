#include <3ds.h>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>

// Define constants for screen dimensions and move delay
#define SCREEN_WIDTH 50  
#define SCREEN_HEIGHT 20 
#define MOVE_DELAY 20 // Increased to slow down the movement

class Game; // Forward declaration

class PacMan {
public:
    int x, y;            // Current position of Pac-Man on the maze
    char direction;      // Current movement direction ('U', 'D', 'L', 'R')
    int score;           // Current score of the player

    PacMan() : x(1), y(16), direction(' '), score(0) {} // Starting at the bottom left corner with no initial direction

    void move(Game& game);

private:
    bool isValidMove(int newX, int newY, Game& game);
};

class Game {
public:
    Game() : remainingTime(0), moveCounter(0), gameRunning(false), startTime(std::chrono::steady_clock::now()) {
        consoleInit(GFX_TOP, &topConsole);  
        consoleInit(GFX_BOTTOM, &bottomConsole); 
        initializeMaze(); 
    }

    void run() {
        auto lastFrameTime = std::chrono::steady_clock::now();
        
        while (aptMainLoop()) {
            hidScanInput();
            u32 kDown = hidKeysDown(); 

            if (kDown & KEY_START) break; 

            if (kDown & KEY_A && !gameRunning) {
                chooseDifficulty(); 
                gameRunning = true; 
                consoleClear(); 
                consoleSelect(&bottomConsole); 
                printf("Game started! Use arrows to move Pac-Man.\n"); 
            }

            if (gameRunning) {
                auto currentTime = std::chrono::steady_clock::now();
                auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastFrameTime).count();
    
                if (elapsedSeconds > 0) { 
                    if (remainingTime > 0) remainingTime--; 
                        lastFrameTime = currentTime; // Reset frame time
                    }
                }
            }
        gfxExit();
    }

    friend class PacMan;  

private:
    char gameMaze[SCREEN_HEIGHT][SCREEN_WIDTH + 1];
    PacMan pacman; 
    int remainingTime; 
    int moveCounter; 
    bool gameRunning; 
    std::chrono::steady_clock::time_point startTime; 
    PrintConsole topConsole; 
    PrintConsole bottomConsole; 

    void initializeMaze() {
        // Set the maze layout using strcpy
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
    }

    void drawMaze() {
        consoleSelect(&topConsole); 
        printf("\x1b[2J\x1b[H"); // Clear screen

        for (int counter1 = 0; counter1 < SCREEN_HEIGHT; counter1++) {
            for (int counter2 = 0; counter2 < SCREEN_WIDTH; counter2++) {
                // Draw Pac-Man in the maze
                if (counter2 == pacman.x && counter1 == pacman.y) {
                    printf("P"); // Represent Pac-Man with 'P'
                } else {
                    printf("%c", gameMaze[counter1][counter2]); // Draw the maze
                }
            }
            printf("\n");
        }
        consoleSelect(&bottomConsole); 
        printf("Score: %d | Time Left: %d\n", pacman.score, remainingTime);
    }

    void handleInput(u32 kDown) {
        if (remainingTime > 0) {
            // Update Pac-Man's direction based on key presses
            if (kDown & KEY_UP) pacman.direction = 'U';
            else if (kDown & KEY_DOWN) pacman.direction = 'D';
            else if (kDown & KEY_LEFT) pacman.direction = 'L';
            else if (kDown & KEY_RIGHT) pacman.direction = 'R';

            moveCounter++; // Increment move counter

            // Move Pac-Man if the delay has been reached
            if (moveCounter >= MOVE_DELAY) {
                pacman.move(*this); // Pass the current game instance to Pac-Man's move function
                moveCounter = 0; // Reset move counter
            }
        }
    }

    void chooseDifficulty() {
        consoleSelect(&bottomConsole); 
        printf("Select Difficulty: A - Easy (4 mins), B - Medium (2.5 mins), X - Hard (2 mins)\n");
        
        while (true) {
            hidScanInput();
            u32 kDown = hidKeysDown(); 
            if (kDown & KEY_A) { 
                remainingTime = 240; // Easy: 4 minutes
                break; 
            } else if (kDown & KEY_B) {
                remainingTime = 150; // Medium: 2.5 minutes
                break;
            } else if (kDown & KEY_X) {
                remainingTime = 120; // Hard: 2 minutes
                break;
            }
        }
        consoleClear();
    }

    void updateGame() {
        // Add any game logic updates needed here
    }
};

void PacMan::move(Game& game) {
    int newX = x, newY = y; // Store new coordinates

    // Update coordinates based on direction
    if (direction == 'U') newY--;
    else if (direction == 'D') newY++;
    else if (direction == 'L') newX--;
    else if (direction == 'R') newX++;

    // Check for valid moves
    if (isValidMove(newX, newY, game)) {
        x = newX;
        y = newY;

        // Check for pellet collision
        if (game.gameMaze[y][x] == '.') {
            score += 10; // Increase score for eating a pellet
            game.gameMaze[y][x] = ' '; // Remove pellet from maze
        }
    }
}

bool PacMan::isValidMove(int newX, int newY, Game& game) {
    // Validate that the new coordinates are within bounds and not hitting a wall
    return (newX >= 0 && newX < SCREEN_WIDTH && newY >= 0 && newY < SCREEN_HEIGHT && game.gameMaze[newY][newX] != '#');
}

int main() {
    gfxInitDefault();
    Game pacmanGame; 
    pacmanGame.run(); 
    return 0; 
}
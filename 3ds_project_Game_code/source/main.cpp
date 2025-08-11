#include <3ds.h>
#include <cstdio>
#include <cstring>
#include <chrono>
#include <thread>

// Define constants for screen dimensions and move delay
#define SCREEN_WIDTH 50  
#define SCREEN_HEIGHT 20 
#define MOVE_DELAY 100 // Delay in milliseconds for movement (adjustable)

// Forward declaration of Game class
class Game;

class PacMan {
public:
    int x, y;            // Current position of Pac-Man
    char direction;      // Current movement direction ('U', 'D', 'L', 'R')
    int score;           // Current score of the player

    PacMan() : x(1), y(16), direction(' '), score(0) {}

    void move(Game& game);

private:
    bool isValidMove(int newX, int newY, Game& game);
};

class Game {
public:
    Game() : remainingTime(0), gameRunning(false) {
        // Initialize consoles for displaying game information
        gfxInitDefault();
        consoleInit(GFX_TOP, &topConsole);  
        consoleInit(GFX_BOTTOM, &bottomConsole); 
        initializeMaze(); 
    }

    void run() {
        auto lastMoveTime = std::chrono::steady_clock::now();
        auto lastFrameTime = std::chrono::steady_clock::now();

        while (aptMainLoop()) {
            hidScanInput();
            u32 kDown = hidKeysDown();

            // Exit the game on start button press
            if (kDown & KEY_START) break;

            // Start a new game on 'A' press
            if (kDown & KEY_A && !gameRunning) {
                chooseDifficulty();
                gameRunning = true;
                consoleClear();
                consoleSelect(&bottomConsole);
                printf("Game started! Use arrows to move Pac-Man.\n");
            }

            if (gameRunning) {
                auto currentTime = std::chrono::steady_clock::now();

                // Update the timer every second
                auto elapsedSeconds = std::chrono::duration_cast<std::chrono::seconds>(currentTime - lastFrameTime).count();
                if (elapsedSeconds > 0) {
                    if (remainingTime > 0) remainingTime--;
                    lastFrameTime = currentTime;
                }

                // Handle input and Pac-Man movement
                handleInput(kDown);
                auto elapsedMoveTime = std::chrono::duration_cast<std::chrono::milliseconds>(currentTime - lastMoveTime).count();
                if (elapsedMoveTime >= MOVE_DELAY) {
                    pacman.move(*this);
                    lastMoveTime = currentTime;
                }

                // Render the game
                drawMaze();

                // Check for game over condition
                if (remainingTime <= 0) {
                    consoleSelect(&topConsole);
                    printf("\x1b[HGame Over! Your score: %d\n", pacman.score);
                    gameRunning = false; // End the game
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
    bool gameRunning;
    PrintConsole topConsole;
    PrintConsole bottomConsole;

    // Initialize the game maze
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
    }

    // Render the maze and game state
    void drawMaze() {
        consoleSelect(&topConsole);
        printf("\x1b[H"); // Move cursor to the top-left

        for (int y = 0; y < SCREEN_HEIGHT; y++) {
            for (int x = 0; x < SCREEN_WIDTH; x++) {
                if (x == pacman.x && y == pacman.y) {
                    printf("P"); // Draw Pac-Man
                } else {
                    printf("%c", gameMaze[y][x]); // Draw maze walls or dots
                }
            }
            printf("\n");
        }

        // Display the score and remaining time
        consoleSelect(&bottomConsole);
        printf("Score: %d | Time Left: %d\n", pacman.score, remainingTime);
    }

    // Handle user inputs for movement
    void handleInput(u32 kDown) {
        if (kDown & KEY_UP) pacman.direction = 'U';
        else if (kDown & KEY_DOWN) pacman.direction = 'D';
        else if (kDown & KEY_LEFT) pacman.direction = 'L';
        else if (kDown & KEY_RIGHT) pacman.direction = 'R';
        else pacman.direction = ' '; // Reset direction if no key is pressed
    }

    // Choose the game difficulty
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
};

// Movement logic for Pac-Man
void PacMan::move(Game& game) {
    int newX = x, newY = y;

    // Determine the new position based on direction
    if (direction == 'U') newY--;
    else if (direction == 'D') newY++;
    else if (direction == 'L') newX--;
    else if (direction == 'R') newX++;

    // If the move is valid, update position
    if (isValidMove(newX, newY, game)) {
        x = newX;
        y = newY;

        // Consume dots and increase score
        if (game.gameMaze[y][x] == '.') {
            score += 10;
            game.gameMaze[y][x] = ' '; // Consume dot
        }
    }
}

// Validate the new move position
bool PacMan::isValidMove(int newX, int newY, Game& game) {
    return newX >= 0 && newX < SCREEN_WIDTH && newY >= 0 && newY < SCREEN_HEIGHT && game.gameMaze[newY][newX] != '#';
}

// Main entry point
int main() {
    Game pacmanGame;
    pacmanGame.run();
    return 0;
}

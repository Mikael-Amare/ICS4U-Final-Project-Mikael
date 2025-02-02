#include <3ds.h>
#include <cstdio>
#include <cstring>
#include <algorithm> // For std::min

#define SCREEN_WIDTH 50  // Width of the maze
#define SCREEN_HEIGHT 20 // Height of the maze
#define SCREEN_PIXEL_WIDTH 400 // 3DS screen width in pixels
#define SCREEN_PIXEL_HEIGHT 240 // 3DS screen height in pixels
#define MOVE_DELAY 5 // Delaying PMan movement

// Calculate padding for centering
#define PADDING_LEFT ((SCREEN_PIXEL_WIDTH - (SCREEN_WIDTH * CHARACTER_WIDTH)) / 2) 
#define PADDING_TOP ((SCREEN_PIXEL_HEIGHT - (SCREEN_HEIGHT * CHARACTER_HEIGHT)) / 2)

const int CHARACTER_WIDTH = 8;  // Width of each character in pixels (adjust based on your font)
const int CHARACTER_HEIGHT = 16; // Height of each character in pixels

struct PacMan {
    int x, y;
    char direction;
    int score;
};

const char maze[SCREEN_HEIGHT][SCREEN_WIDTH + 1] = {
    "#################################################",
    "# ............................................. #",
    "# .###. .#### . #### . . . #### . ####. . ### . #",
    "# .###. .#### . #### . ## . . . . ####. . ### . #",
    "# . . . .#### . #### . ## . . . . . . . . . . . #",
    "####### . . . . #### . ## . . . . . . . . . . . #",
    "####### .#### . #### . ##  ## . . ### . . ### . #",
    "# . . . .#### . #### . ##  ## . . ### . . ### . #",
    "# . . . . . . . . . . . . . . . . . . . . . . . #",
    "####### . ######################### . ###########",
    "# . . . . ### . ### . . # . . . . . . . . . . . #",
    "# . . . . ### . ### . . #  ####  #### . . ####. #",
    "# . . . . . . . . . . . . . . . . . . . . . . . #",
    "####### .#### . ### . # . ### . ####. . .#### . #",
    "####### .#### . ### . # . . . . . . . . . . . . #",
    "####### .#### . ### . # . . . . . . . . . . . . #",
    "#       .#### . ### . # . ### . ### . . . ### . #",
    "#     # . . . . . . . . . . . . . . . . . . . . #",
    "#################################################"
};

char gameMaze[SCREEN_HEIGHT][SCREEN_WIDTH + 1]; // Mutable maze
PacMan pacman = {1, 16, 'R', 0};

PrintConsole topScreen, bottomScreen; // Declare consoles globally

void initializeGameMaze() {
    for (int counter = 0; counter < SCREEN_HEIGHT; counter++) {
        // Ensure maze[counter] is appropriately null-terminated after the copy.
        if (strlen(maze[counter]) > SCREEN_WIDTH) {
            strncpy(gameMaze[counter], maze[counter], SCREEN_WIDTH);
            gameMaze[counter][SCREEN_WIDTH] = '\0'; // Null terminate the string
        } else {
            strcpy(gameMaze[counter], maze[counter]); // Use strcpy if it's safe
        }
    }
    pacman.score = 0; // Reset score
    pacman.x = 1; // Reset Pac-Man's initial position
    pacman.y = 16; // Reset Pac-Man's initial position
}

void drawMaze() {
    consoleSelect(&topScreen); // Draw on the top screen
    consoleClear(); // Clear the console before drawing
    
    printf("Score: %d\n", pacman.score); // Show the score

    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (x == pacman.x && y == pacman.y) {
                printf("P"); // Draw Pac-Man
            } else {
                printf("%c", gameMaze[y][x]); // Draw maze
            }
        }
        printf("\n"); // New line after each row
    }

    // Make sure to flush and swap buffers
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

    // Ensure the new position is within bounds and not a wall
    if (newX >= 0 && newX < SCREEN_WIDTH && newY >= 0 && newY < SCREEN_HEIGHT && gameMaze[newY][newX] != '#') {
        pacman.x = newX;
        pacman.y = newY;

        if (gameMaze[newY][newX] == '.') { // Collect pellet
            gameMaze[newY][newX] = ' '; // Clear pellet
            pacman.score += 10; // Update score
        }
    }
}

bool allDotsCollected() {
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (gameMaze[y][x] == '.') { // If there's a dot
                return false; // Not all dots collected
            }
        }
    }
    return true; // All dots are collected
}

int main() {
    gfxInitDefault();

    // Initialize consoles for both screens
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);

    // Static text on the bottom screen
    consoleSelect(&bottomScreen);
    printf("Pac-Man on 3DS\n");
    printf("Press A to start the game.\n");
    printf("Press START to exit.\n");

    // Disable double buffering for the bottom screen (static text won't need updates)
    gfxSetDoubleBuffering(GFX_BOTTOM, false);
    initializeGameMaze(); // Initialize the maze before the game loop

    bool gameRunning = false; // Flag to track whether the game is running
    int moveCounter = 0; // Timer for movement delay

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        // Exit the loop on START key press
        if (kDown & KEY_START) break;

        // Start the game if A is pressed
        if (kDown & KEY_A && !gameRunning) {
            gameRunning = true; // Set the game running flag
            consoleClear(); // Clear bottom console
            consoleSelect(&bottomScreen);
            printf("Game started! Use arrows to move Pac-Man.\n");
        }

        // If the game is running, process game logic
        if (gameRunning) {
            // Check for directional inputs
            if (kDown & KEY_UP) pacman.direction = 'U';
            if (kDown & KEY_DOWN) pacman.direction = 'D';
            if (kDown & KEY_LEFT) pacman.direction = 'L';
            if (kDown & KEY_RIGHT) pacman.direction = 'R';

            // Increment the move counter
            moveCounter++;
            if (moveCounter >= MOVE_DELAY) { // Only move Pac-Man every MOVE_DELAY frames
                movePacMan(); // Move Pac-Man based on direction
                moveCounter = 0; // Reset the move counter
            }

            // Pause game if SELECT button is pressed
            if (kDown & KEY_SELECT) {
                renderPauseMenu();
                bool inPauseMenu = true; // Set the pause state
                while (inPauseMenu) {
                    hidScanInput(); // Scan for input
                    u32 pauseInput = hidKeysDown();
                    renderPauseMenu(); // Draw the pause menu

                    // Check for input to exit the pause menu
                    if (pauseInput & KEY_START) {
                        gameRunning = false; // Quit the game
                        break;
                        }

                    // Clear pause menu artifacts when exiting
                    if (pauseInput & KEY_A) {
                        consoleClear();
                        inPauseMenu = false; // Resume the game
                        }

                    gfxFlushBuffers(); // Update screen
                    gfxSwapBuffers(); // Swap buffers
                    gspWaitForVBlank(); // Wait for vertical sync
                }
            }

            // Check if all dots are collected
            if (allDotsCollected()) {
                consoleSelect(&bottomScreen);
                printf("Congratulations! All dots collected!\n");
                initializeGameMaze(); // Reset the game maze
                printf("Press A to start the game again.\n");
                gameRunning = false; // Reset game state
            } else {
                // Switch to the top screen to draw the game
                drawMaze(); // Draw the maze with Pac-Man

                gfxFlushBuffers(); // Flush the graphics buffers
                gfxSwapBuffers(); // Swap the buffers to display
                gspWaitForVBlank(); // Wait for the vertical blank to prevent tearing
            }
        }
    }

    gfxExit(); // Clean up graphics before exiting
    return 0; // Return success
}
#include <3ds.h>
#include <cstdio>
#include <cstring>

#define SCREEN_WIDTH 50
#define SCREEN_HEIGHT 19

struct PacMan {
    int x, y;
    char direction;
    int score;
};

const char maze[SCREEN_HEIGHT][SCREEN_WIDTH + 1] = {
    "################################################",
    "# ............................................ #",
    "# .###. .#### . #### . . . #### . ####. . ####.#",
    "# .###. .#### . #### . ## . . . . ####. . ####.#",
    "# . . . .#### . #### . ## . . . . . . . . . . ##",
    "####### . . . . #### . ## . . . . . . . . . . ##",
    "####### .#### . #### . ##  ####  #### . . ####.#",
    "# . . . .#### . #### . ##  ####  #### . . ####.#",
    "# . . . . . . . . . . . . . . . . . . . . . . .#",
    "####### . ######################### . ##########",
    "# . . . . ### . ### . . # . . . . . . . . . . .#",
    "# . . . . ### . ### . . #  ####  #### . . ####.#",
    "# . . . . . . . . . . . . . . . . . . . . . . .#",
    "####### .#### . ### . # . ### . ####. . .#### .#",
    "####### .#### . ### . # . . . . . . . . . . . ##",
    "####### .#### . ### . # . . . . . . . . . . . ##",
    "#       .#### . ### . # . ### . ### . . . ### .#",
    "#     # . . . . . . . . . . . . . . . . . . . .#",
    "################################################"
};

char gameMaze[SCREEN_HEIGHT][SCREEN_WIDTH + 1]; // Mutable maze
PacMan pacman = {1, 16, 'R', 0};

PrintConsole topScreen, bottomScreen; // Declare consoles globally

void initializeGameMaze() {
    for (int counter = 0; counter < SCREEN_HEIGHT; counter++) {
        strncpy(gameMaze[counter], maze[counter], SCREEN_WIDTH + 1);
    }
    pacman.score = 0; // Reset score
    pacman.x = 1; // Reset Pac-Man's initial position
    pacman.y = 16; // Reset Pac-Man's initial position
}

void drawMaze() {
    consoleSelect(&topScreen); // Draw on the top screen
    consoleClear();
    printf("Score: %d\n", pacman.score);
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (x == pacman.x && y == pacman.y) {
                printf("P"); // Draw Pac-Man
            } else {
                printf("%c", gameMaze[y][x]); // Draw maze
            }
        }
        printf("\n"); // Move to the next line
    }
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

            movePacMan(); // Move Pac-Man based on direction

            // Check if all dots are collected
            if (allDotsCollected()) {
                consoleSelect(&bottomScreen);
                printf("Congratulations! All dots collected!\n");
                consoleClear();
                initializeGameMaze(); // Reset the game
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
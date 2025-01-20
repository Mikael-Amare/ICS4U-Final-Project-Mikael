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

void initializeGameMaze() {
    for (int counter = 0; counter < SCREEN_HEIGHT; counter++) {
        strncpy(gameMaze[counter], maze[counter], SCREEN_WIDTH + 1);
    }
}

void drawMaze() {
    consoleSelect(NULL); // Ensure drawing on the top screen
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

int main() {
    initializeGameMaze();
    gfxInitDefault();

    // Initialize consoles for both screens
    PrintConsole topScreen, bottomScreen;
    consoleInit(GFX_TOP, &topScreen);
    consoleInit(GFX_BOTTOM, &bottomScreen);

    // Static text on the bottom screen
    consoleSelect(&bottomScreen);
    printf("Pac-Man on 3DS\n");
    printf("Press START to exit.\n");

    // Disable double buffering for the bottom screen (static text won't need updates)
    gfxSetDoubleBuffering(GFX_BOTTOM, false);

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_A) break; // Use the A button to skip the start screen
        if (kDown & KEY_START) break; // Still keep the START button for exiting the game

        // Check for directional inputs
        if (kDown & KEY_UP) pacman.direction = 'U';
        if (kDown & KEY_DOWN) pacman.direction = 'D';
        if (kDown & KEY_LEFT) pacman.direction = 'L';
        if (kDown & KEY_RIGHT) pacman.direction = 'R';

        movePacMan(); // Move Pac-Man based on direction

        // Switch to the top screen to draw the game
        consoleSelect(&topScreen);
        drawMaze(); // Draw the maze with Pac-Man

        gfxFlushBuffers(); // Flush the graphics buffers
        gfxSwapBuffers(); // Swap the buffers to display
        gspWaitForVBlank(); // Wait for the vertical blank to prevent tearing
    }

    gfxExit(); // Clean up graphics before exiting
    return 0; // Return success
}
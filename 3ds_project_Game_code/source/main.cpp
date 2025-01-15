#include <3ds.h>
#include <cstdio>
#include <cstdlib>

// Screen size for console
#define SCREEN_WIDTH 50
#define SCREEN_HEIGHT 20

// PacMan structure
struct PacMan {
    int x, y;
    char direction;
};

// Game maze (simple example)
const char maze[SCREEN_HEIGHT][SCREEN_WIDTH + 1] = {
    "##################################################",
    "#                                                #",
    "#  ###   ####  ####       ####  ####    ####     #",
    "#  ###   ####  ####   ##  ####  ####    ####     #",
    "#        ####  ####   ##                     ####",
    "#######        ####   ##                     ####",
    "#######  ####  ####   ##  ####  ####    ####     #",
    "#       ####   ####   ##  ####  ####    ####     #",
    "#                                                #",
    "##################################################"
};

// Initialize PacMan
PacMan pacman = {1, 1, 'R'};

// Function to draw the maze and PacMan
void drawMaze() {
    consoleClear();
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (x == pacman.x && y == pacman.y) {
                printf("P");
            } else {
                printf("%c", maze[y][x]);
            }
        }
        printf("\n");
    }
}

// Function to handle movement
void movePacMan() {
    int newX = pacman.x;
    int newY = pacman.y;

    switch (pacman.direction) {
        case 'U': newY--; break;
        case 'D': newY++; break;
        case 'L': newX--; break;
        case 'R': newX++; break;
    }

    // Check for collisions with walls
    if (maze[newY][newX] != '#') {
        pacman.x = newX;
        pacman.y = newY;
    }
}

int main() {
    // Initialize services
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    printf("Pac-Man on 3DS\n");
    printf("Press START to exit.\n");

    while (aptMainLoop()) {
        // Scan for input
        hidScanInput();
        u32 kDown = hidKeysDown();

        // Exit game
        if (kDown & KEY_START) break;

        // Change direction
        if (kDown & KEY_UP) pacman.direction = 'U';
        if (kDown & KEY_DOWN) pacman.direction = 'D';
        if (kDown & KEY_LEFT) pacman.direction = 'L';
        if (kDown & KEY_RIGHT) pacman.direction = 'R';

        // Move PacMan
        movePacMan();

        // Draw the maze
        drawMaze();

        // Wait for the next frame
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    // Exit services
    gfxExit();
    return 0;
}
#include <3ds.h>
#include <cstdio>
#include <cstdlib>

// Screen size for console
#define SCREEN_WIDTH 50
#define SCREEN_HEIGHT 20

// PacMan structure
struct PacMan {
    int x, y;
    char direction;
};

// Game maze (simple example)
const char maze[SCREEN_HEIGHT][SCREEN_WIDTH + 1] = {
    "##################################################",
    "#                                                #",
    "#  ###   ####  ####       ####  ####    ####     #",
    "#  ###   ####  ####   ##  ####  ####    ####     #",
    "#        ####  ####   ##                      ####",
    "#######        ####   ##                      ####",
    "#######  ####  ####   ##  ####  ####    ####     #",
    "#        ####  ####   ##  ####  ####    ####     #",
    "#                                                #",
    "##################################################"
};

// Initialize PacMan
PacMan pacman = {1, 1, 'R'};

// Function to draw the maze and PacMan
void drawMaze() {
    consoleClear();
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (x == pacman.x && y == pacman.y) {
                printf("P");
            } else {
                printf("%c", maze[y][x]);
            }
        }
        printf("\n");
    }
}

// Function to handle movement
void movePacMan() {
    int newX = pacman.x;
    int newY = pacman.y;

    switch (pacman.direction) {
        case 'U': newY--; break;
        case 'D': newY++; break;
        case 'L': newX--; break;
        case 'R': newX++; break;
    }

    // Check for collisions with walls
    if (maze[newY][newX] != '#') {
        pacman.x = newX;
        pacman.y = newY;
    }
}

int main() {
    // Initialize services
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    printf("Pac-Man on 3DS\n");
    printf("Press START to exit.\n");

    while (aptMainLoop()) {
        // Scan for input
        hidScanInput();
        u32 kDown = hidKeysDown();

        // Exit game
        if (kDown & KEY_START) break;

        // Change direction
        if (kDown & KEY_UP) pacman.direction = 'U';
        if (kDown & KEY_DOWN) pacman.direction = 'D';
        if (kDown & KEY_LEFT) pacman.direction = 'L';
        if (kDown & KEY_RIGHT) pacman.direction = 'R';

        // Move PacMan
        movePacMan();

        // Draw the maze
        drawMaze();

        // Wait for the next frame
        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    // Exit services
    gfxExit();
    return 0;
}

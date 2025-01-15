#include <3ds.h>
#include <cstdio>
#include <cstdlib>

// Updated screen size
#define SCREEN_WIDTH 48
#define SCREEN_HEIGHT 19

// PacMan structure
struct PacMan {
    int x, y;
    char direction;
    int score;
};

// Updated game maze (adjusted to fit new screen size and allow pellets to disappear)
char maze[SCREEN_HEIGHT][SCREEN_WIDTH + 1] = {
    "###################################################",
    "# • • • • • • • • • • • • • • • • • • • • • • • • #",
    "# •###• •#### • #### • • • #### • ####• • ####• • #",
    "# •###• •#### • #### • ## • • • • ####• • ####• • #",
    "# • • • •#### • #### • ## • • • • • • • • • • • ###",
    "####### • • • • #### • ## • • • • • • • • • • • ###",
    "####### •#### • #### • ##  ####  #### • • ####• • #",
    "# • • • •#### • #### • ##  ####  #### • • ####• • #",
    "# • • • • • • • • • • • • • • • • • • • • • • • • #",
    "####### • ######################### • #############",
    "# • • • • ### • ### • • # • • • • • • • • • • • • #",
    "# • • • • ### • ### • • #  ####  #### • • ####• •##",
    "# • • • • • • • • • • • • • • • • • • • • • • • • #",
    "####### •#### • ### • # • ### • ####• • •#### • • #",
    "####### •#### • ### • # • • • • • • • • • • • • ###",
    "####### •#### • ### • # • • • • • • • • • • • • ###",
    "#       •#### • ### • # • ### • ### • • • ### • • #",
    "#     # • • • • • • • • • • • • • • • • • • • • • #",
    "###################################################"
};

// Initialize PacMan
PacMan pacman = {1, 16, 'R', 0};  // Starting position in the open area with score 0

// Function to draw the maze and PacMan
void drawMaze() {
    consoleClear();
    printf("Score: %d\n", pacman.score); // Display the score at the top
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (x == pacman.x && y == pacman.y) {
                printf("P"); // Draw Pac-Man
            } else {
                printf("%c", maze[y][x]); // Draw the maze
            }
        }
        printf("\n");
    }
}

// Function to handle movement and eating pellets
void movePacMan() {
    int newX = pacman.x;
    int newY = pacman.y;

    // Calculate the new position based on the direction
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

        // Check if Pac-Man eats a pellet
        if (maze[newY][newX] == '•') {
            maze[newY][newX] = ' '; // Remove the pellet
            pacman.score += 10;    // Increase the score
        }
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
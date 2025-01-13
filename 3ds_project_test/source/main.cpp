#include <3ds.h>
#include <citro2d.h>
#include <sfil.h>
#include <cstdio>

// Screen size for rendering
#define TILE_SIZE 16
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

// Textures for the game
sf2d_texture *pacmanTexture = NULL;
sf2d_texture *wallTexture = NULL;

// Function to draw the maze and PacMan
void drawMaze() {
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            int drawX = x * TILE_SIZE;
            int drawY = y * TILE_SIZE;

            // Draw wall or PacMan
            if (x == pacman.x && y == pacman.y) {
                sf2d_draw_texture(pacmanTexture, drawX, drawY);
            } else if (maze[y][x] == '#') {
                sf2d_draw_texture(wallTexture, drawX, drawY);
            }
        }
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
    consoleInit(GFX_BOTTOM, NULL); // Use bottom screen for text/debugging
    sf2d_init();
    romfsInit();

    // Load textures
    pacmanTexture = sf2d_load_texture_from_file("romfs:/pacman.t3x");
    wallTexture = sf2d_load_texture_from_file("romfs:/wall.t3x");

    if (!pacmanTexture || !wallTexture) {
        printf("Failed to load textures.\n");
        return 1;
    }

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

        // Render the screen
        sf2d_start_frame(GFX_TOP, GFX_LEFT);
        drawMaze();
        sf2d_end_frame();

        sf2d_swapbuffers();
    }

    // Clean up
    sf2d_free_texture(pacmanTexture);
    sf2d_free_texture(wallTexture);
    sf2d_fini();
    romfsExit();
    gfxExit();

    return 0;
}

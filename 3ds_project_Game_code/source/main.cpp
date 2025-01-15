#include <3ds.h>
#include <cstdio>
#include <cstdlib>
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
    consoleClear();
    printf("Score: %d\n", pacman.score);
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

    if (gameMaze[newY][newX] != '#') { // Check for walls
        pacman.x = newX;
        pacman.y = newY;

        if (gameMaze[newY][newX] == '.') { // Collect pellet
            gameMaze[newY][newX] = ' '; // Clear pellet
            pacman.score += 10;
        }
    }
}

int main() {
    initializeGameMaze();
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);

    printf("Pac-Man on 3DS\n");
    printf("Press START to exit.\n");

    while (aptMainLoop()) {
        hidScanInput();
        u32 kDown = hidKeysDown();

        if (kDown & KEY_START) break;

        if (kDown & KEY_UP) pacman.direction = 'U';
        if (kDown & KEY_DOWN) pacman.direction = 'D';
        if (kDown & KEY_LEFT) pacman.direction = 'L';
        if (kDown & KEY_RIGHT) pacman.direction = 'R';

        movePacMan();
        drawMaze();

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}
#include <3ds.h>
#include <cstdio>
#include <cstring>
#include <algorithm>
#include <chrono>
#include <thread>

// Define constants for screen dimensions and move delay
#define SCREEN_WIDTH 50  
#define SCREEN_HEIGHT 20 
#define MOVE_DELAY 5 

// Forward declaration of the Game class
class Game;

// PacMan class representing the player character
class PacMan {
public:
    int x, y;            // Current position of Pac-Man on the maze
    char direction;      // Current movement direction ('U', 'D', 'L', 'R')
    int score;           // Current score of the player

    // Constructor initializes position and direction
    PacMan() : x(1), y(16), direction('R'), score(0) {}

    // Function to move Pac-Man based on direction
    void move(Game& game);

private:
    // Function to check if a move is valid based on maze boundaries and walls
    bool isValidMove(int newX, int newY, Game& game);
};

// Game class managing the state of the game
class Game {
public:
    // Constructor initializes game state and console outputs
    Game() : remainingTime(0), moveCounter(0), gameRunning(false) {
        consoleInit(GFX_TOP, &topConsole);  // Initialize top screen console
        consoleInit(GFX_BOTTOM, &bottomConsole); // Initialize bottom screen console
        initializeMaze(); // Set up the maze layout
    }

// Main game loop
void run() {
    auto lastFrameTime = std::chrono::steady_clock::now(); // Record last frame time
    while (aptMainLoop()) {
        hidScanInput(); // Scan for user input
        u32 kDown = hidKeysDown(); // Get the keys currently pressed

        if (kDown & KEY_START) break; // Exit the game if START is pressed

        if (kDown & KEY_A && !gameRunning) {
            chooseDifficulty(); // Set game difficulty
            gameRunning = true; // Start the game
            consoleClear(); // Clear the console
            consoleSelect(&bottomConsole); // Switch to bottom screen
            printf("Game started! Use arrows to move Pac-Man.\n"); // Display start message
        }

        if (gameRunning) {
    auto currentTime = std::chrono::steady_clock::now();
    auto elapsedTime = std::chrono::duration_cast<std::chrono::seconds>(currentTime - startTime).count();

        // Update remainingTime based on elapsedTime
            if (elapsedTime >= 1 && remainingTime > 0) {
                remainingTime--; // Decrease remaining time every second
                startTime = currentTime; // Reset start time
            }

    handleInput(kDown); // Handle user input
    updateGame(); // Update game logic if necessary

    // Control the frame rate
    std::chrono::duration<double, std::milli> elapsedFrame = currentTime - lastFrameTime;
    if (elapsedFrame.count() >= 16.67) { // Roughly 60 FPS
        drawMaze(); // Draw the maze and Pac-Man
        lastFrameTime = currentTime; // Update last frame time
    }
}
    }
    gfxExit();
}
    // Allow PacMan class access to private members of Game
    friend class PacMan;  

private:
    char gameMaze[SCREEN_HEIGHT][SCREEN_WIDTH + 1]; // 2D array for the maze
    PacMan pacman; // Instance of PacMan
    int remainingTime; // Time left for the game
    int moveCounter; // Counter for moves to control movement speed
    bool gameRunning; // Flag to track if the game is currently running

    PrintConsole topConsole; // Console for the top screen
    PrintConsole bottomConsole; // Console for the bottom screen

    // Function to initialize the maze layout
    void initializeMaze() {
        // Define the maze layout using characters
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

        pacman.score = 0; // Reset Pac-Man's score
        pacman.x = 1; // Reset Pac-Man's starting x position
        pacman.y = 16; // Reset Pac-Man's starting y position
    }

void drawMaze() {
    consoleSelect(&topConsole); // Switch to top screen console
    consoleClear(); // Clear previous output

    // Draw the maze first
    printf("Score: %d\n", pacman.score); // Display current score
    printf("Time: %d seconds\n", remainingTime); // Display remaining time 

    // Display the maze layout including Pac-Man
    for (int y = 0; y < SCREEN_HEIGHT; ++y) {
        for (int x = 0; x < SCREEN_WIDTH; ++x) {
            if (y == pacman.y && x == pacman.x) {
                printf("P"); // Draw Pac-Man
            } else {
                printf("%c", gameMaze[y][x]); // Draw maze character
            }
        }
        printf("\n"); // New line after each row
    }

    gfxFlushBuffers(); // Flush graphics buffers to display
    gfxSwapBuffers(); // Swap buffers for double buffering
    gspWaitForVBlank(); // Wait for vertical blank to prevent tearing
}

// Function to handle user input for Pac-Man's movement
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

        // Manage countdown for remaining time
        static int frameCount = 0; // Frame counter for time update
        frameCount++;
        if (frameCount >= 60) { // Decrease time every 60 frames
            remainingTime--;
            frameCount = 0; 
        }
    }
}

    // Function to prompt the player to choose a difficulty level
    void chooseDifficulty() {
        consoleSelect(&bottomConsole); // Switch to bottom console
        printf("Choose Difficulty:\n");
        printf("A Easy (4 min)\n");
        printf("B Medium (2.5 min)\n");
        printf("X Hard (2 min)\n");

        while (true) {
            hidScanInput(); // Scan for user input
            u32 kDown = hidKeysDown(); // Get the keys currently pressed
            // Set remaining time based on difficulty selection
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
            gfxFlushBuffers(); // Update screen
            gfxSwapBuffers(); // Swap buffers for double buffering
            gspWaitForVBlank(); // Wait for vertical blank
        }
    }

    // Function to update game state; currently empty but can be extended
    void updateGame() {
        // Update game logic here if necessary
    }
};

// Function to handle Pac-Man's movement based on its direction
void PacMan::move(Game& game) {
    int newX = x; // New x position based on direction
    int newY = y; // New y position based on direction

    // Determine new position based on the current direction
    switch (direction) {
        case 'U': newY--; break; // Move up
        case 'D': newY++; break; // Move down
        case 'L': newX--; break; // Move left
        case 'R': newX++; break; // Move right
    }

    // Check for valid move
    if (isValidMove(newX, newY, game)) {
        // Clear the old position in the maze
        game.gameMaze[y][x] = ' '; // Set old position to empty space
        x = newX; // Update x position
        y = newY; // Update y position

        // If Pac-Man eats a pellet, increase score and clear the pellet
        if (game.gameMaze[newY][newX] == '.') { 
            game.gameMaze[newY][newX] = ' '; // Clear the pellet
            score += 10; // Increase score
        }
    }
}

// Function to check if the proposed move is valid
bool PacMan::isValidMove(int newX, int newY, Game& game) {
    return (newX >= 0 && newX < SCREEN_WIDTH && newY >= 0 && newY < SCREEN_HEIGHT && game.gameMaze[newY][newX] != '#');
}

int main() {
    gfxInitDefault(); // Initialize graphics
    Game pacmanGame; // Create an instance of the Game
    pacmanGame.run(); // Start the game loop
    return 0; 
}
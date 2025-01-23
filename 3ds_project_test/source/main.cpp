class Game {
public:
    Game() : remainingTime(0), moveCounter(0), gameRunning(false) {
        consoleInit(GFX_TOP, &topConsole);    // Correct order of parameters
        consoleInit(GFX_BOTTOM, &bottomConsole); // Correct order of parameters
        initializeMaze();
    }
    
    // ... Rest of the class ...
};
```

### Full Code Context with Fixes

Here’s the adjusted portion of the `Game` class to reflect the corrections:

```cpp
class Game {
public:
    Game() : remainingTime(0), moveCounter(0), gameRunning(false) {
        consoleInit(GFX_TOP, &topConsole);    // Correct order of parameters
        consoleInit(GFX_BOTTOM, &bottomConsole); // Correct order of parameters
        initializeMaze();
    }

    // ... Other member functions remain unchanged ...
    
private:
    PrintConsole topConsole;    // Console for top screen
    PrintConsole bottomConsole; // Console for bottom screen
    // ... Other member variables remain unchanged ...
}

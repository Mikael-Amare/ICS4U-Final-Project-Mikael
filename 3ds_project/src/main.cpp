#include <3ds.h>
#include <stdio.h>

int main() {
    // Initialize the graphics
    gfxInitDefault();
    consoleInit(GFX_TOP, NULL);
    printf("Hello World!\nPress START to exit.\n");
    // Main loop
    while (aptMainLoop()) {
        hidScanInput(); // Scan input

        u32 kDown = hidKeysDown();
        if (kDown & KEY_START) break; // Exit when START is pressed

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit(); // Exit graphics
    return 0;
}

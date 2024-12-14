#include <3ds.h>
#include <cstdio>

int main()
{
    gfxInitDefault();
    consoleInit(GFX_TOP, nullptr);

    printf("\x1b[16;20HHello World!");
    printf("\x1b[30;16HPress Start to exit.");

    while (aptMainLoop())
    {
        hidScanInput();
        if (hidKeysDown() & KEY_START) break;

        gfxFlushBuffers();
        gfxSwapBuffers();
        gspWaitForVBlank();
    }

    gfxExit();
    return 0;
}

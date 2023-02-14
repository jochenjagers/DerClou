/*
    SDL_main.c, placed in the public domain by Sam Lantinga  4/13/98

    The WinMain function -- calls your program's main() function
*/

#include <ctype.h>
#include <limits.h>
#include <stdio.h>
#include <stdlib.h>

/* Include the SDL main definition header */
#include "SDL.h"
#include "SDL_main.h"
#include "base/base.h"

/* Set a variable to tell if the stdio redirect has been enabled. */
static int stdioRedirectEnabled = 0;

static char stdoutPath[FILENAME_MAX];
static char stderrPath[FILENAME_MAX];

/* SDL_Quit() shouldn't be used with atexit() directly because
   calling conventions may differ... */
static void cleanup(void) { SDL_Quit(); }

/* Remove the output files if there was no output written */
static void cleanup_output(void)
{
    FILE *file = NULL;
    int empty = 0;

    /* Flush the output in case anything is queued */
    fclose(stdout);
    fclose(stderr);

    /* Without redirection we're done */
    if (!stdioRedirectEnabled)
    {
        return;
    }

    /* See if the files have any output in them */
    if (stdoutPath[0])
    {
        file = fopen(stdoutPath, "rb");
        if (file)
        {
            empty = (fgetc(file) == EOF) ? 1 : 0;
            fclose(file);
            if (empty)
            {
                remove(stdoutPath);
            }
        }
    }
    if (stderrPath[0])
    {
        file = fopen(stderrPath, "rb");
        if (file)
        {
            empty = (fgetc(file) == EOF) ? 1 : 0;
            fclose(file);
            if (empty)
            {
                remove(stderrPath);
            }
        }
    }
}

/* This is where execution begins [console apps] */
int main(int argc, char *argv[])
{
    int status = 0;

    /* Load SDL dynamic link library */
    if (SDL_Init(SDL_INIT_NOPARACHUTE) < 0)
    {
        printf("WinMain() error: %s", SDL_GetError());
        return 1;
    }
    atexit(cleanup_output);
    atexit(cleanup);

    /* Run the application main() code */
    status = tcStartGame(argc, argv);

    /* Exit cleanly, calling atexit() functions */
    exit(status);

    /* Hush little compiler, don't you cry... */
    return 0;
}
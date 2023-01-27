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

static char stdoutPath[NAME_MAX];
static char stderrPath[NAME_MAX];

static void UnEscapeQuotes(char *arg)
{
    char *last = NULL;

    while (*arg)
    {
        if (*arg == '"' && *last == '\\')
        {
            char *c_curr = arg;
            char *c_last = last;

            while (*c_curr)
            {
                *c_last = *c_curr;
                c_last = c_curr;
                c_curr++;
            }
            *c_last = '\0';
        }
        last = arg;
        arg++;
    }
}

/* Parse a command line buffer into arguments */
static int ParseCommandLine(char *cmdline, char **argv)
{
    char *bufp;
    char *lastp = NULL;
    int argc, last_argc;

    argc = last_argc = 0;
    for (bufp = cmdline; *bufp;)
    {
        /* Skip leading whitespace */
        while (isspace(*bufp))
        {
            ++bufp;
        }
        /* Skip over argument */
        if (*bufp == '"')
        {
            ++bufp;
            if (*bufp)
            {
                if (argv)
                {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            while (*bufp && (*bufp != '"' || (lastp && *lastp == '\\')))
            {
                lastp = bufp;
                ++bufp;
            }
        }
        else
        {
            if (*bufp)
            {
                if (argv)
                {
                    argv[argc] = bufp;
                }
                ++argc;
            }
            /* Skip over word */
            while (*bufp && !isspace(*bufp))
            {
                ++bufp;
            }
        }
        if (*bufp)
        {
            if (argv)
            {
                *bufp = '\0';
            }
            ++bufp;
        }

        /* Strip out \ from \" sequences */
        if (argv && last_argc != argc)
        {
            UnEscapeQuotes(argv[last_argc]);
        }
        last_argc = argc;
    }
    if (argv)
    {
        argv[argc] = NULL;
    }
    return (argc);
}

/* SDL_Quit() shouldn't be used with atexit() directly because
   calling conventions may differ... */
static void cleanup(void) { SDL_Quit(); }

/* Remove the output files if there was no output written */
static void cleanup_output(void)
{
    FILE *file;
    int empty;

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
    int status;

    /* Load SDL dynamic link library */
    if (SDL_Init(SDL_INIT_NOPARACHUTE) < 0)
    {
        printf("WinMain() error", SDL_GetError());
        return (1);
    }
    atexit(cleanup_output);
    atexit(cleanup);

    /* Run the application main() code */
    status = SDL_main(argc, argv);

    /* Exit cleanly, calling atexit() functions */
    exit(status);

    /* Hush little compiler, don't you cry... */
    return 0;
}
#include <stdio.h>
#include "ogli.h"

#define GLINFO_MAJOR_VERSION    1
#define GLINFO_MINOR_VERSION    1

const char * HELP_MSG = "OpenGL information query utility - v%d.%d (%s)\n"
                        "Coded by Trinh D.D. Nguyen\n\n"
                        "Usage: glinfo [-?|c|e|i]\n"
                        "Where as: -?  give this help message\n"
                        "          -c  use core profile to query, by default legacy profile is used\n"
                        "          -e  list all extensions only\n"
                        "          -i  display OpenGL information, use combine with -e\n"
                        "          -v  display program's version number\n";

void die(const char * msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(-1);
}

void showVersions()
{
    printf("OpenGL Information Query v%d.%d\n"
           "Coded by Trinh D.D. Nguyen\n", GLINFO_MAJOR_VERSION,
                                           GLINFO_MINOR_VERSION);
    printf("Information query library v%d.%d (%s)\n", OGLI_MAJOR_VERSION,
                                                      OGLI_MINOR_VERSION, 
                                                      OGLI_PLATFORM);
#ifdef  OGLI_USE_GLEW
    printf("GLEW library v%s\n", glewGetString(GLEW_VERSION));
#endif
}

int main(int argc, char **argv)
{
    OGLI_PROFILE        profile = OGLI_LEGACY;
    OGLI_CONTEXT        * ctx = NULL;
    GLint               idx;
    GLboolean           extShow = GL_FALSE, infShow = GL_TRUE, valid = GL_FALSE;

    if (argc > 1)
    {
        valid = GL_FALSE;
        for(idx = 0; idx < argc; idx++)
        {
            if (strcmp(argv[idx], "-?") == 0)
            {
                printf(HELP_MSG, GLINFO_MAJOR_VERSION, GLINFO_MINOR_VERSION, OGLI_PLATFORM);
                return 0;
            }

            if (strcmp(argv[idx], "-v") == 0)
            {
                showVersions();
                return 0;
            }

            if (strcmp(argv[idx], "-c") == 0)
            {
                profile = OGLI_CORE;
                valid = GL_TRUE;
            }
            
            if (strcmp(argv[idx], "-e") == 0)
            {
                extShow = GL_TRUE;
                infShow = GL_FALSE;
                valid = GL_TRUE;
            }

            if (strcmp(argv[idx], "-i") == 0)
            {
                infShow = GL_TRUE;
                valid = GL_TRUE;
            }
        }
        if(!valid)   
            die("Unknown command line option.");
    }

    ctx = ogliInit(profile);    
    if (!ctx)
        die("Cannot init OGLI library.");

    if (!ogliCreateContext(ctx))
        die("Error creating OpenGL context.");

    if (!ogliQuery(ctx))
        die("Error fetching OpenGL information.");

    printf(">>> OpenGL\n");
    if (infShow)
    {
        printf(" . Vendor      : %s\n", ctx->iblock.glVendor);
        printf(" . Renderer    : %s\n", ctx->iblock.glRenderer);
        printf(" . Version     : %s\n", ctx->iblock.glVersion);
        printf(" . GLSL        : %s\n", ctx->iblock.glSL);
        printf(" . Extensions  : %u total\n", ctx->iblock.totalExtensions);
    }

    if (extShow)
        printf("%s\n", ctx->iblock.glExtensions);
    
    printf("\n>>> OpenGLU\n");
    if (infShow)
    {
        printf(" . Version     : %s\n", ctx->iblock.gluVersion);
    }

    if (extShow)
        printf("%s\n", ctx->iblock.gluExtensions);


    if (!ogliDestroyContext(ctx))
        die("Error destroying rendering OpenGL context.");
    
    ogliShutdown(ctx);

    return  0;
}

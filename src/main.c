#include <stdio.h>
#include "ogli.h"

const char * HELP_MSG = "OpenGL information query utility - v1.0 (%s)\n"
                        "Coded by Trinh D.D. Nguyen\n\n"
                        "Usage: glinfo [-?|c|e|i]\n"
                        "Where as: -?  give this help message\n"
                        "          -c  use core profile to query, by default legacy profile is used\n"
                        "          -e  list all extensions\n"
                        "          -i  query library version\n";

void die(const char * msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(-1);
}

int main(int argc, char **argv)
{
    OGLI_PROFILE        profile = OGLI_LEGACY;
    GL_INFO_CONTEXT     * ctx = NULL;
    GLint               idx;
    GLboolean           extShow = GL_FALSE;

    if (argc > 1)
    {
        for(idx = 0; idx < argc; idx++)
        {
            if (strcmp(argv[idx], "-?") == 0)
            {
                printf(HELP_MSG, OGLI_PLATFORM);
                return 0;
            }

            if (strcmp(argv[idx], "-i") == 0)
            {
                printf("Information query library v%d.%d (%s)\n", OGLI_MAJOR_VERSION,
                                                                  OGLI_MINOR_VERSION, 
                                                                  OGLI_PLATFORM);
                return 0;
            }

            if (strcmp(argv[idx], "-c") == 0)
                profile = OGLI_CORE;

            if (strcmp(argv[idx], "-e") == 0)
                extShow = GL_TRUE;
        }
    }

    ctx = ogliInit(profile);    
    if (!ctx)
        die("Cannot init OGLI library.");

    if (!ogliCreateContext(ctx))
        die("Error creating OpenGL context.");

    if (!ogliQuery(ctx))
        die("Error fetching OpenGL information.");

    printf("--- OpenGL\n");
    printf("Vendor      : %s\n", ctx->iblock.glVendor);
    printf("Renderer    : %s\n", ctx->iblock.glRenderer);
    printf("Version     : %s\n", ctx->iblock.glVersion);
    printf("GLSL        : %s\n", ctx->iblock.glSL);
    printf("Extensions  : %u total\n", ctx->iblock.totalExtensions);

    if (extShow)
        printf("%s\n", ctx->iblock.glExtensions);
    
    printf("\n--- OpenGLU\n");
    printf("Version     : %s\n", ctx->iblock.gluVersion);

    if (extShow)
        printf("%s\n", ctx->iblock.gluExtensions);


    if (!ogliDestroyContext(ctx))
        die("Error destroying rendering OpenGL context.");
    
    ogliShutdown(ctx);

    return  0;
}

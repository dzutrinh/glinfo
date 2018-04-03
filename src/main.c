#include <stdio.h>
#include "ogli.h"

void die(const char * msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(0);
}

int main(int argc, char **argv)
{
    GL_INFO_CONTEXT * ctx = ogliInit(OGLI_LEGACY);
    
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
    printf("Extenions   : %u total\n", ctx->iblock.totalExtensions);
    
    printf("--- OpenGLU\n");
    printf("Version     : %s\n", ctx->iblock.gluVersion);
    printf("Extensions  : %s\n", ctx->iblock.gluExtensions);

    //printf("Extensions  : %s\n", ctx->iblock.extensions);

    if (!ogliDestroyContext(ctx))
        die("Error destroying rendering OpenGL context.");
    
    ogliShutdown(ctx);

    return  0;
}

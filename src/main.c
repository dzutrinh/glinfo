#include <stdio.h>
#include "ogli.h"

static char * g_SM[GL_SHADE_MODEL] = {  "None",
                                        "SM 1.0",
                                        "SM 2.0", 
                                        "SM 3.0", 
                                        "SM 4.0"};

void error(const char * msg)
{
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(0);
}

int main(int argc, char **argv)
{
    GL_INFO_CONTEXT * ctx = ogliInit(4, 6);
    
    if (!ctx)
        error("Cannot init OGLI library.");

    if (!ogliCreateContext(ctx))
        error("Error creating OpenGL context.");

    if (!ogliQuery(ctx))
        error("Error fetching OpenGL information.");

    printf("Vendor      : %s\n", ctx->iblock.vendor);
    printf("Renderer    : %s\n", ctx->iblock.renderer);
    printf("Version     : %s\n", ctx->iblock.version);
    printf("GLSL        : %s\n", ctx->iblock.glsl);
    printf("Shader Model: %s\n", g_SM[ctx->iblock.sm]);

    if (!ogliDestroyContext(ctx))
        error("Error destroying rendering OpenGL context.");
        
    ogliShutdown(ctx);

    return  0;
}

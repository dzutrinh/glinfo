#include <stdio.h>
#include "glinfo.h"

GLboolean gliGetInfo(GL_INFO_CONTEXT * ctx)
{
  return GL_FALSE; 
}

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutCreateWindow("GLUT");

    glewInit();

    // returns pre 3.0 OpenGL information
    printf("Vendor      : %s\n", glGetString(GL_VENDOR));
    printf("Version     : %s\n", glGetString(GL_VERSION));
    printf("Renderer    : %s\n", glGetString(GL_RENDERER));
    printf("GLSL        : %s\n", glGetString(GL_SHADING_LANGUAGE_VERSION));

    const GLubyte * extensions_string=glGetString(GL_EXTENSIONS);
    //printf("Extensions: %s\n", extensions_string);
    printf("Shader Model: ");
    if (gliSupported( (const char *) extensions_string, "GL_NV_gpu_program4"))
        printf("4.0\n");
    else
    if (gliSupported( (const char *) extensions_string, "GL_NV_vertex_program3"))
        printf("3.0\n");
    else
    if (gliSupported( (const char *) extensions_string, "GL_ARB_fragment_program"))
        printf("2.0\n");
    else
        printf("None\n");

}

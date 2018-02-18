#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif
#include <cstdio>

int main(int argc, char **argv)
{
    glutInit(&argc, argv);
    glutCreateWindow("GLUT");

    glewInit();
    printf("Vendor  : %s\n", glGetString(GL_VENDOR));
    printf("Version : %s\n", glGetString(GL_VERSION));
    printf("Renderer: %s\n", glGetString(GL_RENDERER));
}

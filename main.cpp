#include <stdio.h>
#include <string.h>
#include <GL/glew.h>
#ifdef __APPLE__
#include <GLUT/glut.h>
#else
#include <GL/glut.h>
#endif

bool isExtensionSupported(const char *extList, const char *extension)
{
  const char *start;
  const char *where, *terminator;

  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if (where || *extension == '\0')
    return false;

  /* It takes a bit of care to be fool-proof about parsing the
     OpenGL extensions string. Don't be fooled by sub-strings,
     etc. */
  for (start=extList;;)
  {
    where = strstr(start, extension);
    if (!where)
      break;
    terminator = where + strlen(extension);
    if ( where == start || *(where - 1) == ' ' )
      if ( *terminator == ' ' || *terminator == '\0' )
        return true;
    start = terminator;
  }
  return false;
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
    if (isExtensionSupported( (const char *) extensions_string, "GL_NV_gpu_program4"))
        printf("4.0\n");
    else
    if (isExtensionSupported( (const char *) extensions_string, "GL_NV_vertex_program3"))
        printf("3.0\n");
    else
    if (isExtensionSupported( (const char *) extensions_string, "GL_ARB_fragment_program"))
        printf("2.0\n");
    else
        printf("None\n");

}

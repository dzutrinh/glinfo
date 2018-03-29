#include <string.h>
#include "glinfo.h"

GLboolean gliSupported(const char *extList, const char *extension)
{
  const char *start;
  const char *where, *terminator;

  /* Extension names should not have spaces. */
  where = strchr(extension, ' ');
  if (where || *extension == '\0')
    return GL_FALSE;

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
        return GL_TRUE;
    start = terminator;
  }
  
  return GL_FALSE;
}

GL_INFO_CONTEXT * gliInit(int major, int minor)
{
  GL_INFO_CONTEXT * ctx = (GL_INFO_CONTEXT *) malloc(sizeof(GL_INFO_CONTEXT));
  if (!ctx)
  {
    return NULL;
  }

  ctx->requestMajor = major;
  ctx->requestMinor = minor;
  memset(&ctx->iblock, 0, sizeof(GL_INFO_BLOCK));
  return ctx;
}

GLboolean gliShutdown(GL_INFO_CONTEXT ** ctx)
{
  if (!(*ctx))
  {
    return GL_FALSE;
  }
  
  free((*ctx));
  (*ctx) = NULL;
  return GL_TRUE;
}


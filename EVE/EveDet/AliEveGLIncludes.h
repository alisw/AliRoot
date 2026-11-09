#ifndef AliEveGLIncludes_H
#define AliEveGLIncludes_H

#ifdef __APPLE__
#define GL_SILENCE_DEPRECATION   // Apple has deprecated OpenGL and warns on every use
#include <OpenGL/gl.h>
#include <OpenGL/glu.h>
#else
#include <GL/gl.h>
#include <GL/glu.h>
#endif

#endif // AliEveGLIncludes_H

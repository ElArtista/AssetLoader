#include "shader.h"
#include <stdlib.h>
#include <stdio.h>
#include <plat.h>
#ifdef OS_OSX
#include <OpenGL/glu.h>
#else
#include <GL/glu.h>
#endif

/* --------------------------------------------------
 * Checks and shows last OpenGL error occurred
 * -------------------------------------------------- */
void gl_check_error()
{
    GLenum err = glGetError();
    if (err != GL_NO_ERROR) {
        const GLubyte* err_desc = gluErrorString(err);
        fprintf(stderr, "OpenGL error: %s", err_desc);
    }
}

/* --------------------------------------------------
 * Checks and shows last shader compile error occurred
 * -------------------------------------------------- */
void gl_check_last_compile_error(GLuint id)
{
    /* Check if last compile was successful */
    GLint compileStatus;
    glGetShaderiv(id, GL_COMPILE_STATUS, &compileStatus);
    if (compileStatus == GL_FALSE) {
        /* Gather the compile log size */
        GLint logLength;
        glGetShaderiv(id, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0) {
            /* Fetch and print log */
            GLchar* buf = malloc(logLength * sizeof(GLchar));
            glGetShaderInfoLog(id, logLength, 0, buf);
            fprintf(stderr, "Shader error: %s", buf);
            free(buf);
        }
    }
}

/* --------------------------------------------------
 * Checks and shows last shader link error occurred
 * -------------------------------------------------- */
void gl_check_last_link_error(GLuint id)
{
    /* Check if last link was successful */
    GLint status;
    glGetProgramiv(id, GL_LINK_STATUS, &status);
    if (status == GL_FALSE) {
        /* Gather the link log size */
        GLint logLength;
        glGetProgramiv(id, GL_INFO_LOG_LENGTH, &logLength);
        if (logLength != 0) {
            /* Fetch and print log */
            GLchar* buf = malloc(logLength * sizeof(GLchar));
            glGetProgramInfoLog(id, logLength, 0, buf);
            fprintf(stderr, "Shader program error: %s", buf);
            free(buf);
        }
    }
}

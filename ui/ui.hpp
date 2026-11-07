
#ifndef UICPP
#define UICPP

#include <GL/glew.h>
#include <GLFW/glfw3.h>

// Nuklear configuration
#define NK_INCLUDE_FIXED_TYPES
#define NK_INCLUDE_STANDARD_IO
#define NK_INCLUDE_STANDARD_VARARGS
#define NK_INCLUDE_DEFAULT_ALLOCATOR
#define NK_INCLUDE_VERTEX_BUFFER_OUTPUT
#define NK_INCLUDE_FONT_BAKING
#define NK_INCLUDE_DEFAULT_FONT

// Core Nuklear implementation
#define NK_IMPLEMENTATION
#include "../nuklear/nuklear.h"

// GLFW + OpenGL implementation
#define NK_GLFW_GL3_IMPLEMENTATION
#include "../nuklear/nuklear_glfw_gl3.h"

void RenderUI(struct nk_context* ctx, struct nk_glfw* glfw);

#endif

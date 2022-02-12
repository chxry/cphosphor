#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <log.h>

#include "util.h"

typedef struct {
  unsigned int VBO;
  unsigned int VAO;
  unsigned int EBO;
} mesh_t;

void window_init(char* title, int w, int h, bool fullscreen);
void window_loop();
void window_destroy();
unsigned int create_shader(const char* vert_path, const char* frag_path);
void use_shader(unsigned int shader);
mesh_t create_mesh();
void render_mesh(mesh_t mesh);

#pragma once
#include <stdbool.h>
#include <math.h>
#include <SDL2/SDL.h>
#include <glad/glad.h>
#include <cimgui.h>
#include <cglm/cglm.h>
#include <log.h>
#include <vec.h>
#include <stb_image.h>

#include "game/game.h"
#include "game/player.h"
#include "ui.h"

typedef struct {
  unsigned int VBO;
  unsigned int VAO;
  int verts;
} mesh_t;

void window_init(char* title);
void window_loop();
void window_destroy();
unsigned int shader_init(const char* vert_path, const char* frag_path);
void shader_set_mat4(unsigned int shader, const char* name, mat4 val);
void shader_use(unsigned int shader);
mesh_t mesh_init_pos();
mesh_t mesh_init_pos_tex();
void mesh_render(mesh_t mesh);
unsigned int tex_load(char* path, int mode);
void tex_use(unsigned int tex);
unsigned int tex_load_cubemap(char** faces, int mode);
void tex_use_cubemap(unsigned int tex);

#pragma once
#include <stdbool.h>
#include <SDL2/SDL.h>
#include <glad/gl.h>
#include <cglm/cglm.h>
#include <vec.h>
#include <stb/stb_image.h>
#include <tinyobj/tinyobj_loader_c.h>

#include "game/game.h"
#include "game/player.h"
#include "game/world.h"
#include "ui.h"
#include "log.h"

typedef struct {
  unsigned int VBO;
  unsigned int VAO;
  int verts;
} mesh_t;

typedef enum {
  pos = 3,
  pos_tex = 5,
  pos_tex_norm = 8
} mesh_attr;

extern SDL_Window* window;
extern int frame_delay;
extern mesh_t cube_mesh;
extern unsigned int basic_shader;
extern unsigned int debug_shader;
extern unsigned int shadow_shader;

void window_init(char* title);
void window_loop();
void gbuffer_init(int width, int height);
void gbuffer_resize(int width, int height);
void gbuffer_render();
unsigned int shader_init(const char* vert_path, const char* frag_path);
void shader_set_mat4(unsigned int shader, const char* name, mat4 val);
void shader_set_vec3(unsigned int shader, const char* name, vec3 val);
void shader_set_float(unsigned int shader, const char* name, float val);
void shader_set_int(unsigned int shader, const char* name, int val);
void shader_use(unsigned int shader);
mesh_t mesh_init(float* verts, int len, mesh_attr attr);
mesh_t mesh_load_obj(const char* path, mesh_attr attr);
void mesh_render(mesh_t mesh);
unsigned int tex_load(const char* path, int mode);
void tex_use(unsigned int tex);
unsigned int tex_load_cubemap(char** faces, int mode);
void tex_use_cubemap(unsigned int tex);

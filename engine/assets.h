#pragma once
#include <stdlib.h>
#include <stdbool.h>
#include <string.h>
#include <parson/parson.h>
#include <vec.h>
#include <map.h>
#include <physfs.h>
#include <glad/gl.h>
#include <stb/stb_image.h>
#include <fmod.h>

#include "log.h"

typedef struct {
  vec_void_t assets;
  void* (*load)(JSON_Object* obj);
} asset_t;
typedef map_t(asset_t) map_asset_t;

typedef struct {
  void* data;
  int len;
} filedata_t;

typedef struct {
  char* path;
  unsigned int tex;
} tex_t;

typedef struct {
  int id;
  unsigned int tex;
} cubemap_t;

typedef struct {
  char* path;
  unsigned int VBO;
  unsigned int VAO;
  int verts;
} mesh_t;

typedef enum {
  pos = 3,
  pos_tex = 5,
  pos_tex_norm = 8
} mesh_attr;

typedef struct {
  int id;
  unsigned int program;
} shader_t;

typedef struct {
  char* path;
  FMOD_SOUND* sound;
} sound_t;

#include "core/gfx.h"
#include "core/audio.h"

extern map_asset_t assets;

void assets_init(char* path);
void asset_register(char* name, asset_t asset);
tex_t* get_tex(char* path);
cubemap_t* get_cubemap(int id);
mesh_t* get_mesh(char* path);
shader_t* get_shader(int id);
sound_t* get_sound(char* path);

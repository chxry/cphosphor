#pragma once
#include <parson/parson.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <vec.h>

typedef struct {
  int entity;
  const char* mesh;
  const char* tex;
  // todo only in editor mode
  char mesh_buf[256];
  char tex_buf[256];
} model_t;
typedef vec_t(model_t) vec_model_t;

model_t model_load(JSON_Object* obj);
JSON_Value* model_save(model_t model);
void model_inspector(model_t* model);
model_t model_create(int entity);
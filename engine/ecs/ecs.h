#pragma once
#include <parson/parson.h>
#include <vec.h>

typedef struct {
  char* name;
  void* (*load)(JSON_Object* obj);
  JSON_Value* (*save)(void* c);
  void (*inspector)(void* c);
  void* (*create)(int entity);
  char* inspector_label;
} component_t;
typedef vec_t(component_t) vec_component_t;

extern vec_component_t components;

void ecs_init();
void component_register(component_t component);
void component_iter(void (*f)(component_t c));

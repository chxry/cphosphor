#pragma once
#include <parson/parson.h>
#include <cglm/cglm.h>

#define VEC3_FROM_JSON(v) \
  { json_array_get_number(v, 0), json_array_get_number(v, 1), json_array_get_number(v, 2) }

JSON_Value* json_vec3(vec3 vec);

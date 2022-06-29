#include "json.h"

JSON_Value* json_vec3(vec3 vec) {
  JSON_Value* arrv = json_value_init_array();
  JSON_Array* arr = json_array(arrv);
  json_array_append_number(arr, vec[0]);
  json_array_append_number(arr, vec[1]);
  json_array_append_number(arr, vec[2]);
  return arrv;
}
#include "editor.h"

bool scene_open = true;
bool inspector_open = true;
bool outline_open = true;
bool assets_open = true;
bool environment_open = true;
bool info_open = false;

int selected_entity = -1;
char name_buf[256];

void set_selected_entity(int entity) {
  selected_entity = entity;
  if (entity >= 0) {
    strcpy(name_buf, get_entity(entity)->name);
  }
}
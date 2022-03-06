#include "editor.h"

bool scene = true;
bool inspector = true;
bool outline = true;
bool assets = true;
bool lighting = true;

int selected_entity = -1;
vec3 light_dir = (vec3){-2, 6, -5};
char mesh_mesh_buf[256];
char mesh_tex_buf[256];

void set_selected_entity(int i) {
  selected_entity = i;
  if (i >= 0) {
    entity_t entity = world.entities.data[selected_entity];
    if (entity.model) {
      strcpy(mesh_mesh_buf, entity.model->mesh);
      strcpy(mesh_tex_buf, world.entities.data[selected_entity].model->tex);
    }
  }
}
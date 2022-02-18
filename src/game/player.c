#include "player.h"

vec3 player_pos = (vec3){0.0, 3.0, 3.0};
float yvel = 0.0;
float yaw = -90.0;
float pitch = 0.0;
bool ground = false;
const float HEIGHT = 1.6;
const float RADIUS = 0.4;

void player_processevent(SDL_Event* e) {
  if (e->type == SDL_MOUSEMOTION && SDL_GetRelativeMouseMode()) {
    yaw += e->motion.xrel * conf.sens;
    pitch -= e->motion.yrel * conf.sens;
  }
  if (pitch > 89.0) {
    pitch = 89.0;
  }
  if (pitch < -89.0) {
    pitch = -89.0;
  }
}

bool test_collision(vec3 new_pos) {
  vec3 player_box[2] = {{new_pos[0] - RADIUS, new_pos[1], new_pos[2] - RADIUS}, {new_pos[0] + RADIUS, new_pos[1] + HEIGHT, new_pos[2] + RADIUS}};
  bool collides = false;
  int i;
  collider_t collider;
  vec_foreach(&colliders, collider, i) {
    if ((collides = glm_aabb_aabb(player_box, (vec3*)&collider))) {
      break;
    }
  }
  if (collides) {
    glm_vec3_copy(player_pos, new_pos);
  } else {
    glm_vec3_copy(new_pos, player_pos);
  }
  return collides;
}

void player_movement(mat4* view) {
  vec3 front;
  front[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
  front[1] = sin(glm_rad(pitch));
  front[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
  glm_vec3_normalize(front);

  const unsigned char* keys = SDL_GetKeyboardState(NULL);
  vec3 new_pos;
  glm_vec3_copy(player_pos, new_pos);
  if (SDL_GetRelativeMouseMode()) {
    if (keys[conf.binds[KEYBIND_FORWARD]]) {
      new_pos[0] += state.player_speed * cos(glm_rad(yaw));
      new_pos[2] += state.player_speed * sin(glm_rad(yaw));
      test_collision(new_pos);
    }
    if (keys[conf.binds[KEYBIND_BACK]]) {
      new_pos[0] -= state.player_speed * cos(glm_rad(yaw));
      new_pos[2] -= state.player_speed * sin(glm_rad(yaw));
      test_collision(new_pos);
    }
    if (keys[conf.binds[KEYBIND_LEFT]]) {
      new_pos[0] -= state.player_speed * cos(glm_rad(90 + yaw));
      new_pos[2] -= state.player_speed * sin(glm_rad(90 + yaw));
      test_collision(new_pos);
    }
    if (keys[conf.binds[KEYBIND_RIGHT]]) {
      new_pos[0] += state.player_speed * cos(glm_rad(90 + yaw));
      new_pos[2] += state.player_speed * sin(glm_rad(90 + yaw));
      test_collision(new_pos);
    }
    if (keys[conf.binds[KEYBIND_JUMP]] && ground) {
      yvel = state.player_jumpheight;
      ground = false;
    }
  }
  yvel -= state.world_gravity;
  new_pos[1] += yvel;
  if (test_collision(new_pos)) {
    ground = yvel < 0;
    yvel = 0.0;
  }

  vec3 cam_pos;
  glm_vec3_add(player_pos, (vec3){0.0, HEIGHT, 0.0}, cam_pos);
  glm_look(cam_pos, front, GLM_YUP, *view);
}
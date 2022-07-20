#include "player.h"

vec3 player_pos = (vec3){10.0, 2.0, 10.0};
vec3 vel = (vec3){0.0, 0.0, 0.0};
vec3 cam_pos;
vec3 cam_dir;
float yaw = -90.0;
float pitch = 0.0;
bool ground = false;
float height = 1.6;
const float RADIUS = 0.4;

void player_processevent(SDL_Event* e) {
  if (!SDL_GetRelativeMouseMode()) {
    return;
  }
  switch (e->type) {
  case SDL_MOUSEMOTION:
    yaw += e->motion.xrel * conf.sens;
    pitch -= e->motion.yrel * conf.sens;
    break;
  case SDL_MOUSEBUTTONDOWN:
    if (e->button.button == SDL_BUTTON_LEFT) {
      collision_t collision = world_raycast(cam_pos, cam_dir);
      if (collision.distance == 0) {
        break;
      }
      log_info("%s %i - %f %f %f", collision.entity->name, collision.entity->id, collision.hit[0], collision.hit[1], collision.hit[2]);
    }
    break;
  }
  if (pitch > 89.0) {
    pitch = 89.0;
  }
  if (pitch < -89.0) {
    pitch = -89.0;
  }
}

bool test_collision(vec3 new_pos) {
  aabb_t player_box = {{new_pos[0] - RADIUS, new_pos[1], new_pos[2] - RADIUS}, {new_pos[0] + RADIUS, new_pos[1] + height, new_pos[2] + RADIUS}};
  if (world_test_collision(player_box)) {
    glm_vec3_copy(player_pos, new_pos);
    return true;
  } else {
    glm_vec3_copy(new_pos, player_pos);
    return false;
  }
}

void player_movement(vec3 cam_pos, vec3 cam_dir) {
  cam_dir[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
  cam_dir[1] = sin(glm_rad(pitch));
  cam_dir[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
  glm_vec3_normalize(cam_dir);

  const unsigned char* keys = SDL_GetKeyboardState(NULL);
  float speed = state.player_speed;
  if (SDL_GetRelativeMouseMode()) {
    if (keys[conf.binds[KEYBIND_SPRINT]]) {
      speed *= 1.5;
    }
    if (keys[conf.binds[KEYBIND_FORWARD]]) {
      vel[0] += speed * cos(glm_rad(yaw));
      vel[2] += speed * sin(glm_rad(yaw));
    }
    if (keys[conf.binds[KEYBIND_BACK]]) {
      vel[0] -= speed * cos(glm_rad(yaw));
      vel[2] -= speed * sin(glm_rad(yaw));
    }
    if (keys[conf.binds[KEYBIND_LEFT]]) {
      vel[0] -= speed * cos(glm_rad(90 + yaw));
      vel[2] -= speed * sin(glm_rad(90 + yaw));
    }
    if (keys[conf.binds[KEYBIND_RIGHT]]) {
      vel[0] += speed * cos(glm_rad(90 + yaw));
      vel[2] += speed * sin(glm_rad(90 + yaw));
    }
    if (keys[conf.binds[KEYBIND_JUMP]] && ground) {
      vel[1] = state.player_jumpheight;
      ground = false;
    }
    if (keys[conf.binds[KEYBIND_CROUCH]]) {
      height = glm_lerp(height, 0.8, 0.25);
    } else {
      height = glm_lerp(height, 1.6, 0.25);
    }
  }
  vel[1] -= state.world_gravity;
  vec3 new_pos;
  glm_vec3_copy(player_pos, new_pos);
  new_pos[0] += vel[0];
  test_collision(new_pos);

  new_pos[1] += vel[1];
  if (test_collision(new_pos)) {
    if (!ground && (ground = vel[1] < 0)) {
      audio_play("audio/fall.wav");
    }
    vel[1] = 0.0;
  }

  new_pos[2] += vel[2];
  test_collision(new_pos);
  vel[0] *= 0.9;
  vel[2] *= 0.9;

  glm_vec3_add(player_pos, (vec3){0.0, height, 0.0}, cam_pos);
}

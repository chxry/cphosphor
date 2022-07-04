#include "scene.h"

bool scene_focused = false;
ImVec2 scene_size = (ImVec2){0, 0};
bool first_frame = true;

unsigned int scene_fbo;
unsigned int scene_tex;

vec3 cam_pos = (vec3){0.0, 3.0, 0.0};
const float cam_speed = 0.25;
float yaw = -90.0;
float pitch = 0.0;

void scene_init() {
  glGenFramebuffers(1, &scene_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo);
  glGenTextures(1, &scene_tex);
  glBindTexture(GL_TEXTURE_2D, scene_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scene_size.x, scene_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene_tex, 0);
}

void scene_processevent(SDL_Event* e) {
  if (e->type == SDL_MOUSEMOTION && scene_focused && SDL_GetMouseState(0, 0) & SDL_BUTTON_LMASK) {
    yaw += e->motion.xrel * 0.1;
    pitch -= e->motion.yrel * 0.1;
  }
  if (pitch > 89.0) {
    pitch = 89.0;
  }
  if (pitch < -89.0) {
    pitch = -89.0;
  }
}

void scene_update() {
  int frame_start = SDL_GetTicks();
  vec3 cam_dir;
  cam_dir[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
  cam_dir[1] = sin(glm_rad(pitch));
  cam_dir[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
  glm_vec3_normalize(cam_dir);
  if (scene_focused) {
    const unsigned char* keys = SDL_GetKeyboardState(NULL);
    vec3 dir;
    if (keys[SDL_SCANCODE_W]) {
      glm_vec3_scale(cam_dir, cam_speed, dir);
      glm_vec3_add(cam_pos, dir, cam_pos);
    }
    if (keys[SDL_SCANCODE_S]) {
      glm_vec3_scale(cam_dir, cam_speed, dir);
      glm_vec3_sub(cam_pos, dir, cam_pos);
    }
    if (keys[SDL_SCANCODE_A]) {
      glm_vec3_crossn(cam_dir, GLM_YUP, dir);
      glm_vec3_scale(dir, cam_speed, dir);
      glm_vec3_sub(cam_pos, dir, cam_pos);
    }
    if (keys[SDL_SCANCODE_D]) {
      glm_vec3_crossn(cam_dir, GLM_YUP, dir);
      glm_vec3_scale(dir, cam_speed, dir);
      glm_vec3_add(cam_pos, dir, cam_pos);
    }
  }

  mat4 light_view, light_projection;
  gbuffer_render_shadows(light_view, light_projection);
  world_render_shadows(light_view, light_projection);

  glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
  gbuffer_resize(scene_size.x, scene_size.y);
  glBindTexture(GL_TEXTURE_2D, scene_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scene_size.x, scene_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glViewport(0, 0, scene_size.x, scene_size.y);
  glClearColor(0.0, 0.0, 0.0, 1.0);
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
  mat4 view, projection;
  glm_look(cam_pos, cam_dir, GLM_YUP, view);
  glm_perspective(glm_rad(90), scene_size.x / scene_size.y, 0.1, 100.0, projection);
  world_render(view, projection);
  if (selected_entity >= 0) {
    world_render_collider(view, projection, selected_entity);
  }

  glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo);
  gbuffer_render(light_view, light_projection);
  glBindFramebuffer(GL_FRAMEBUFFER, 0);
  if (first_frame) {
    log_info("First render took %ims.", SDL_GetTicks() - frame_start);
    first_frame = false;
  }
}

void scene_render() {
  if (scene) {
    igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){0, 0});
    if (igBegin(SCENE_TITLE, &scene, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
      igGetWindowSize(&scene_size);
      scene_focused = igIsWindowFocused(ImGuiFocusedFlags_None);
      igImage((void*)(intptr_t)scene_tex, scene_size, (ImVec2){0, 1}, (ImVec2){1, 0}, (ImVec4){1, 1, 1, 1}, (ImVec4){0, 0, 0, 0});
    }
    igEnd();
    igPopStyleVar(1);
  }
}
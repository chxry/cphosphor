#include <SDL2/SDL.h>
#define CIMGUI_DEFINE_ENUMS_AND_STRUCTS
#include <cimgui.h>
#include <cglm/cglm.h>

#include "engine/core.h"
#include "engine/gfx.h"
#include "engine/lighting.h"
#include "engine/world.h"
#include "engine/log.h"

bool scene = true;
bool scene_focused = false;
ImVec2 scene_size = (ImVec2){960, 540};

unsigned int scene_fbo;
unsigned int scene_tex;

vec3 light_dir = (vec3){-2, 6, -5};
vec3 cam_pos = (vec3){0.0, 3.0, 0.0};
const float cam_speed = 0.25;
float yaw = -90.0;
float pitch = 0.0;

int main() {
  log_init();
  assets_init("res.tar");
  window_init(1280, 720, false, "editor");
  world_init();

  unsigned int skybox_tex = tex_load_cubemap((char* [6]){"tex/sky/right.jpg", "tex/sky/left.jpg", "tex/sky/top.jpg", "tex/sky/bottom.jpg", "tex/sky/front.jpg", "tex/sky/back.jpg"}, GL_RGB);

  igCreateContext(NULL);
  ImGuiIO* io = igGetIO();
  io->IniFilename = NULL;
  io->ConfigWindowsMoveFromTitleBarOnly = true;
  ImGuiStyle* style = igGetStyle();
  style->WindowRounding = 4.0;
  style->FrameRounding = 4.0;
  style->PopupRounding = 4.0;
  asset_t font = asset_load("roboto.ttf");
  ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, font.data, font.len, 16, NULL, NULL);
  ImGui_ImplSDL2_InitForOpenGL(window, ctx);
  ImGui_ImplOpenGL3_Init("#version 460");

  glGenFramebuffers(1, &scene_fbo);
  glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo);
  glGenTextures(1, &scene_tex);
  glBindTexture(GL_TEXTURE_2D, scene_tex);
  glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, scene_size.x, scene_size.y, 0, GL_RGB, GL_UNSIGNED_BYTE, NULL);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, scene_tex, 0);

  bool quit = false;
  while (!quit) {
    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_MOUSEMOTION:
        if (scene_focused && SDL_GetMouseState(0, 0) & SDL_BUTTON_LMASK) {
          yaw += e.motion.xrel * 0.1;
          pitch -= e.motion.yrel * 0.1;
        }
        break;
      case SDL_QUIT:
        quit = true;
        break;
      }
      ImGui_ImplSDL2_ProcessEvent(&e);
    }
    if (pitch > 89.0) {
      pitch = 89.0;
    }
    if (pitch < -89.0) {
      pitch = -89.0;
    }
    vec3 cam_dir;
    cam_dir[0] = cos(glm_rad(yaw)) * cos(glm_rad(pitch));
    cam_dir[1] = sin(glm_rad(pitch));
    cam_dir[2] = sin(glm_rad(yaw)) * cos(glm_rad(pitch));
    glm_vec3_normalize(cam_dir);
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

    mat4 light_view, light_projection;
    gbuffer_render_shadows(light_view, light_projection, light_dir);
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
    skybox_render(view, projection, skybox_tex);

    glBindFramebuffer(GL_FRAMEBUFFER, scene_fbo);
    gbuffer_render(light_view, light_projection, light_dir);
    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    glViewport(0, 0, 1280, 720);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    igNewFrame();

    if (scene) {
      igPushStyleVar_Vec2(ImGuiStyleVar_WindowPadding, (ImVec2){0, 0});
      igSetNextWindowContentSize(scene_size);
      if (igBegin("Scene", &scene, ImGuiWindowFlags_NoScrollbar | ImGuiWindowFlags_NoScrollWithMouse)) {
        igGetWindowSize(&scene_size);
        scene_focused = igIsWindowFocused(ImGuiFocusedFlags_None);
        igImage((void*)(intptr_t)scene_tex, scene_size, (ImVec2){0, 1}, (ImVec2){1, 0}, (ImVec4){1, 1, 1, 1}, (ImVec4){0, 0, 0, 0});
      }
      igEnd();
      igPopStyleVar(1);
    }

    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(window);
  }

  return 0;
}

#include <SDL2/SDL.h>
#include <cglm/cglm.h>

#include "engine/core.h"
#include "engine/gfx.h"
#include "engine/audio.h"
#include "engine/world.h"
#include "engine/log.h"
#include "game.h"
#include "ui.h"
#include "player.h"

int main() {
  log_init();
  conf_load("conf.json");
  assets_init("res.tar");
  lua_init();
  state_bind();
  window_init(conf.width, conf.height, conf.fullscreen, "flop");
  ui_init();
  audio_init(conf.volume);
  world_load("test.json");

  unsigned int skybox_tex = tex_load_cubemap((char* [6]){"tex/sky/right.jpg", "tex/sky/left.jpg", "tex/sky/top.jpg", "tex/sky/bottom.jpg", "tex/sky/front.jpg", "tex/sky/back.jpg"}, GL_RGB);

  bool quit = false;
  frame_delay = 1000 / conf.fps;
  while (!quit) {
    int frame_start = SDL_GetTicks();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_WINDOWEVENT:
        if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          gbuffer_resize(e.window.data1, e.window.data2);
          conf.width = e.window.data1;
          conf.height = e.window.data2;
        }
        break;
      case SDL_QUIT:
        quit = true;
        break;
      }
      ui_processevent(&e);
      player_processevent(&e);
    }

    mat4 light_view, light_projection;
    gbuffer_render_shadows(light_view, light_projection, state.world_lightdir);
    world_render_shadows(light_view, light_projection);

    mat4 view, projection;
    player_movement(&view);
    glm_perspective(glm_rad(conf.fov), (float)conf.width / (float)conf.height, 0.1, 100.0, projection);
    glViewport(0, 0, conf.width, conf.height);
    glClearColor(0.0, 0.0, 0.0, 1.0);
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    world_render(view, projection);
    if (state.debug_drawcolliders) {
      world_render_colliders(view, projection);
    }
    skybox_render(view, projection, skybox_tex);

    glBindFramebuffer(GL_FRAMEBUFFER, 0);
    gbuffer_render(light_view, light_projection, state.world_lightdir);
    ui_render();
    SDL_GL_SwapWindow(window);
    int frame_time = SDL_GetTicks() - frame_start;
    if (frame_delay > frame_time) {
      SDL_Delay(frame_delay - frame_time);
    }
  }

  return 0;
}

#include <SDL2/SDL.h>
#include <cglm/cglm.h>

#include "engine/core/gfx.h"
#include "engine/core/audio.h"
#include "engine/core/physics.h"
#include "engine/ecs/world.h"
#include "engine/util/ui.h"
#include "engine/engine.h"
#include "engine/assets.h"
#include "engine/log.h"
#include "game.h"
#include "ui.h"
#include "player.h"

int main() {
  log_init();
  engine_init();
  conf_load("conf.json");
  lua_init();
  state_bind();
  window_init(conf.width, conf.height, conf.fullscreen, ENGINE_NAME " " ENGINE_VER);
  ui_init();
  splash_render(conf.width, conf.height);
  audio_init(conf.volume);
  assets_init("res.zip");
  shaders_init();
  world_load("res/scene.json");
  physics_init();
  bool quit = false;
  frame_delay = 1000 / conf.fps;
  while (!quit) {
    int frame_start = SDL_GetTicks();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_WINDOWEVENT:
        if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          renderer_resize(e.window.data1, e.window.data2);
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

    mat4 view, projection;
    player_movement(&view);
    glm_perspective(glm_rad(conf.fov), (float)conf.width / (float)conf.height, 0.1, 100.0, projection);
    physics_update();

    renderer_render(0, cam_dir, view, projection, conf.width, conf.height, -state.debug_drawcolliders);
    ui_render();
    SDL_GL_SwapWindow(window);
    int frame_time = SDL_GetTicks() - frame_start;
    if (frame_delay > frame_time) {
      SDL_Delay(frame_delay - frame_time);
    }
  }

  return 0;
}

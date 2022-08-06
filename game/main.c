#include <SDL2/SDL.h>
#include <cglm/cglm.h>
#include <stdbool.h>

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
  audio_init(conf.volume);
  splash_render("Loading", conf.width, conf.height);
  assets_init("res.zip");
  shaders_init();
  world_load("res/scene.json");
  physics_init();
  while (SDL_GetTicks() < 1500) {
    splash_render("Loading Finished.", conf.width, conf.height);
  }
  bool quit = false;
  frame_delay = 1000 / conf.fps;

  int i;
  audiosrc_t* audiosrc;
  vec_foreach(&get_component("audiosrc")->components, audiosrc, i) {
    if (audiosrc->autoplay) {
      entity_t* entity = get_entity(audiosrc->entity);
      audio_play(get_sound(audiosrc->sound)->sound, entity->pos);
    }
  }

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

    vec3 cam_pos, cam_dir;
    player_movement(cam_pos, cam_dir);
    physics_update();
    audio_update(cam_pos, cam_dir);

    renderer_render(0, cam_pos, cam_dir, conf.fov, conf.width, conf.height, -state.debug_drawcolliders, false);
    ui_render();
    SDL_GL_SwapWindow(window);
    int frame_time = SDL_GetTicks() - frame_start;
    if (frame_delay > frame_time) {
      SDL_Delay(frame_delay - frame_time);
    }
  }

  return 0;
}

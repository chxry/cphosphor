#include "gfx.h"

SDL_Window* window;
SDL_GLContext ctx;

void window_init(char* title, int w, int h) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    log_fatal("Failed to init SDL.");
    exit(-1);
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);

  window = SDL_CreateWindow(title, 0, 0, w, h, SDL_WINDOW_SHOWN | SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE);
  ctx = SDL_GL_CreateContext(window);
  gladLoadGL();

  log_info("Created window \"%s\".", title);
  log_debug("Renderer: %s", glGetString(GL_RENDERER));
  log_debug("Version: %s", glGetString(GL_VERSION));
}

void window_loop() {
  bool quit = false;
  while (!quit) {
    SDL_Event e;

    while (SDL_PollEvent(&e) != 0) {
      if (e.type == SDL_QUIT)
        quit = true;
    }

    glClearColor(0.1, 0.1, 0.1, 1.0);
    glClear(GL_COLOR_BUFFER_BIT);
    SDL_GL_SwapWindow(window);
  }
}

void window_destroy() {
  SDL_GL_DeleteContext(ctx);
  SDL_DestroyWindow(window);
  SDL_Quit();
}
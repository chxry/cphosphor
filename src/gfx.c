#include "gfx.h"

SDL_Window* window;
SDL_GLContext ctx;

float vertices[] = {
    -1.0, -1.0, 0.5, 1.0, 0.0, 0.0,
    1.0, -1.0, 0.5, 0.0, 1.0, 0.0,
    -1.0, 1.0, 0.5, 0.0, 0.0, 1.0,
    1.0, 1.0, 0.5, 1.0, 0.0, 0.0,
    -1.0, -1.0, -0.5, 0.0, 1.0, 0.0,
    1.0, -1.0, -0.5, 0.0, 0.0, 1.0,
    -1.0, 1.0, -0.5, 1.0, 0.0, 0.0,
    1.0, 1.0, -0.5, 0.0, 1.0, 0.0};
unsigned int indices[] = {2, 6, 7, 2, 3, 7, 0, 4, 5, 0, 1, 5, 0, 2, 6, 0, 4, 6, 1, 3, 7, 1, 5, 7, 0, 2, 3, 0, 1, 3, 4, 6, 7, 4, 5, 7};
const FPS = 60;

void window_init(char* title, int w, int h, bool fullscreen) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    log_fatal("Failed to init SDL.");
    exit(-1);
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);

  window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, w, h, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
  ctx = SDL_GL_CreateContext(window);
  if (!gladLoadGL()) {
    log_fatal("Failed to load opengl.");
    exit(-1);
  }
  glEnable(GL_DEPTH_TEST);

  igCreateContext(NULL);
  ImGuiIO io = *igGetIO();
  io.IniFilename = NULL;
  ImFontAtlas_AddFontFromFileTTF(io.Fonts, "./res/roboto.ttf", 16, NULL, NULL);
  ImGui_ImplSDL2_InitForOpenGL(window, &ctx);
  ImGui_ImplOpenGL3_Init("#version 460");

  log_info("Created window \"%s\".", title);
  log_info("Renderer: %s - %s", glGetString(GL_RENDERER), glGetString(GL_VERSION));
}

void window_loop() {
  unsigned int shader = shader_init("./res/shader.vert", "./res/shader.frag");
  mesh_t mesh = mesh_init();

  bool quit = false;
  bool debug_overlay = true;
  int frame_delay = 1000 / FPS;

  while (!quit) {
    int frame_start = SDL_GetTicks();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      ImGui_ImplSDL2_ProcessEvent(&e);
      switch (e.type) {
      case SDL_KEYDOWN:
        if (e.key.keysym.sym == SDLK_BACKQUOTE) {
          debug_overlay = !debug_overlay;
        }
        break;
      case SDL_WINDOWEVENT:
        if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          glViewport(0, 0, e.window.data1, e.window.data2);
        }
        break;
      case SDL_QUIT:
        quit = true;
        break;
      }
    }

    glClearColor(0.0, 0.0, 0.0, 1.0);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
    ImGui_ImplOpenGL3_NewFrame();
    ImGui_ImplSDL2_NewFrame(window);
    igNewFrame();

    if (debug_overlay) {
      igSetNextWindowPos((ImVec2){16.0, 16.0}, ImGuiCond_Always, (ImVec2){0.0, 0.0});
      igSetNextWindowBgAlpha(0.5);
      if (igBegin("Debug overlay", &debug_overlay, ImGuiWindowFlags_NoDecoration | ImGuiWindowFlags_NoFocusOnAppearing | ImGuiWindowFlags_AlwaysAutoResize)) {
        ImGuiIO io = *igGetIO();
        igText("%.1fms", io.DeltaTime * 1000);
        igText("%.1ffps", io.Framerate);
      }
      igEnd();
    }

    mat4 model = GLM_MAT4_IDENTITY;
    glm_rotate_x(model, SDL_GetTicks() / 450.0, model);
    glm_rotate_y(model, SDL_GetTicks() / 450.0, model);
    shader_set_mat4(shader, "model", model);

    mat4 view = GLM_MAT4_IDENTITY;
    glm_translate_z(view, -5.0);
    shader_set_mat4(shader, "view", view);

    mat4 projection = GLM_MAT4_IDENTITY;
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    glm_perspective(80.0f, w / h, 0.1f, 100.0f, projection);
    shader_set_mat4(shader, "projection", projection);

    shader_use(shader);
    mesh_render(mesh);

    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(window);
    int frame_time = SDL_GetTicks() - frame_start;
    if (frame_delay > frame_time) {
      SDL_Delay(frame_delay - frame_time);
    }
  }
}

void window_destroy() {
  SDL_GL_DeleteContext(ctx);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

unsigned int shader_init(const char* vert_path, const char* frag_path) {
  int success;
  const char* vert_src = read_file(vert_path);
  unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, &vert_src, NULL);
  glCompileShader(vert);
  glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
  if (!success) {
    log_error("Failed to compile vertex shader \"%s\".", vert);
  }

  const char* frag_src = read_file(frag_path);
  unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag, 1, &frag_src, NULL);
  glCompileShader(frag);
  glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
  if (!success) {
    log_error("Failed to compile fragment shader \"%s\".", vert);
  }

  unsigned int program = glCreateProgram();
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    log_error("Failed to link \"%s\" and \"%s\".", vert, frag);
  }
  glDeleteShader(vert);
  glDeleteShader(frag);
  return program;
}

void shader_set_mat4(unsigned int shader, const char* name, mat4 val) {
  glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, (float*)val);
}

void shader_use(unsigned int shader) {
  glUseProgram(shader);
}

mesh_t mesh_init() {
  mesh_t mesh;
  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glGenBuffers(1, &mesh.EBO);
  glBindVertexArray(mesh.VAO);

  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

  glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, mesh.EBO);
  glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

  // position
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
  glEnableVertexAttribArray(0);
  // color
  glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3 * sizeof(float)));
  glEnableVertexAttribArray(1);

  return mesh;
}

void mesh_render(mesh_t mesh) {
  glBindVertexArray(mesh.VAO);
  glDrawElements(GL_TRIANGLES, 36, GL_UNSIGNED_INT, 0);
}

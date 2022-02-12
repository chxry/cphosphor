#include "gfx.h"

SDL_Window* window;
SDL_GLContext ctx;

float vertices[] = {
    0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,
    -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,
    0.0f, 0.5f, 0.0f, 0.0f, 0.0f, 1.0f};
unsigned int indices[] = {0, 1, 2};

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
  unsigned int shader = create_shader("./res/shader.vert", "./res/shader.frag");
  mesh_t mesh = create_mesh();

  bool quit = false;
  bool debug_overlay = true;

  while (!quit) {
    SDL_Event e;
    while (SDL_PollEvent(&e) != 0) {
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
    glClear(GL_COLOR_BUFFER_BIT);
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

    use_shader(shader);
    render_mesh(mesh);

    igRender();
    ImGui_ImplOpenGL3_RenderDrawData(igGetDrawData());
    SDL_GL_SwapWindow(window);
  }
}

void window_destroy() {
  SDL_GL_DeleteContext(ctx);
  SDL_DestroyWindow(window);
  SDL_Quit();
}

unsigned int create_shader(const char* vert_path, const char* frag_path) {
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

void use_shader(unsigned int shader) {
  glUseProgram(shader);
}

mesh_t create_mesh() {
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

void render_mesh(mesh_t mesh) {
  glBindVertexArray(mesh.VAO);
  glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
}

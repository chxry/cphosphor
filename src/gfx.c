#include "gfx.h"

SDL_Window* window;
SDL_GLContext ctx;

float cubeVertices[] = {
    -0.5, -0.5, -0.5, 0.0, 0.0,
    0.5, -0.5, -0.5, 1.0, 0.0,
    0.5, 0.5, -0.5, 1.0, 1.0,
    0.5, 0.5, -0.5, 1.0, 1.0,
    -0.5, 0.5, -0.5, 0.0, 1.0,
    -0.5, -0.5, -0.5, 0.0, 0.0,

    -0.5, -0.5, 0.5, 0.0, 0.0,
    0.5, -0.5, 0.5, 1.0, 0.0,
    0.5, 0.5, 0.5, 1.0, 1.0,
    0.5, 0.5, 0.5, 1.0, 1.0,
    -0.5, 0.5, 0.5, 0.0, 1.0,
    -0.5, -0.5, 0.5, 0.0, 0.0,

    -0.5, 0.5, 0.5, 1.0, 0.0,
    -0.5, 0.5, -0.5, 1.0, 1.0,
    -0.5, -0.5, -0.5, 0.0, 1.0,
    -0.5, -0.5, -0.5, 0.0, 1.0,
    -0.5, -0.5, 0.5, 0.0, 0.0,
    -0.5, 0.5, 0.5, 1.0, 0.0,

    0.5, 0.5, 0.5, 1.0, 0.0,
    0.5, 0.5, -0.5, 1.0, 1.0,
    0.5, -0.5, -0.5, 0.0, 1.0,
    0.5, -0.5, -0.5, 0.0, 1.0,
    0.5, -0.5, 0.5, 0.0, 0.0,
    0.5, 0.5, 0.5, 1.0, 0.0,

    -0.5, -0.5, -0.5, 0.0, 1.0,
    0.5, -0.5, -0.5, 1.0, 1.0,
    0.5, -0.5, 0.5, 1.0, 0.0,
    0.5, -0.5, 0.5, 1.0, 0.0,
    -0.5, -0.5, 0.5, 0.0, 0.0,
    -0.5, -0.5, -0.5, 0.0, 1.0,

    -0.5, 0.5, -0.5, 0.0, 1.0,
    0.5, 0.5, -0.5, 1.0, 1.0,
    0.5, 0.5, 0.5, 1.0, 0.0,
    0.5, 0.5, 0.5, 1.0, 0.0,
    -0.5, 0.5, 0.5, 0.0, 0.0,
    -0.5, 0.5, -0.5, 0.0, 1.0};
float skyboxVertices[] = {
    -1.0, 1.0, -1.0,
    -1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
    1.0, 1.0, -1.0,
    -1.0, 1.0, -1.0,

    -1.0, -1.0, 1.0,
    -1.0, -1.0, -1.0,
    -1.0, 1.0, -1.0,
    -1.0, 1.0, -1.0,
    -1.0, 1.0, 1.0,
    -1.0, -1.0, 1.0,

    1.0, -1.0, -1.0,
    1.0, -1.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, -1.0,
    1.0, -1.0, -1.0,

    -1.0, -1.0, 1.0,
    -1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,
    1.0, -1.0, 1.0,
    -1.0, -1.0, 1.0,

    -1.0, 1.0, -1.0,
    1.0, 1.0, -1.0,
    1.0, 1.0, 1.0,
    1.0, 1.0, 1.0,
    -1.0, 1.0, 1.0,
    -1.0, 1.0, -1.0,

    -1.0, -1.0, -1.0,
    -1.0, -1.0, 1.0,
    1.0, -1.0, -1.0,
    1.0, -1.0, -1.0,
    -1.0, -1.0, 1.0,
    1.0, -1.0, 1.0};

const int FPS = 0; // unlimited

void window_init(char* title) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    log_fatal("Failed to init SDL.");
    exit(-1);
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLEBUFFERS, 1);
  SDL_GL_SetAttribute(SDL_GL_MULTISAMPLESAMPLES, conf.msaa);

  window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, conf.width, conf.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | (conf.fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
  ctx = SDL_GL_CreateContext(window);
  SDL_SetRelativeMouseMode(true);
  SDL_CaptureMouse(true);

  if (!gladLoadGL()) {
    log_fatal("Failed to load opengl.");
    exit(-1);
  }
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

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
  mesh_t mesh = mesh_init_pos_tex();
  unsigned int shader = shader_init("./res/shader.vert", "./res/shader.frag");
  unsigned int tex = tex_load("./res/cat.png", GL_RGBA);
  mesh_t skybox_mesh = mesh_init_pos();
  unsigned int skybox_shader = shader_init("./res/skybox.vert", "./res/skybox.frag");
  unsigned int skybox_tex = tex_load_cubemap((char* [6]){"./res/skybox/right.jpg", "./res/skybox/left.jpg", "./res/skybox/top.jpg", "./res/skybox/bottom.jpg", "./res/skybox/front.jpg", "./res/skybox/back.jpg"}, GL_RGB);

  bool quit = false;
  bool debug_overlay = true;
  int frame_delay = 1000 / FPS;

  while (!quit) {
    int frame_start = SDL_GetTicks();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      ImGui_ImplSDL2_ProcessEvent(&e);
      player_processevent(&e);
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

    mat4 view;
    player_movement(&view);
    mat4 projection = GLM_MAT4_IDENTITY;
    int w, h;
    SDL_GetWindowSize(window, &w, &h);
    glm_perspective(glm_rad(80.0), w / h, 0.1, 100.0, projection);

    // cube
    shader_use(shader);
    tex_use(tex);
    mat4 model = GLM_MAT4_IDENTITY;
    glm_rotate_x(model, SDL_GetTicks() / 450.0, model);
    glm_rotate_y(model, SDL_GetTicks() / 450.0, model);
    shader_set_mat4(shader, "model", model);
    shader_set_mat4(shader, "view", view);
    shader_set_mat4(shader, "projection", projection);
    mesh_render(mesh);

    // skybox
    shader_use(skybox_shader);
    tex_use_cubemap(skybox_tex);
    glDepthFunc(GL_LEQUAL);
    mat3 view3;
    mat4 skybox_view;
    glm_mat4_pick3(view, view3);
    glm_mat4_ins3(view3, skybox_view);
    shader_set_mat4(skybox_shader, "view", skybox_view);
    shader_set_mat4(skybox_shader, "projection", projection);
    mesh_render(skybox_mesh);
    glDepthFunc(GL_LESS);

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

mesh_t mesh_init_pos() {
  mesh_t mesh;
  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(skyboxVertices), &skyboxVertices, GL_STATIC_DRAW);

  // position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  return mesh;
}

mesh_t mesh_init_pos_tex() {
  mesh_t mesh;
  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, sizeof(cubeVertices), &cubeVertices, GL_STATIC_DRAW);

  // position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  // texcoord
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  return mesh;
}

void mesh_render(mesh_t mesh) {
  glBindVertexArray(mesh.VAO);
  glDrawArrays(GL_TRIANGLES, 0, 36);
}

unsigned int tex_load(char* path, int mode) {
  unsigned int tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  int w, h, n;
  unsigned char* data = stbi_load(path, &w, &h, &n, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, mode, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
  } else {
    log_error("Failed to load texture \"%s\".", path);
  }
  stbi_image_free(data);
  return tex;
}

void tex_use(unsigned int tex) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_2D, tex);
}

unsigned int tex_load_cubemap(char** faces, int mode) {
  unsigned int tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  vec_str_t v;
  vec_init(&v);
  vec_pusharr(&v, faces, 6);
  int w, h, n;
  char* path;
  int i;
  vec_foreach(&v, path, i) {
    unsigned char* data = stbi_load(path, &w, &h, &n, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, mode, GL_UNSIGNED_BYTE, data);
    } else {
      log_error("Failed to load texture \"%s\".", path);
    }
    stbi_image_free(data);
  }
  vec_deinit(&v);
  return tex;
}

void tex_use_cubemap(unsigned int tex) {
  glActiveTexture(GL_TEXTURE0);
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}
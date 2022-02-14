#include "gfx.h"

SDL_Window* window;
SDL_GLContext ctx;
unsigned int basic_shader;
unsigned int blank_shader;

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

  if (!gladLoadGL()) {
    log_fatal("Failed to load opengl.");
    exit(-1);
  }
  glEnable(GL_DEPTH_TEST);
  glEnable(GL_MULTISAMPLE);

  ui_init(window, &ctx);
  log_info("Created window \"%s\".", title);
  log_info("Renderer: %s - %s", glGetString(GL_RENDERER), glGetString(GL_VERSION));
}

void window_loop() {
  unsigned int skybox_shader = shader_init("shaders/skybox.vert", "shaders/skybox.frag");
  glUniform1i(glGetUniformLocation(skybox_shader, "skybox"), 0);

  basic_shader = shader_init("shaders/basic.vert", "shaders/basic.frag");
  blank_shader = shader_init("shaders/blank.vert", "shaders/blank.frag");
  glUniform1i(glGetUniformLocation(basic_shader, "tex"), 0);

  mesh_t skybox_mesh = mesh_load_obj("cube.obj");
  unsigned int skybox_tex = tex_load_cubemap((char* [6]){"skybox/right.jpg", "skybox/left.jpg", "skybox/top.jpg", "skybox/bottom.jpg", "skybox/front.jpg", "skybox/back.jpg"}, GL_RGB);

  bool quit = false;
  int frame_delay = 1000 / FPS;
  while (!quit) {
    int frame_start = SDL_GetTicks();

    SDL_Event e;
    while (SDL_PollEvent(&e)) {
      switch (e.type) {
      case SDL_WINDOWEVENT:
        if (e.window.event == SDL_WINDOWEVENT_SIZE_CHANGED) {
          glViewport(0, 0, e.window.data1, e.window.data2);
        }
        break;
      case SDL_KEYDOWN:
        if (e.key.keysym.sym == SDLK_ESCAPE) {
          SDL_SetRelativeMouseMode(!SDL_GetRelativeMouseMode());
        }
        break;
      case SDL_QUIT:
        quit = true;
        break;
      }
      ui_processevent(&e);
      player_processevent(&e);
    }

    glClearColor(0.0, 0.0, 0.0, 1.0f);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

    mat4 view;
    player_movement(&view);
    mat4 projection = GLM_MAT4_IDENTITY;
    glm_perspective(glm_rad(80.0), conf.width / conf.height, 0.1, 100.0, projection);

    world_render(view, projection);

    glDepthFunc(GL_LEQUAL);
    shader_use(skybox_shader);
    tex_use_cubemap(skybox_tex);
    mat4 skybox_view = GLM_MAT4_ZERO;
    mat3 view3;
    glm_mat4_pick3(view, view3);
    glm_mat4_ins3(view3, skybox_view);
    shader_set_mat4(skybox_shader, "view", skybox_view);
    shader_set_mat4(skybox_shader, "projection", projection);
    mesh_render(skybox_mesh);
    glDepthFunc(GL_LESS);

    ui_render(window);
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
  const char* vert_src = asset_load(vert_path).data;
  unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, &vert_src, NULL);
  glCompileShader(vert);
  glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
  if (!success) {
    log_error("Failed to compile vertex shader \"%s\".", vert_path);
  }

  const char* frag_src = asset_load(frag_path).data;
  unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag, 1, &frag_src, NULL);
  glCompileShader(frag);
  glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
  if (!success) {
    log_error("Failed to compile fragment shader \"%s\".", frag_path);
  }

  unsigned int program = glCreateProgram();
  glAttachShader(program, vert);
  glAttachShader(program, frag);
  glLinkProgram(program);
  glGetProgramiv(program, GL_LINK_STATUS, &success);
  if (!success) {
    log_error("Failed to link \"%s\" and \"%s\".", vert_path, frag_path);
  } else {
    log_debug("Compiled shaders \"%s\" and \"%s\".", vert_path, frag_path);
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

mesh_t mesh_init_pos(float* verts, int size) {
  mesh_t mesh;
  mesh.verts = size;
  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, size * 12, verts, GL_STATIC_DRAW);

  // position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);

  return mesh;
}

mesh_t mesh_init_pos_tex(float* verts, int size) {
  mesh_t mesh;
  mesh.verts = size;
  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, size * 20, verts, GL_STATIC_DRAW);

  // position
  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
  // texcoord
  glEnableVertexAttribArray(1);
  glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3 * sizeof(float)));

  return mesh;
}

void load_obj(void* ctx, const char* filename, const int is_mtl, const char* obj_filename, char** buf, size_t* len) {
  asset_t asset = asset_load(filename);
  *buf = asset.data;
  *len = asset.len;
}

mesh_t mesh_load_obj(const char* path) {
  tinyobj_shape_t* shape = NULL;
  tinyobj_material_t* material = NULL;
  tinyobj_attrib_t attrib;
  tinyobj_attrib_init(&attrib);
  unsigned long num_shapes;
  unsigned long num_materials;
  tinyobj_parse_obj(&attrib, &shape, &num_shapes, &material, &num_materials, path, load_obj, NULL, TINYOBJ_FLAG_TRIANGULATE);

  float* verts = malloc(attrib.num_faces * 3 * sizeof(float));
  for (int i = 0; i < attrib.num_faces; i++) {
    unsigned int idx = attrib.faces[i].v_idx;
    verts[i * 3] = attrib.vertices[3 * idx];
    verts[i * 3 + 1] = attrib.vertices[3 * idx + 1];
    verts[i * 3 + 2] = attrib.vertices[3 * idx + 2];
  }

  log_debug("Loaded model \"%s\".", path);
  return mesh_init_pos(verts, attrib.num_faces);
}

void mesh_render(mesh_t mesh) {
  glBindVertexArray(mesh.VAO);
  glDrawArrays(GL_TRIANGLES, 0, mesh.verts);
}

unsigned int tex_load(char* path, int mode) {
  unsigned int tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  int w, h, n;
  asset_t img = asset_load(path);
  unsigned char* data = stbi_load_from_memory(img.data, img.len, &w, &h, &n, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, mode, GL_UNSIGNED_BYTE, data);
    log_debug("Loaded texture \"%s\".", path);
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
    asset_t img = asset_load(path);
    unsigned char* data = stbi_load_from_memory(img.data, img.len, &w, &h, &n, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, GL_RGB, w, h, 0, mode, GL_UNSIGNED_BYTE, data);
      log_debug("Loaded texture \"%s\".", path);
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
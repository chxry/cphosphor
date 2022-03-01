#include "gfx.h"

SDL_Window* window;
SDL_GLContext ctx;
int frame_delay;
mesh_t cube_mesh;
unsigned int basic_shader;
unsigned int debug_shader;

void window_init(char* title) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    log_error("Failed to init SDL.");
    exit(-1);
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, conf.width, conf.height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | (conf.fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
  ctx = SDL_GL_CreateContext(window);
  SDL_SetRelativeMouseMode(true);

  int gl = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
  glEnable(GL_DEPTH_TEST);
  glLineWidth(2.0);

  gbuffer_init(conf.width, conf.height);
  ui_init(window, &ctx);
  log_info("Loaded OpenGL %i.%i on \"%s\".", GLAD_VERSION_MAJOR(gl), GLAD_VERSION_MINOR(gl), glGetString(GL_RENDERER));
}

void window_loop() {
  cube_mesh = mesh_load_obj("mesh/sky.obj", pos);
  unsigned int skybox_tex = tex_load_cubemap((char* [6]){"tex/sky/right.jpg", "tex/sky/left.jpg", "tex/sky/top.jpg", "tex/sky/bottom.jpg", "tex/sky/front.jpg", "tex/sky/back.jpg"}, GL_RGB);
  unsigned int skybox_shader = shader_init("shaders/skybox.vert", "shaders/skybox.frag");
  basic_shader = shader_init("shaders/basic.vert", "shaders/basic.frag");
  debug_shader = shader_init("shaders/debug.vert", "shaders/debug.frag");

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
    gbuffer_render_shadows(light_view, light_projection);

    mat4 view, projection;
    player_movement(&view);
    glm_perspective(glm_rad(conf.fov), (float)conf.width / (float)conf.height, 0.1, 100.0, projection);
    glViewport(0, 0, conf.width, conf.height);
    glClearColor(0, 0, 0, 1);
    glBindFramebuffer(GL_FRAMEBUFFER, gbuffer);
    glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
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
    mesh_render(cube_mesh);
    glDepthFunc(GL_LESS);

    gbuffer_render(light_view, light_projection);
    ui_render(window);
    SDL_GL_SwapWindow(window);
    int frame_time = SDL_GetTicks() - frame_start;
    if (frame_delay > frame_time) {
      SDL_Delay(frame_delay - frame_time);
    }
  }
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
  }
  glDeleteShader(vert);
  glDeleteShader(frag);
  return program;
}

void shader_set_mat4(unsigned int shader, const char* name, mat4 val) {
  glUniformMatrix4fv(glGetUniformLocation(shader, name), 1, GL_FALSE, (float*)val);
}

void shader_set_vec3(unsigned int shader, const char* name, vec3 val) {
  glUniform3fv(glGetUniformLocation(shader, name), 1, (float*)val);
}

void shader_set_float(unsigned int shader, const char* name, float val) {
  glUniform1f(glGetUniformLocation(shader, name), val);
}

void shader_set_int(unsigned int shader, const char* name, int val) {
  glUniform1i(glGetUniformLocation(shader, name), val);
}

void shader_use(unsigned int shader) {
  glUseProgram(shader);
}

mesh_t mesh_init(float* verts, int len, mesh_attr attr) {
  mesh_t mesh;
  int stride = attr * sizeof(float);
  mesh.verts = len;
  glGenVertexArrays(1, &mesh.VAO);
  glGenBuffers(1, &mesh.VBO);
  glBindVertexArray(mesh.VAO);
  glBindBuffer(GL_ARRAY_BUFFER, mesh.VBO);
  glBufferData(GL_ARRAY_BUFFER, len * stride, verts, GL_STATIC_DRAW);

  glEnableVertexAttribArray(0);
  glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, stride, (void*)0);
  if (attr >= pos_tex) {
    glEnableVertexAttribArray(1);
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_TRUE, stride, (void*)(3 * sizeof(float)));
    if (attr >= pos_tex_norm) {
      glEnableVertexAttribArray(2);
      glVertexAttribPointer(2, 3, GL_FLOAT, GL_TRUE, stride, (void*)(5 * sizeof(float)));
    }
  }

  return mesh;
}

void load_obj(void* ctx, const char* filename, const int is_mtl, const char* obj_filename, char** buf, size_t* len) {
  asset_t asset = asset_load(filename);
  *buf = asset.data;
  *len = asset.len;
}

mesh_t mesh_load_obj(const char* path, mesh_attr attr) {
  tinyobj_shape_t* shape = NULL;
  tinyobj_material_t* material = NULL;
  tinyobj_attrib_t attrib;
  tinyobj_attrib_init(&attrib);
  unsigned long num_shapes;
  unsigned long num_materials;
  tinyobj_parse_obj(&attrib, &shape, &num_shapes, &material, &num_materials, path, load_obj, NULL, TINYOBJ_FLAG_TRIANGULATE);

  float* verts = malloc(attrib.num_faces * attr * sizeof(float));
  for (int i = 0; i < attrib.num_faces; i++) {
    unsigned int pos = attrib.faces[i].v_idx;
    unsigned int tex = attrib.faces[i].vt_idx;
    unsigned int norm = attrib.faces[i].vn_idx;
    verts[i * attr] = attrib.vertices[3 * pos];
    verts[i * attr + 1] = attrib.vertices[3 * pos + 1];
    verts[i * attr + 2] = attrib.vertices[3 * pos + 2];
    if (attr >= pos_tex) {
      verts[i * attr + 3] = attrib.texcoords[2 * tex];
      verts[i * attr + 4] = attrib.texcoords[2 * tex + 1];
      if (attr >= pos_tex_norm) {
        verts[i * attr + 5] = attrib.normals[3 * norm];
        verts[i * attr + 6] = attrib.normals[3 * norm + 1];
        verts[i * attr + 7] = attrib.normals[3 * norm + 2];
      }
    }
  }

  return mesh_init(verts, attrib.num_faces, attr);
}

void mesh_render(mesh_t mesh) {
  glBindVertexArray(mesh.VAO);
  glDrawArrays(GL_TRIANGLES, 0, mesh.verts);
}

unsigned int tex_load(const char* path, int mode) {
  unsigned int tex;
  glGenTextures(1, &tex);
  glBindTexture(GL_TEXTURE_2D, tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  int w, h, n;
  asset_t img = asset_load(path);
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load_from_memory(img.data, img.len, &w, &h, &n, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, w, h, 0, mode, GL_UNSIGNED_BYTE, data);
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
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);

  vec_str_t v;
  vec_init(&v);
  vec_pusharr(&v, faces, 6);
  int w, h, n;
  char* path;
  int i;
  stbi_set_flip_vertically_on_load(false);
  vec_foreach(&v, path, i) {
    asset_t img = asset_load(path);
    unsigned char* data = stbi_load_from_memory(img.data, img.len, &w, &h, &n, 0);
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
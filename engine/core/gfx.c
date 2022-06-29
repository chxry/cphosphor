#include "gfx.h"

SDL_Window* window;
SDL_GLContext ctx;
int frame_delay;
unsigned int basic_shader;
unsigned int debug_shader;
unsigned int skybox_shader;
unsigned int atmosphere_shader;
ImFont* display_font;

void window_init(int width, int height, bool fullscreen, char* title) {
  if (SDL_Init(SDL_INIT_EVERYTHING)) {
    log_error("Failed to init SDL.");
    exit(-1);
  }
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 4);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 6);
  SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
  window = SDL_CreateWindow(title, SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, width, height, SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | (fullscreen ? SDL_WINDOW_FULLSCREEN : 0));
  ctx = SDL_GL_CreateContext(window);

  int gl = gladLoadGL((GLADloadfunc)SDL_GL_GetProcAddress);
  glEnable(GL_DEPTH_TEST);
  glLineWidth(2.0);

  gbuffer_init(width, height);
  basic_shader = shader_init("shaders/basic.vert", "shaders/basic.frag");
  debug_shader = shader_init("shaders/debug.vert", "shaders/debug.frag");
  skybox_shader = shader_init("shaders/skybox.vert", "shaders/skybox.frag");
  atmosphere_shader = shader_init("shaders/skybox.vert", "shaders/atmosphere.frag");
  log_info("Loaded OpenGL %i.%i on \"%s\".", GLAD_VERSION_MAJOR(gl), GLAD_VERSION_MINOR(gl), glGetString(GL_RENDERER));
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
    glDeleteTextures(1, &tex);
    tex = 0;
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

void imgui_init(bool docking) {
  igCreateContext(NULL);
  ImGuiIO* io = igGetIO();
  if (docking) {
    io->ConfigWindowsMoveFromTitleBarOnly = true;
    io->ConfigFlags |= ImGuiConfigFlags_DockingEnable;
  }
  ImGuiStyle* style = igGetStyle();
  ImVec4* colors = style->Colors;
  colors[ImGuiCol_Text] = (ImVec4){1.00, 1.00, 1.00, 1.00};
  colors[ImGuiCol_TextDisabled] = (ImVec4){0.50, 0.50, 0.50, 1.00};
  colors[ImGuiCol_WindowBg] = (ImVec4){0.10, 0.10, 0.10, 1.00};
  colors[ImGuiCol_ChildBg] = (ImVec4){0.00, 0.00, 0.00, 0.00};
  colors[ImGuiCol_PopupBg] = (ImVec4){0.19, 0.19, 0.19, 0.92};
  colors[ImGuiCol_Border] = (ImVec4){0.19, 0.19, 0.19, 0.29};
  colors[ImGuiCol_BorderShadow] = (ImVec4){0.00, 0.00, 0.00, 0.24};
  colors[ImGuiCol_FrameBg] = (ImVec4){0.05, 0.05, 0.05, 0.54};
  colors[ImGuiCol_FrameBgHovered] = (ImVec4){0.19, 0.19, 0.19, 0.54};
  colors[ImGuiCol_FrameBgActive] = (ImVec4){0.20, 0.22, 0.23, 1.00};
  colors[ImGuiCol_TitleBg] = (ImVec4){0.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_TitleBgActive] = (ImVec4){0.06, 0.06, 0.06, 1.00};
  colors[ImGuiCol_TitleBgCollapsed] = (ImVec4){0.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_MenuBarBg] = (ImVec4){0.14, 0.14, 0.14, 1.00};
  colors[ImGuiCol_ScrollbarBg] = (ImVec4){0.05, 0.05, 0.05, 0.54};
  colors[ImGuiCol_ScrollbarGrab] = (ImVec4){0.34, 0.34, 0.34, 0.54};
  colors[ImGuiCol_ScrollbarGrabHovered] = (ImVec4){0.40, 0.40, 0.40, 0.54};
  colors[ImGuiCol_ScrollbarGrabActive] = (ImVec4){0.56, 0.56, 0.56, 0.54};
  colors[ImGuiCol_CheckMark] = (ImVec4){0.33, 0.67, 0.86, 1.00};
  colors[ImGuiCol_SliderGrab] = (ImVec4){0.34, 0.34, 0.34, 0.54};
  colors[ImGuiCol_SliderGrabActive] = (ImVec4){0.56, 0.56, 0.56, 0.54};
  colors[ImGuiCol_Button] = (ImVec4){0.05, 0.05, 0.05, 0.54};
  colors[ImGuiCol_ButtonHovered] = (ImVec4){0.19, 0.19, 0.19, 0.54};
  colors[ImGuiCol_ButtonActive] = (ImVec4){0.20, 0.22, 0.23, 1.00};
  colors[ImGuiCol_Header] = (ImVec4){0.00, 0.00, 0.00, 0.52};
  colors[ImGuiCol_HeaderHovered] = (ImVec4){0.00, 0.00, 0.00, 0.36};
  colors[ImGuiCol_HeaderActive] = (ImVec4){0.20, 0.22, 0.23, 0.33};
  colors[ImGuiCol_Separator] = (ImVec4){0.28, 0.28, 0.28, 0.29};
  colors[ImGuiCol_SeparatorHovered] = (ImVec4){0.44, 0.44, 0.44, 0.29};
  colors[ImGuiCol_SeparatorActive] = (ImVec4){0.40, 0.44, 0.47, 1.00};
  colors[ImGuiCol_ResizeGrip] = (ImVec4){0.28, 0.28, 0.28, 0.29};
  colors[ImGuiCol_ResizeGripHovered] = (ImVec4){0.44, 0.44, 0.44, 0.29};
  colors[ImGuiCol_ResizeGripActive] = (ImVec4){0.40, 0.44, 0.47, 1.00};
  colors[ImGuiCol_Tab] = (ImVec4){0.00, 0.00, 0.00, 0.52};
  colors[ImGuiCol_TabHovered] = (ImVec4){0.14, 0.14, 0.14, 1.00};
  colors[ImGuiCol_TabActive] = (ImVec4){0.20, 0.20, 0.20, 0.36};
  colors[ImGuiCol_TabUnfocused] = (ImVec4){0.00, 0.00, 0.00, 0.52};
  colors[ImGuiCol_TabUnfocusedActive] = (ImVec4){0.14, 0.14, 0.14, 1.00};
  colors[ImGuiCol_DockingPreview] = (ImVec4){0.33, 0.67, 0.86, 1.00};
  colors[ImGuiCol_DockingEmptyBg] = (ImVec4){0.10, 0.10, 0.10, 1.00};
  colors[ImGuiCol_PlotLines] = (ImVec4){1.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_PlotLinesHovered] = (ImVec4){1.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_PlotHistogram] = (ImVec4){1.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_PlotHistogramHovered] = (ImVec4){1.00, 0.00, 0.00, 1.00};
  colors[ImGuiCol_TableHeaderBg] = (ImVec4){0.00, 0.00, 0.00, 0.52};
  colors[ImGuiCol_TableBorderStrong] = (ImVec4){0.00, 0.00, 0.00, 0.52};
  colors[ImGuiCol_TableBorderLight] = (ImVec4){0.28, 0.28, 0.28, 0.29};
  colors[ImGuiCol_TableRowBg] = (ImVec4){0.00, 0.00, 0.00, 0.00};
  colors[ImGuiCol_TableRowBgAlt] = (ImVec4){1.00, 1.00, 1.00, 0.06};
  colors[ImGuiCol_TextSelectedBg] = (ImVec4){0.20, 0.22, 0.23, 1.00};
  colors[ImGuiCol_DragDropTarget] = (ImVec4){0.33, 0.67, 0.86, 1.00};
  colors[ImGuiCol_NavHighlight] = (ImVec4){0.05, 0.05, 0.05, 0.54};
  colors[ImGuiCol_NavWindowingHighlight] = (ImVec4){0.19, 0.19, 0.19, 0.54};
  colors[ImGuiCol_NavWindowingDimBg] = (ImVec4){1.00, 0.00, 0.00, 0.20};
  colors[ImGuiCol_ModalWindowDimBg] = (ImVec4){1.00, 0.00, 0.00, 0.35};
  style->WindowRounding = 4.0;
  style->PopupRounding = 4.0;
  style->FrameRounding = 2.0;

  asset_t roboto = asset_load("roboto.ttf");
  asset_t fa = asset_load("fontawesome.ttf");
  asset_t clashdisplay = asset_load("clashdisplay.ttf");
  ImFontConfig config = *ImFontConfig_ImFontConfig();
  ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, roboto.data, roboto.len, 16, &config, NULL);
  config.MergeMode = true;
  static const ImWchar icon_ranges[] = {ICON_MIN_FA, ICON_MAX_FA, 0};
  ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, fa.data, fa.len, 12, &config, icon_ranges);
  config.MergeMode = false;
  display_font = ImFontAtlas_AddFontFromMemoryTTF(io->Fonts, clashdisplay.data, clashdisplay.len, 54, &config, NULL);
  ImFontAtlas_Build(io->Fonts);
  ImGui_ImplSDL2_InitForOpenGL(window, ctx);
  ImGui_ImplOpenGL3_Init("#version 460");
}

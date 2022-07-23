#include "gfx.h"

SDL_Window* window;
SDL_GLContext ctx;
int frame_delay;
unsigned int basic_shader;
unsigned int debug_shader;
unsigned int skybox_shader;
unsigned int atmosphere_shader;
unsigned int shadow_shader;
unsigned int lighting_shader;
unsigned int billboard_shader;
ImFont* display_font;
ImFont* large_icons;

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
  glEnable(GL_BLEND);
  glLineWidth(2.0);

  renderer_init(width, height);
  log_info("Loaded OpenGL %i.%i on \"%s\".", GLAD_VERSION_MAJOR(gl), GLAD_VERSION_MINOR(gl), glGetString(GL_RENDERER));
}

void shaders_init() {
  // this is cursed
  basic_shader = get_shader(1391274353)->program;
  debug_shader = get_shader(1583569869)->program;
  skybox_shader = get_shader(2083617856)->program;
  atmosphere_shader = get_shader(765834938)->program;
  shadow_shader = get_shader(1246522299)->program;
  lighting_shader = get_shader(1334256266)->program;
  billboard_shader = get_shader(844270324)->program;
  shader_use(basic_shader);
  shader_set_int(basic_shader, "skybox", 0);
  shader_set_int(basic_shader, "tex", 1);
  shader_use(lighting_shader);
  shader_set_int(lighting_shader, "gPosition", 0);
  shader_set_int(lighting_shader, "gNormal", 1);
  shader_set_int(lighting_shader, "gAlbedospec", 2);
  shader_set_int(lighting_shader, "shadowmap", 3);
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

void mesh_render(mesh_t mesh) {
  glBindVertexArray(mesh.VAO);
  glDrawArrays(GL_TRIANGLES, 0, mesh.verts);
}

void tex_use(unsigned int tex, int slot) {
  glActiveTexture(slot);
  glBindTexture(GL_TEXTURE_2D, tex);
}

void tex_use_cubemap(unsigned int tex, int slot) {
  glActiveTexture(slot);
  glBindTexture(GL_TEXTURE_CUBE_MAP, tex);
}

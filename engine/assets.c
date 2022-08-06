#include "assets.h"

map_asset_t assets;

filedata_t load_file(char* path) {
  filedata_t file;
  PHYSFS_file* f = PHYSFS_openRead(path);
  file.len = PHYSFS_fileLength(f);
  file.data = malloc(file.len + 1);
  PHYSFS_readBytes(f, file.data, file.len);
  ((char*)file.data)[file.len] = 0;
  return file;
}

tex_t* load_tex(JSON_Object* obj) {
  tex_t* tex = malloc(sizeof(tex_t));
  tex->path = json_object_get_string(obj, "path");
  int format = json_object_get_boolean(obj, "alpha") ? GL_RGBA : GL_RGB;
  char buf[64];
  snprintf(buf, sizeof(buf), "Loading \"%s\".", tex->path);
  splash_render(buf, 1280, 720);

  glGenTextures(1, &tex->tex);
  glBindTexture(GL_TEXTURE_2D, tex->tex);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
  glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
  int w, h, n;
  filedata_t file = load_file(tex->path);
  stbi_set_flip_vertically_on_load(true);
  unsigned char* data = stbi_load_from_memory(file.data, file.len, &w, &h, &n, 0);
  if (data) {
    glTexImage2D(GL_TEXTURE_2D, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
  } else {
    log_error("Failed to load texture \"%s\".", tex->path);
    glDeleteTextures(1, &tex->tex);
    tex->tex = 0;
  }
  stbi_image_free(data);
  return tex;
}

cubemap_t* load_cubemap(JSON_Object* obj) {
  cubemap_t* cm = malloc(sizeof(cubemap_t));
  cm->id = json_object_get_number(obj, "id");
  int format = json_object_get_boolean(obj, "alpha") ? GL_RGBA : GL_RGB;

  glGenTextures(1, &cm->tex);
  glBindTexture(GL_TEXTURE_CUBE_MAP, cm->tex);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
  glTexParameteri(GL_TEXTURE_CUBE_MAP, GL_TEXTURE_WRAP_R, GL_CLAMP_TO_EDGE);
  stbi_set_flip_vertically_on_load(false);
  int w, h, n;

  JSON_Object* arr = json_object_get_array(obj, "tex");
  for (int i = 0; i < json_array_get_count(arr); i++) {
    char* path = json_array_get_string(arr, i);
    char buf[64];
    snprintf(buf, sizeof(buf), "Loading \"%s\".", path);
    splash_render(buf, 1280, 720);
    filedata_t file = load_file(path);
    unsigned char* data = stbi_load_from_memory(file.data, file.len, &w, &h, &n, 0);
    if (data) {
      glTexImage2D(GL_TEXTURE_CUBE_MAP_POSITIVE_X + i, 0, format, w, h, 0, format, GL_UNSIGNED_BYTE, data);
    } else {
      log_error("Failed to load texture \"%s\".", path);
    }
    stbi_image_free(data);
  }
  return cm;
}

void load_obj(void* ctx, const char* filename, const int is_mtl, const char* obj_filename, char** buf, size_t* len) {
  filedata_t file = load_file(filename);
  *buf = file.data;
  *len = file.len;
}

mesh_t* load_mesh(JSON_Object* obj) {
  mesh_t* mesh = malloc(sizeof(mesh_t));
  char* path = json_object_get_string(obj, "path");
  int attr = json_object_get_number(obj, "attr");
  char buf[64];
  snprintf(buf, sizeof(buf), "Loading \"%s\".", path);
  splash_render(buf, 1280, 720);

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

  mesh_t m = mesh_init(verts, attrib.num_faces, attr);
  memcpy(mesh, &m, sizeof(mesh_t));
  mesh->path = path;
  return mesh;
}

shader_t* load_shader(JSON_Object* obj) {
  shader_t* shader = malloc(sizeof(shader_t));
  shader->id = json_object_get_number(obj, "id");
  char* vert_path = json_object_get_string(obj, "vert");
  char* frag_path = json_object_get_string(obj, "frag");
  int success;
  char buf[512];
  const char* vert_src = load_file(vert_path).data;
  unsigned int vert = glCreateShader(GL_VERTEX_SHADER);
  glShaderSource(vert, 1, &vert_src, NULL);
  glCompileShader(vert);
  glGetShaderiv(vert, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(vert, 512, NULL, buf);
    log_error("Failed to compile vertex shader \"%s\".", vert_path);
    log_error("%s", strtok(buf, "\n"));
  }

  const char* frag_src = load_file(frag_path).data;
  unsigned int frag = glCreateShader(GL_FRAGMENT_SHADER);
  glShaderSource(frag, 1, &frag_src, NULL);
  glCompileShader(frag);
  glGetShaderiv(frag, GL_COMPILE_STATUS, &success);
  if (!success) {
    glGetShaderInfoLog(frag, 512, NULL, buf);
    log_error("Failed to compile fragment shader \"%s\".", frag_path);
    log_error("%s", strtok(buf, "\n"));
  }

  shader->program = glCreateProgram();
  glAttachShader(shader->program, vert);
  glAttachShader(shader->program, frag);
  glLinkProgram(shader->program);
  glGetProgramiv(shader->program, GL_LINK_STATUS, &success);
  if (!success) {
    log_error("Failed to link \"%s\" and \"%s\".", vert_path, frag_path);
  }
  glDeleteShader(vert);
  glDeleteShader(frag);
  return shader;
}

sound_t* load_sound(JSON_Object* obj) {
  sound_t* sound = malloc(sizeof(sound_t));
  sound->path = json_object_get_string(obj, "path");
  char buf[64];
  snprintf(buf, sizeof(buf), "Loading \"%s\".", sound->path);
  splash_render(buf, 1280, 720);
  filedata_t file = load_file(sound->path);
  FMOD_CREATESOUNDEXINFO exinfo = {.cbsize = sizeof(FMOD_CREATESOUNDEXINFO), .length = file.len};
  fmod_checked(FMOD_System_CreateSound(fmod_system, file.data, FMOD_OPENMEMORY | FMOD_3D, &exinfo, &sound->sound));
  return sound;
}

luafile_t* load_luafile(JSON_Object* obj) {
  luafile_t* luafile = malloc(sizeof(luafile_t));
  luafile->path = json_object_get_string(obj, "path");
  char buf[64];
  snprintf(buf, sizeof(buf), "Loading \"%s\".", luafile->path);
  splash_render(buf, 1280, 720);
  filedata_t file = load_file(luafile->path);
  luafile->contents = file.data;
  return luafile;
}

asset_t tex = {.load = load_tex};
asset_t cubemap = {.load = load_cubemap};
asset_t mesh = {.load = load_mesh};
asset_t shader = {.load = load_shader};
asset_t sound = {.load = load_sound};
asset_t luafile = {.load = load_luafile};

void assets_init(char* path) {
  map_init(&assets);
  asset_register("tex", tex);
  asset_register("cubemap", cubemap);
  asset_register("mesh", mesh);
  asset_register("shader", shader);
  asset_register("sound", sound);
  asset_register("luafile", luafile);
  PHYSFS_init(NULL);
  PHYSFS_mount(path, "", 0);
  JSON_Object* root = json_object(json_parse_file("res/assets.json"));
  for (int i = 0; i < json_object_get_count(root); i++) {
    JSON_Array* arr = json_value_get_array(json_object_get_value_at(root, i));
    asset_t* asset = map_get(&assets, json_object_get_name(root, i));
    for (int j = 0; j < json_array_get_count(arr); j++) {
      vec_push(&asset->assets, asset->load(json_array_get_object(arr, j)));
    }
  }

  log_info("Loaded assets from \"%s\".", path);
}

void asset_register(char* name, asset_t asset) {
  vec_init(&asset.assets);
  map_set(&assets, name, asset);
}

void* get_asset_path(char* path, char* name) {
  int i;
  void* asset;
  vec_foreach(&(map_get(&assets, name)->assets), asset, i) {
    if (strcmp(*((char**)asset), path) == 0) {
      return asset;
    }
  }
  return 0;
}

void* get_asset_id(int id, char* name) {
  int i;
  void* asset;
  vec_foreach(&(map_get(&assets, name)->assets), asset, i) {
    if (*((int*)asset) == id) {
      return asset;
    }
  }
  return 0;
}

tex_t* get_tex(char* path) {
  return get_asset_path(path, "tex");
}

cubemap_t* get_cubemap(int id) {
  return get_asset_id(id, "cubemap");
}

mesh_t* get_mesh(char* path) {
  return get_asset_path(path, "mesh");
}

shader_t* get_shader(int id) {
  return get_asset_id(id, "shader");
}

sound_t* get_sound(char* path) {
  return get_asset_path(path, "sound");
}

luafile_t* get_luafile(char* path) {
  return get_asset_path(path, "luafile");
}

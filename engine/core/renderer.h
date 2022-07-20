#pragma once
#include <glad/gl.h>
#include <cglm/cglm.h>
#include <stdbool.h>

#include "ecs/world.h"
#include "util/math.h"
#include "assets.h"
#include "gfx.h"

#define SHADOW_RES 8192

extern unsigned int shadowmap;

void renderer_init(int width, int height);
void renderer_resize(int width, int height);
void renderer_render(unsigned int fbo, vec3 cam_pos, vec3 cam_dir, float fov, int x, int y, int colliders, bool billboards);

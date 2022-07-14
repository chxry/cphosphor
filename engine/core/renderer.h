#pragma once
#include <glad/gl.h>
#include <cglm/cglm.h>

#include "ecs/world.h"
#include "assets.h"
#include "gfx.h"

#define SHADOW_RES 8192

extern unsigned int gbuffer;

void renderer_init(int width, int height);
void renderer_resize(int width, int height);
void renderer_render(unsigned int fbo, mat4 view, mat4 projection, int x, int y, int colliders);

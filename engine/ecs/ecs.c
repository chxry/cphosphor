#include "ecs.h"

vec_component_t components; // maybe use a map here instead?

void ecs_init() {
  vec_init(&components);
}

void component_register(component_t component) {
  vec_push(&components, component);
}

void component_iter(void (*f)(component_t c)) {
  int i;
  component_t c;
  vec_foreach(&components, c, i) {
    f(c);
  }
}

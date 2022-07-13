#include "physics.h"

void* physics_universe;

void physics_init() {
  physics_universe = universeCreate();
  universeSetGravity(physics_universe, 0, -9.98, 0);

  int i;
  rigidbody_t* rb;
  vec_foreach(&get_component("rigidbody")->components, rb, i) {
    entity_t* entity = get_entity(rb->entity);
    void* shape = shapeCreateBox(physics_universe, entity->scale[0], entity->scale[1], entity->scale[2]);
    rb->body = bodyCreate(physics_universe, shape, rb->mass, entity->pos[0], entity->pos[1], entity->pos[2]);
    bodySetRotationEular(rb->body, glm_rad(entity->rot[0]), glm_rad(entity->rot[1]), glm_rad(entity->rot[2]));
  }
}

void physics_update() {
  universeStep(physics_universe, 1.0 / 144.0, 8);
  int i;
  rigidbody_t* rb;
  vec_foreach(&get_component("rigidbody")->components, rb, i) {
    entity_t* entity = get_entity(rb->entity);
    Vec pos;
    Vec rotq;
    bodyGetPositionAndOrientation(rb->body, &pos, &rotq);
    vec3 rot;
    quat_to_euler(&rotq, rot);
    entity->pos[0] = pos.x;
    entity->pos[1] = pos.y;
    entity->pos[2] = pos.z;
    entity->rot[0] = glm_deg(rot[0]);
    entity->rot[1] = glm_deg(rot[1]);
    entity->rot[2] = glm_deg(rot[2]);
  }
}
#include "../include/cam.h"
#include <string.h>
#include <cglm/cglm.h>

void cam_init(Camera *cam) {
    float f0 = 0.0f;
    memset(cam->pos,   *((int*)&f0), 3 * sizeof(float));
    memset(cam->front, *((int*)&f0), 3 * sizeof(float));
    memset(cam->up,    *((int*)&f0), 3 * sizeof(float));
    memset(cam->vel,   *((int*)&f0), 3 * sizeof(float));
    memset(cam->accel, *((int*)&f0), 3 * sizeof(float));
    cam->front[2] = -1;
    cam->up[1] = 1;
    cam->yaw = -90;
    cam->pitch = 0;
    cam->sensitivity = 0.005;
    cam->speed = 25;
}

void cam_mouse_move(Camera *cam, double xmove, double ymove) {
    cam->yaw   += xmove * cam->sensitivity;
    cam->pitch += ymove * cam->sensitivity;
    
    if (cam->pitch > GLM_PI / 2 - 0.01) {
        cam->pitch = GLM_PI / 2 - 0.01;
    } else if (cam->pitch < -GLM_PI / 2 + 0.01) {
        cam->pitch = -GLM_PI / 2 + 0.01;
    }

    if (cam->yaw >  GLM_PI) cam->yaw = -GLM_PI;
    else if (cam->yaw < -GLM_PI) cam->yaw =  GLM_PI;
}

void cam_keyboard_move(Camera *cam, bool w, bool a, bool s, bool d, bool shift, bool space) {
    cam->accel[0] = 0;
    cam->accel[1] = 0;
    cam->accel[2] = 0;
    
    vec3 camLeft;
    glm_cross(cam->front, cam->up, camLeft);
    glm_vec3_normalize(camLeft);
    glm_vec3_scale(camLeft, cam->speed, camLeft);

    vec3 camForward = {cam->front[0], 0, cam->front[2]};
    glm_vec3_normalize(camForward);
    glm_vec3_scale(camForward, cam->speed, camForward);

    if (w)     glm_vec3_add(cam->accel, camForward, cam->accel);
    if (s)     glm_vec3_sub(cam->accel, camForward, cam->accel);
    if (d)     glm_vec3_add(cam->accel, camLeft,    cam->accel);
    if (a)     glm_vec3_sub(cam->accel, camLeft,    cam->accel);
    if (space) glm_vec3_add(cam->accel, (vec3){0, cam->speed * 4, 0}, cam->accel);
    if (shift) glm_vec3_sub(cam->accel, (vec3){0, cam->speed * 4, 0}, cam->accel);
}

void cam_view_matrix(Camera *cam, mat4 view) {
    vec3 direction = {
        cos(cam->yaw) * cos(cam->pitch),
        sin(cam->pitch),
        sin(cam->yaw) * cos(cam->pitch)
    };

    glm_vec3_normalize_to(direction, cam->front);

    vec3 camLook;
    glm_vec3_add(cam->pos, cam->front, camLook);
    glm_lookat(cam->pos, camLook, cam->up, view);
}

void cam_update(Camera *cam, float delta_time) {
    vec3 norm_accel;
    vec3 norm_vel;
    vec3 friction;

    //I don't know why but I don't need to scale these by delta_time
    glm_vec3_scale(cam->vel, -(5.0f / 3.0f), friction);
    friction[1] = cam->vel[1] * -10.0f;
    glm_vec3_add(cam->accel, friction, cam->accel);
    
    glm_vec3_scale(cam->accel, delta_time, norm_accel);
    glm_vec3_add(cam->vel, norm_accel, cam->vel);

    glm_vec3_scale(cam->vel, delta_time, norm_vel);
    glm_vec3_add(cam->pos, norm_vel, cam->pos);
}

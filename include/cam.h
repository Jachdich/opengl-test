#include <cglm/vec3.h>

struct Camera {
    vec3 pos;
    vec3 front;
    vec3 up;
    float yaw;
    float pitch;
    float sensitivity;
    float speed;

    vec3 vel;
    vec3 accel;
};

typedef struct Camera Camera;

void cam_init(Camera *cam);
void cam_mouse_move(Camera *cam, double xmove, double ymove);
void cam_keyboard_move(Camera *cam, bool w, bool a, bool s, bool d, bool shift, bool space);
void cam_view_matrix(Camera *cam, mat4 view);
void cam_update(Camera *cam, float delta_time);


#include <stdio.h>
#include "../include/glad/glad.h"
#include <GLFW/glfw3.h>
#include <stdbool.h>
#include <stdlib.h>
#include <stdint.h>
#include <math.h>
#include <cglm/cglm.h>
#define STB_IMAGE_IMPLEMENTATION
#include "../include/stb_image.h"
#include "../include/cam.h"
#include "../include/shaders.h"

Camera cam;

int winwidth = 1920;
int winheight = 1080;


struct Vertex {
    vec3 pos;
};
typedef struct Vertex Vertex;

Vertex vertices[] = {
    (Vertex){-1.0f, -1.0f, 0.0f},
    (Vertex){1.0f, -1.0f, 0.0f},
    (Vertex){-1.0f, 1.0f, 0.0f},
    (Vertex){1.0f, 1.0f, 0.0f},
};

int indices[] = {
    0, 1, 2, 1, 2, 3
};

struct Mesh {
    uint32_t vbo, vao, ebo;
    uint32_t shader;
    size_t num_textures;
    size_t num_vertices;
    size_t num_indices;
    Vertex *vertices;
    int *indices;
    uint32_t rad;
};
typedef struct Mesh Mesh;

void fb_size_callback(GLFWwindow *win, int w, int h) {
    (void)win;
    glViewport(0, 0, w, h);
    winwidth = w;
    winheight = h;
}


void process_input(GLFWwindow *win) {
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(win, true);
    }
    cam_keyboard_move(&cam,
        glfwGetKey(win, GLFW_KEY_W) == GLFW_PRESS,
        glfwGetKey(win, GLFW_KEY_A) == GLFW_PRESS,
        glfwGetKey(win, GLFW_KEY_S) == GLFW_PRESS,
        glfwGetKey(win, GLFW_KEY_D) == GLFW_PRESS,
        glfwGetKey(win, GLFW_KEY_LEFT_SHIFT) == GLFW_PRESS,
        glfwGetKey(win, GLFW_KEY_SPACE) == GLFW_PRESS
    );
    
}

double lastxpos = 1920/2;
double lastypos = 1080/2;

void mouse_callback(GLFWwindow*, double xpos, double ypos) {
    double deltax = xpos - lastxpos;
    double deltay = lastypos - ypos;

    cam_mouse_move(&cam, deltax, deltay);
    
    lastxpos = xpos;
    lastypos = ypos;
}

Mesh mesh_init(Vertex *vertices, size_t num_vertices, 
               int *indices, size_t num_indices,
               const char *vertfile, const char *fragfile,
               size_t num_textures) {
    Mesh obj;
    
    glGenBuffers(1, &obj.vbo);
    glGenBuffers(1, &obj.ebo);
    glGenVertexArrays(1, &obj.vao);

    glBindVertexArray(obj.vao);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, obj.ebo);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, num_indices * sizeof(*indices), indices, GL_STATIC_DRAW);

    //pos component of vbo
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glEnableVertexAttribArray(0);

    obj.shader = make_shader_program(vertfile, fragfile);
    glUseProgram(obj.shader);

    obj.num_textures = num_textures;

    obj.num_vertices = num_vertices;
    obj.indices = indices;
    obj.num_indices = num_indices;
    
    return obj;
}

void mesh_delete(Mesh *obj) {
    glDeleteVertexArrays(1, &obj->vao);
    glDeleteBuffers(1, &obj->vbo);
    glDeleteBuffers(1, &obj->ebo);
    glDeleteProgram(obj->shader);
    //free(obj->vertices);
    //free(obj->indices);
}

void mesh_draw(Mesh *mesh, mat4 view, mat4 projection) {
    glUseProgram(mesh->shader);
    glUniform1i(glGetUniformLocation(mesh->shader, "tex1"), 0);
    glUniformMatrix4fv(glGetUniformLocation(mesh->shader, "view"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(mesh->shader, "projection"), 1, GL_FALSE, (float*)projection);
    glUniform3fv(glGetUniformLocation(mesh->shader, "viewPos"), 1, (float*)cam.pos);
}

Mesh cube;

int main() {
    cam_init(&cam);

    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    
    GLFWwindow *window = glfwCreateWindow(winwidth, winheight, "Test Program", NULL, NULL);
    if (window == NULL) {
        fprintf(stderr, "Failed to create window!\n");
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);
    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        fprintf(stderr, "Failed to initialise GLAD!\n");
        glfwTerminate();
        return -1;
    }
    
    glViewport(0, 0, winwidth, winheight);
    glfwSetFramebufferSizeCallback(window, fb_size_callback);

    glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);  
    glfwSetCursorPosCallback(window, mouse_callback);  

    glEnable(GL_DEPTH_TEST);
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(65535);
    
    cube = mesh_init(vertices, 4, indices, 6, "shaders/vertex.glsl", "shaders/frag.glsl", 0);
    float totalTime = 0;
    float lastNow = glfwGetTime();
    int frames = 0;

    while (!glfwWindowShouldClose(window)) {

        float now = glfwGetTime();
        float delta_time = now - lastNow;
        lastNow = now;

        totalTime += delta_time;
        if (totalTime > 0.1) {
            printf("%f\n", frames / totalTime);
            totalTime = 0;
            frames = 0;
        }

        process_input(window);

        //glClearColor(0.2f, 0.9f, 1.0f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        mat4 view = GLM_MAT4_IDENTITY_INIT;
        mat4 projection = GLM_MAT4_IDENTITY_INIT;

        glm_translate(view, (vec3){0, 0, -3});
        glm_perspective(GLM_PI/2, (float)winwidth / (float)winheight, 0.1, 100, projection);
        // glm_ortho(0.0f, 80.0f, 0.0f, 80.0f, 0.1, 100, projection);

        cam_update(&cam, delta_time);
        vec3 look;
        cam_view_matrix(&cam, look);
        printf("%f %f %f\t%f %f %f\n", look[0], look[1], look[2], cam.pos[0], cam.pos[1], cam.pos[2]);
        
        glBindVertexArray(cube.vao);
        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glUseProgram(cube.shader);
        glUniformMatrix4fv(glGetUniformLocation(cube.shader, "view"), 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(cube.shader, "projection"), 1, GL_FALSE, (float*)projection);
        glUniform3fv(glGetUniformLocation(cube.shader, "viewPos"), 1, (float*)cam.pos);
        glUniform3fv(glGetUniformLocation(cube.shader, "camLook"), 1, (float*)look);
        glUniformMatrix4fv(glGetUniformLocation(cube.shader, "model"), 1, GL_FALSE, (float*)model);

        glUniform2f(glGetUniformLocation(cube.shader, "resolution"), (float)winwidth, (float)winheight);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.ebo);
        glDrawElements(GL_TRIANGLES, cube.num_indices, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        frames++;
    }

    glfwTerminate();
    
    mesh_delete(&cube);
    return 0;
}

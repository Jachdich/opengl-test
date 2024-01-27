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
#include "../include/shaders.h"
#include <json-c/json.h>

float vertices[] = {
     1.0f,  1.0f, 0.0f,
     1.0f, -1.0f, 0.0f,
    -1.0f, -1.0f, 0.0f,
    -1.0f,  1.0f, 0.0f, 
};
unsigned int indices[] = {
    0, 1, 3,
    1, 2, 3,
};

struct Vertex {
    vec3 pos;
    vec2 uv;
    vec3 normal;
};
typedef struct Vertex Vertex;

struct Texture {
    uint32_t id;
};
typedef struct Texture Texture;

double lastxpos = 0;
double lastypos = 0;

int winwidth = 1920;
int winheight = 1080;

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
}

void mouse_callback(GLFWwindow*, double xpos, double ypos) {
    double deltax = xpos - lastxpos;
    double deltay = lastypos - ypos;

    
    lastxpos = xpos;
    lastypos = ypos;
}

Texture tex_init(const char *fname) {
    Texture tex;
    glGenTextures(1, &tex.id);
    
    int width, height, nrChannels;
    printf("Loading texture '%s'...", fname);
    uint8_t *data = stbi_load(fname, &width, &height, &nrChannels, 0); 
    printf(" Got %p\n", data);
    glBindTexture(GL_TEXTURE_2D, tex.id);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    return tex;
}

void tex_free(Texture *tex) {
    glDeleteTextures(1, &tex->id);
}

Texture *load_textures(const char **fnames, size_t n) {
    Texture *texs = malloc(sizeof(*texs) * n);
    for (size_t i = 0; i < n; i++) {
        texs[i] = tex_init(fnames[i]);
    }

    return texs;
}

void free_textures(Texture *texs, size_t n) {
    for (size_t i = 0; i < n; i++) {
        tex_free(texs + i);
    }
    free(texs);
}

int main() {
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

    glfwSetCursorPosCallback(window, mouse_callback);  

    float totalTime = 0;
    float lastNow = glfwGetTime();
    int frames = 0;


    uint32_t vbo, ebo;
    uint32_t vao;
    glGenBuffers(1, &vbo);
    glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);
    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, 3 * sizeof(float) * 4, vertices, GL_STATIC_DRAW);

    glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    uint32_t shader = make_shader_program("shaders/vertex.glsl", "shaders/frag.glsl");
    glUseProgram(shader);

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

        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        glUseProgram(shader);
        glBindVertexArray(vao);

        glUniform2f(glGetUniformLocation(shader, "resolution"), (float)winwidth, (float)winheight);

        glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        frames++;
    }

    glfwTerminate();
    
    return 0;
}

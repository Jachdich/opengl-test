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

// float vertices[] = {
    // -0.5f, -0.5f, 0.0f,   0.0f, 0.0f, 1.0f,   0.0f, 0.0f,
     // 0.5f, -0.5f, 0.0f,   0.0f, 1.0f, 1.0f,   1.0f, 0.0f,
    // -0.5f,  0.5f, 0.0f,   1.0f, 0.0f, 0.0f,   0.0f, 1.0f,
     // 0.5f,  0.5f, 0.0f,   1.0f, 1.0f, 0.0f,   1.0f, 1.0f,
// };

Camera cam;

float vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,   0.0f,  0.0f, -1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f,  0.0f, -1.0f,
     
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,   0.0f,  0.0f, -1.0f, 

    
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.0f,  0.0f, 1.0f,
     
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,   0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,   0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  0.0f, 1.0f,

    
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
    
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,  -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,  -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,  -1.0f,  0.0f,  0.0f,

    
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
     
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   1.0f,  0.0f,  0.0f,

//bottom
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,   0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, -1.0f,  0.0f,

     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,   0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,   0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,   0.0f, -1.0f,  0.0f,

//top
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,   0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,   0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,   0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,   0.0f,  1.0f,  0.0f,
};

vec3 cubes[] = {
    (vec3){0, 0, 0},
//    (vec3){-1, -1, 0},
//    (vec3){1, 0, -8}
};

vec3 lightPos = {1.2, 1.0, 2.0};

/*
float vertices[] = {
    1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
    1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
   -1.0f, -1.0f, 0.0f, 0.0f, 0.0f,
    1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
   -1.0f,  1.0f, 0.0f, 0.0f, 0.0f,
};*/


/*
int32_t indices[] = {
    0, 1, 2,
    1, 2, 3
};

float texture_coords[] = {
    0.0f, 0.0f,
    1.0f, 0.0f,
    0.5f, 1.0f,
};*/

bool wireframe = false;
bool edge = true;

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
    } if (glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS) {
        if (edge) {
            edge = false;
            wireframe = !wireframe;
            if (wireframe) {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            } else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
        }
    } else {
        edge = true;
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

struct VertexObject {
    uint32_t vbo, vao;
    uint32_t shader;
    uint32_t *textures;
};

typedef struct VertexObject VertexObject;

VertexObject vobj_init(float *vertices, size_t num_vertices, const char *vertfile, const char *fragfile, const char **texture_files, size_t num_textures) {
    VertexObject obj;
    
    glGenBuffers(1, &obj.vbo);
    glGenVertexArrays(1, &obj.vao);

    glBindVertexArray(obj.vao);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
    glBufferData(GL_ARRAY_BUFFER, num_vertices, vertices, GL_STATIC_DRAW);    

    //pos component of vbo
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), NULL);
    glEnableVertexAttribArray(0);

    //uv coord component of vbo
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    //normal component of vbo
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(2);
    
    obj.shader = make_shader_program(vertfile, fragfile);
    glUseProgram(obj.shader);

    obj.textures = malloc(sizeof(*obj.textures) * num_textures);
    glGenTextures(num_textures, obj.textures);

    int width, height, nrChannels;
    for (size_t i = 0; i < num_textures; i++) {
        printf("Loading texture '%s'...", texture_files[i]);
        uint8_t *data = stbi_load(texture_files[i], &width, &height, &nrChannels, 0); 
        printf(" Got %p\n", data);
        glBindTexture(GL_TEXTURE_2D, obj.textures[i]);
        glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
        glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
        glGenerateMipmap(GL_TEXTURE_2D);
        stbi_image_free(data);
    }

    return obj;
}

void vobj_delete(VertexObject *vobj) {
    glDeleteVertexArrays(1, &vobj->vao);
    glDeleteBuffers(1, &vobj->vbo);
    glDeleteProgram(vobj->shader);
    free(vobj->textures);
}

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

    VertexObject cube = vobj_init(vertices, sizeof(vertices), "shaders/vertex.glsl", "shaders/frag.glsl", (const char*[]){"container.png", "awesomeface.png"}, 2);
    VertexObject light = vobj_init(vertices, sizeof(vertices), "shaders/vertex.glsl", "shaders/lightfrag.glsl", NULL, 0);
    
    /*data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);*/

    float totalTime = 0;
    float lastNow = glfwGetTime();
    int frames = 0;

    while (!glfwWindowShouldClose(window)) {

        float now = glfwGetTime();
        float delta_time = now - lastNow;
        lastNow = now;

        totalTime += delta_time;
        if (totalTime > 1) {
            printf("%f\n", frames / totalTime);
            totalTime = 0;
            frames = 0;
        }

        process_input(window);

        //glClearColor(0.2f, 0.9f, 1.0f, 1.0f);
        glClearColor(0.0f, 0.0f, 0.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

        vec3 colour = {1, 1, 1};
        
        mat4 view = GLM_MAT4_IDENTITY_INIT;
        mat4 projection = GLM_MAT4_IDENTITY_INIT;

        glm_translate(view, (vec3){0, 0, -3});
        glm_perspective(GLM_PI/2, (float)winwidth / (float)winheight, 0.1, 100, projection);

        cam_update(&cam, delta_time);
        cam_view_matrix(&cam, view);

        glUseProgram(cube.shader);
        glUniform1i(glGetUniformLocation(cube.shader, "tex1"), 0);
        glUniform1i(glGetUniformLocation(cube.shader, "tex2"), 1);
        glUniformMatrix4fv(glGetUniformLocation(cube.shader, "view"), 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(cube.shader, "projection"), 1, GL_FALSE, (float*)projection);
        glUniform3fv(glGetUniformLocation(cube.shader, "lightColour"), 1, (float*)colour);
        glUniform3fv(glGetUniformLocation(cube.shader, "lightPos"), 1, (float*)lightPos);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube.textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cube.textures[1]);
        
        glBindVertexArray(cube.vao);
        for (uint32_t i = 0; i < sizeof(cubes) / sizeof(vec3); i++) {
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(model, cubes[i]);
            //glm_rotate(model, glfwGetTime() * 0.9f, (vec3){0.5f, 1.0f, 0.0f});
            glUniformMatrix4fv(glGetUniformLocation(cube.shader, "model"), 1, GL_FALSE, (float*)model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }

        glUseProgram(light.shader);
        glUniformMatrix4fv(glGetUniformLocation(light.shader, "view"), 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(light.shader, "projection"), 1, GL_FALSE, (float*)projection);
        glUniform3fv(glGetUniformLocation(light.shader, "colour"), 1, (float*)colour);

        glBindVertexArray(light.vao);
        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glm_translate(model, lightPos);
        glm_scale(model, (vec3){0.3, 0.3, 0.3});
        glUniformMatrix4fv(glGetUniformLocation(light.shader, "model"), 1, GL_FALSE, (float*)model);
        glDrawArrays(GL_TRIANGLES, 0, 36);
                            
        glfwSwapBuffers(window);
        glfwPollEvents();
        frames++;
    }

    glfwTerminate();
    
    vobj_delete(&cube);
    return 0;
}

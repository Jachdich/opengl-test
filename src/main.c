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
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 0.0f,

    
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
     
     0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,

    
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

    
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     
     0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

//bottom
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,

     0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, 1.0f,

//top
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f, 1.0f
};

vec3 cubes[] = {
    (vec3){0, 1, 0},
//    (vec3){-1, -1, 0},
//    (vec3){1, 0, -8}
};

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
    
    uint32_t vbo, vao, light_vao;//, ebo;
    glGenBuffers(1, &vbo);
    //glGenBuffers(1, &ebo);
    glGenVertexArrays(1, &vao);
    glGenVertexArrays(1, &light_vao);

    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);    

    //pos component of vbo
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5*sizeof(float), NULL);
    glEnableVertexAttribArray(0);
    // //colour component of vbo
    // glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8*sizeof(float), (void*)(3*sizeof(float)));
    // glEnableVertexAttribArray(1);
    //uv coord component of vbo
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5*sizeof(float), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);
    
    //initialise the EBO
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW); 

    uint32_t shader_program = make_shader_program("shaders/vertex.glsl", "shaders/frag.glsl");
    glUseProgram(shader_program);

//    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);


    int width, height, nrChannels;
    uint8_t *data = stbi_load("dirt.png", &width, &height, &nrChannels, 0); 
    uint32_t textures[2];
    glGenTextures(2, textures);
    
    glBindTexture(GL_TEXTURE_2D, textures[0]);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);
    
    data = stbi_load("awesomeface.png", &width, &height, &nrChannels, 0);
    glBindTexture(GL_TEXTURE_2D, textures[1]);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
    glGenerateMipmap(GL_TEXTURE_2D);
    stbi_image_free(data);

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

        glClearColor(0.2f, 0.9f, 1.0f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

/*
        mat4 trans = GLM_MAT4_IDENTITY_INIT;
        
        glm_translate(trans, (vec3){0.5f, -0.5f, 0.0f});
        glm_rotate(trans, angle, (vec3){0, 0, 1});
        //glm_scale(trans, (vec3){0.5, 0.5, 0.5});*/    

        //vec3 colour = {(sin(glfwGetTime()) + 1) / 2, 0.5f, 0.5f};
        vec3 colour = {1, 1, 1};
        
        mat4 view = GLM_MAT4_IDENTITY_INIT;
        mat4 projection = GLM_MAT4_IDENTITY_INIT;

        glm_translate(view, (vec3){0, 0, -3});
        glm_perspective(GLM_PI/2, (float)winwidth / (float)winheight, 0.1, 100, projection);

        cam_update(&cam, delta_time);
        cam_view_matrix(&cam, view);

        glUseProgram(shader_program);
        glUniform1i(glGetUniformLocation(shader_program, "tex1"), 0);
        glUniform1i(glGetUniformLocation(shader_program, "tex2"), 1);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "view"), 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(shader_program, "projection"), 1, GL_FALSE, (float*)projection);
        glUniform3fv(glGetUniformLocation(shader_program, "colour"), 1, (float*)colour);

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, textures[0]);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, textures[1]);
        
        glBindVertexArray(vao);
        //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, ebo);
        //glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);
        for (uint32_t i = 0; i < sizeof(cubes) / sizeof(vec3); i++) {
            mat4 model = GLM_MAT4_IDENTITY_INIT;
            glm_translate(model, cubes[i]);
            //glm_rotate(model, glfwGetTime() * 0.9f, (vec3){0.5f, 1.0f, 0.0f});
            glUniformMatrix4fv(glGetUniformLocation(shader_program, "model"), 1, GL_FALSE, (float*)model);
            glDrawArrays(GL_TRIANGLES, 0, 36);
        }
        
        glfwSwapBuffers(window);
        glfwPollEvents();
        frames++;
    }

    glfwTerminate();
    
    glDeleteVertexArrays(1, &vao);
    glDeleteBuffers(1, &vbo);
    //glDeleteBuffers(1, &ebo);
    glDeleteProgram(shader_program);

    return 0;
}

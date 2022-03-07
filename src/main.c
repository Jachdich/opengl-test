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

enum TexType {
    TEX_DIFFUSE,
    TEX_SPECULAR,
};
typedef enum TexType TexType;

struct Vertex {
    vec3 pos;
    vec2 uv;
    vec3 norm;
};
typedef struct Vertex Vertex;

struct Texture {
    uint32_t id;
    TexType type;
};
typedef struct Texture Texture;

struct Mesh {
    uint32_t vbo, vao, ebo;
    uint32_t shader;
    Texture *textures;
    size_t num_textures;
    Vertex *vertices;
};
typedef struct Mesh Mesh;


Vertex vertices[] = {
   {{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f},  { 0.0f,  0.0f, -1.0f}},
   {{ 0.5f, -0.5f, -0.5f},  {1.0f, 0.0f},  { 0.0f,  0.0f, -1.0f}},
   {{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f},  { 0.0f,  0.0f, -1.0f}},
   {{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f},  { 0.0f,  0.0f, -1.0f}}, 
   {{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f},  { 0.0f,  0.0f, -1.0f}}, 
   {{-0.5f, -0.5f, -0.5f},  {0.0f, 0.0f},  { 0.0f,  0.0f, -1.0f}}, 
   {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f},  { 0.0f,  0.0f, 1.0f}},
   {{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f},  { 0.0f,  0.0f, 1.0f}},
   {{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f},  { 0.0f,  0.0f, 1.0f}},
   {{ 0.5f,  0.5f,  0.5f},  {1.0f, 1.0f},  { 0.0f,  0.0f, 1.0f}},
   {{-0.5f,  0.5f,  0.5f},  {0.0f, 1.0f},  { 0.0f,  0.0f, 1.0f}},
   {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f},  { 0.0f,  0.0f, 1.0f}},
   {{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f},  {-1.0f,  0.0f,  0.0f}},
   {{-0.5f,  0.5f, -0.5f},  {1.0f, 1.0f},  {-1.0f,  0.0f,  0.0f}},
   {{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f},  {-1.0f,  0.0f,  0.0f}},
   {{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f},  {-1.0f,  0.0f,  0.0f}},
   {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f},  {-1.0f,  0.0f,  0.0f}},
   {{-0.5f,  0.5f,  0.5f},  {1.0f, 0.0f},  {-1.0f,  0.0f,  0.0f}},
   {{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f},  { 1.0f,  0.0f,  0.0f}},
   {{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f},  { 1.0f,  0.0f,  0.0f}},
   {{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f},  { 1.0f,  0.0f,  0.0f}},
   {{ 0.5f, -0.5f, -0.5f},  {0.0f, 1.0f},  { 1.0f,  0.0f,  0.0f}},
   {{ 0.5f, -0.5f,  0.5f},  {0.0f, 0.0f},  { 1.0f,  0.0f,  0.0f}},
   {{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f},  { 1.0f,  0.0f,  0.0f}},
   {{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f},  { 0.0f, -1.0f,  0.0f}},
   {{ 0.5f, -0.5f, -0.5f},  {1.0f, 1.0f},  { 0.0f, -1.0f,  0.0f}},
   {{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f},  { 0.0f, -1.0f,  0.0f}},
   {{ 0.5f, -0.5f,  0.5f},  {1.0f, 0.0f},  { 0.0f, -1.0f,  0.0f}},
   {{-0.5f, -0.5f,  0.5f},  {0.0f, 0.0f},  { 0.0f, -1.0f,  0.0f}},
   {{-0.5f, -0.5f, -0.5f},  {0.0f, 1.0f},  { 0.0f, -1.0f,  0.0f}},
   {{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f},  { 0.0f,  1.0f,  0.0f}},
   {{ 0.5f,  0.5f, -0.5f},  {1.0f, 1.0f},  { 0.0f,  1.0f,  0.0f}},
   {{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f},  { 0.0f,  1.0f,  0.0f}},
   {{ 0.5f,  0.5f,  0.5f},  {1.0f, 0.0f},  { 0.0f,  1.0f,  0.0f}},
   {{-0.5f,  0.5f,  0.5f},  {0.0f, 0.0f},  { 0.0f,  1.0f,  0.0f}},
   {{-0.5f,  0.5f, -0.5f},  {0.0f, 1.0f},  { 0.0f,  1.0f,  0.0f}},
};

vec3 cubes[1000] = {
    (vec3){0, 0, 0},
    (vec3){-1, -1, 0},
    (vec3){1, 0, -8}
};

int n_cubes = 3;

void add_random_block() {
    cubes[n_cubes][0] = rand() % 20 - 10;
    cubes[n_cubes][1] = rand() % 20 - 10;
    cubes[n_cubes][2] = rand() % 20 - 10;

    n_cubes++;
}


vec3 lightPos = {1.2, 1.0, 2.0};

bool wireframe = false;
bool edge = true;
bool edge2 = true;

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

    if (glfwGetKey(win, GLFW_KEY_P) == GLFW_PRESS) {
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

    if (glfwGetKey(win, GLFW_KEY_N) == GLFW_PRESS) {
        if (edge2) {
            edge2 = false;
            cubes[n_cubes][0] = (int)cam.pos[0];
            cubes[n_cubes][1] = (int)cam.pos[1];
            cubes[n_cubes][2] = (int)cam.pos[2];

            n_cubes++;
        }
    } else {
        edge2 = true;
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

Texture tex_init(const char *fname, const TexType type) {
    Texture tex;
    glGenTextures(1, &tex.id);
    tex.type = type;
    
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

Texture *load_textures(const char **fnames, const TexType *types, size_t n) {
    Texture *texs = malloc(sizeof(*texs) * n);
    for (size_t i = 0; i < n; i++) {
        texs[i] = tex_init(fnames[i], types[i]);
    }

    return texs;
}

void free_textures(Texture *texs, size_t n) {
    for (size_t i = 0; i < n; i++) {
        tex_free(texs + i);
    }
    free(texs);
}

Mesh mesh_init(Vertex *vertices, size_t num_vertices, const char *vertfile, const char *fragfile, Texture *textures, size_t num_textures) {
    Mesh obj;
    
    glGenBuffers(1, &obj.vbo);
    glGenVertexArrays(1, &obj.vao);

    glBindVertexArray(obj.vao);

    glBindBuffer(GL_ARRAY_BUFFER, obj.vbo);
    glBufferData(GL_ARRAY_BUFFER, num_vertices * sizeof(Vertex), vertices, GL_STATIC_DRAW);    

    //pos component of vbo
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), NULL);
    glEnableVertexAttribArray(0);

    //uv coord component of vbo
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    //normal component of vbo
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(2);
    
    obj.shader = make_shader_program(vertfile, fragfile);
    glUseProgram(obj.shader);

    obj.textures = textures;
    obj.num_textures = num_textures;

    return obj;
}

void mesh_delete(Mesh *obj) {
    glDeleteVertexArrays(1, &obj->vao);
    glDeleteBuffers(1, &obj->vbo);
    glDeleteBuffers(1, &obj->ebo);
    glDeleteProgram(obj->shader);
}

void mesh_draw(Mesh *mesh, mat4 view, mat4 projection) {
    glUseProgram(mesh->shader);
    glUniform1i(glGetUniformLocation(mesh->shader, "tex1"), 0);
    glUniformMatrix4fv(glGetUniformLocation(mesh->shader, "view"), 1, GL_FALSE, (float*)view);
    glUniformMatrix4fv(glGetUniformLocation(mesh->shader, "projection"), 1, GL_FALSE, (float*)projection);
    glUniform3fv(glGetUniformLocation(mesh->shader, "viewPos"), 1, (float*)cam.pos);
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

    Texture *textures = load_textures((const char*[]){"container.png", "container2.png", "container2_specular.png"}, (const TexType[]){TEX_DIFFUSE, TEX_DIFFUSE, TEX_SPECULAR}, 3);

    Mesh cube  = mesh_init(vertices, sizeof(vertices) / sizeof(Vertex), "shaders/vertex.glsl", "shaders/frag.glsl", textures, 3);
    Mesh light = mesh_init(vertices, sizeof(vertices) / sizeof(Vertex), "shaders/vertex.glsl", "shaders/lightfrag.glsl", NULL, 0);
    
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
        if (totalTime > 0.1) {
            printf("%f\n", frames / totalTime);
            totalTime = 0;
            frames = 0;
            add_random_block();
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
        
        glUniform3fv(glGetUniformLocation(cube.shader, "light.position"), 1, (float*)lightPos);
        glUniform3f(glGetUniformLocation(cube.shader,  "light.ambient"),  0.1f, 0.1f, 0.1f);
        glUniform3f(glGetUniformLocation(cube.shader,  "light.diffuse"),  1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(cube.shader,  "light.specular"), 1.0f, 1.0f, 1.0f); 
        glUniform1f(glGetUniformLocation(cube.shader,  "light.linear"),   0.09f);
        glUniform1f(glGetUniformLocation(cube.shader,  "light.quadratic"),0.032f); 

        glUniform3f(glGetUniformLocation(cube.shader, "dLight.direction"), -0.5345224838248488f, -0.8017837257372731f, -0.2672612419124244f);
        glUniform3f(glGetUniformLocation(cube.shader, "dLight.ambient"),  0.3f, 0.3f, 0.3f);
        glUniform3f(glGetUniformLocation(cube.shader, "dLight.diffuse"),  1.0f, 1.0f, 1.0f);
        glUniform3f(glGetUniformLocation(cube.shader, "dLight.specular"), 1.0f, 1.0f, 1.0f); 

        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube.textures[0].id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cube.textures[1].id);
        glActiveTexture(GL_TEXTURE2);
        glBindTexture(GL_TEXTURE_2D, cube.textures[2].id);
        
        glBindVertexArray(cube.vao);
        for (int32_t i = 0; i < n_cubes; i++) {
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
    
    mesh_delete(&cube);
    return 0;
}

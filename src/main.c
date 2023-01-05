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
#include <json-c/json.h>

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
    vec3 normal;
};
typedef struct Vertex Vertex;

struct Texture {
    uint32_t id;
    TexType type;
};
typedef struct Texture Texture;

struct Mesh {
    uint32_t vbo, vao, ebo, map;
    uint32_t shader;
    Texture *textures;
    size_t num_textures;
    size_t num_vertices;
    size_t num_indices;
    Vertex *vertices;
    int *indices;
    uint32_t rad;
};
typedef struct Mesh Mesh;

bool wireframe = false;
bool edge = true;
bool edge2 = true;
Mesh cube;

int winwidth = 1920;
int winheight = 1080;

int star = 1, planet = 0;


void fb_size_callback(GLFWwindow *win, int w, int h) {
    (void)win;
    glViewport(0, 0, w, h);
    winwidth = w;
    winheight = h;
}

Mesh mesh_surface(int star, int planet);

void process_input(GLFWwindow *win) {
    if (glfwGetKey(win, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(win, true);
    }
    if (glfwGetKey(win, GLFW_KEY_N) == GLFW_PRESS) {
        planet += 1;
        planet %= 4;
        cube = mesh_surface(star, planet);
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

Mesh mesh_init(Vertex *vertices, size_t num_vertices, 
               int *indices, size_t num_indices,
               const char *vertfile, const char *fragfile,
               Texture *textures, size_t num_textures) {
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

    //uv coord component of vbo
    glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(3*sizeof(float)));
    glEnableVertexAttribArray(1);

    // normal component of vbo
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), (void*)(5*sizeof(float)));
    glEnableVertexAttribArray(2);
    
    obj.shader = make_shader_program(vertfile, fragfile);
    glUseProgram(obj.shader);

    obj.textures = textures;
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

int get_tiles(int star, int planet, uint64_t **vals) {
    json_object *root = json_object_from_file("s0.0.json");
    json_object *stars_arr = json_object_object_get(root, "stars");
    json_object *star_ob = json_object_array_get_idx(stars_arr, star);
    json_object *planets_arr = json_object_object_get(star_ob, "planets");
    json_object *planet_ob = json_object_array_get_idx(planets_arr, planet);
    json_object *surface_ob = json_object_object_get(planet_ob, "surface");
    json_object *tiles_ob = json_object_object_get(surface_ob, "tiles");

    uint32_t rad = json_object_get_int(json_object_object_get(planet_ob, "radius")) * 2;
    *vals = malloc(sizeof(*vals) * rad * rad);
    for (uint32_t i = 0; i < rad * rad; i++) {
        uint64_t val = json_object_get_uint64(json_object_array_get_idx(tiles_ob, i));
        (*vals)[i] = val;
    }
    json_object_put(stars_arr);
    return rad;
}

#define TYPE(tile) (int32_t)(tile & 0xFFFFFFFF)
#define HEIGHT(tile) ((int32_t)((tile >> 32) & 0xFFFFFFFF) / 3.0)

double lerp(double a, double b, double p) {
    return a + (b - a) * p;
}

typedef struct {
    Vertex *a, *b, *c, *d;
} Face;

Face make_square(vec3 pos, vec2 size, Vertex *vertices, int *indices, int *num_vertices, int *num_indices) {
    float w = size[0];
    float t = size[1];
    float h = size[2];
    indices[(*num_indices)++] = *num_vertices;
    glm_vec2_copy((vec2){0, 0}, vertices[*num_vertices].uv);
    glm_vec3_copy(pos,  vertices[(*num_vertices)++].pos);

    uint32_t i1 = *num_vertices;
    indices[(*num_indices)++] = *num_vertices;
    glm_vec2_copy((vec2){0, 1 / 4.0}, vertices[*num_vertices].uv);
    glm_vec3_copy(pos, vertices[(*num_vertices)++].pos);
    glm_vec3_add((vec3){w, 0, 0}, vertices[*num_vertices - 1].pos, vertices[*num_vertices - 1].pos);
 
    uint32_t i2 = *num_vertices;
    indices[(*num_indices)++] = *num_vertices;
    glm_vec2_copy((vec2){1 / 4.0, 0}, vertices[*num_vertices].uv);
    glm_vec3_copy(pos, vertices[(*num_vertices)++].pos);
    glm_vec3_add((vec3){0, t, h}, vertices[*num_vertices - 1].pos, vertices[*num_vertices - 1].pos);

    indices[(*num_indices)++] = i1;
    indices[(*num_indices)++] = i2;
    indices[(*num_indices)++] = *num_vertices;
    glm_vec2_copy((vec2){1 / 4.0, 1 / 4.0}, vertices[*num_vertices].uv);
    glm_vec3_copy(pos, vertices[(*num_vertices)++].pos);
    glm_vec3_add((vec3){w, t, h}, vertices[*num_vertices - 1].pos, vertices[*num_vertices - 1].pos);

    return (Face){ vertices + *num_vertices - 2, //don't ask about the order
                   vertices + *num_vertices - 3, //it's to do with the cross product :skull:
                   vertices + *num_vertices - 4,
                   vertices + *num_vertices - 1};
}

uint32_t gen_mesh_from_tiles(uint64_t *tiles, uint32_t rad, Vertex *vertices, int *indices, int *num_vertices, int *num_indices) {
    uint8_t *map = malloc(sizeof(*map) * rad * rad * 4);
    memset(map, 0, rad * rad * 4);
    Face *faces = malloc(1000000); //TODO a lot
    uint32_t fnum = 0;
    
    for (uint32_t y = 1; y < rad; y++) {
        for (uint32_t x = 0; x < rad; x++) {
            int32_t ya = y / 2 - rad;
            int32_t xa = x / 2 - rad;
            if (ya * ya + xa * xa > (rad) * (rad)) {
                // continue;
            }
            uint32_t type = TYPE(tiles[y * rad + x]);
            vec3 tile  = {x, HEIGHT(tiles[y * rad + x]), y};
            float dhx = HEIGHT(tiles[(y + 1) * rad + x]) - HEIGHT(tiles[y * rad + x]);
            float dhz = HEIGHT(tiles[y * rad + x + 1]) - HEIGHT(tiles[y * rad + x]);
            // vec3 tiley1 = {x, lerp(tile[1], HEIGHT(tiles[(y + 1) / 2 * rad + x/2]), 0.5), y + 1};
            
            faces[fnum++] = make_square(tile, (vec3){0.8f, 0.0f, 0.8f}, vertices, indices, num_vertices, num_indices);
            glm_vec3_add(tile, (vec3){0.0f, 0.0f, 0.8f}, tile);
            faces[fnum++] = make_square(tile, (vec3){0.8f, dhx, 0.2f}, vertices, indices, num_vertices, num_indices);
            glm_vec3_add(tile, (vec3){0.8f, 0.0f, -0.8f}, tile);
            faces[fnum++] = make_square(tile, (vec3){0.2f, dhz, 0.8f}, vertices, indices, num_vertices, num_indices);
            map[(y * rad + x) * 4] = type;
        }
        // indices[(*num_indices)++] = 65535;
    }
    
    for (uint32_t i = 0; i < fnum; i++) {
        vec3 e1, e2;
        glm_vec3_sub(faces[i].a->pos, faces[i].b->pos, e1);
        glm_vec3_sub(faces[i].c->pos, faces[i].b->pos, e2);
        vec3 no;
        glm_vec3_cross(e1, e2, no);
        
        glm_vec3_copy(no, faces[i].a->normal);
        glm_vec3_copy(no, faces[i].b->normal);
        glm_vec3_copy(no, faces[i].c->normal);
        glm_vec3_copy(no, faces[i].d->normal);
    }
    
    
    uint32_t texid;
    glGenTextures(1, &texid);
    glActiveTexture(GL_TEXTURE1);
    glBindTexture(GL_TEXTURE_2D, texid);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGBA, rad, rad, 0, GL_RGBA, GL_UNSIGNED_BYTE, map);
    glGenerateMipmap(GL_TEXTURE_2D);
    free(map);
    free(faces);
    return texid;
}

Mesh mesh_surface(int star, int planet) {
    int num_vertices = 0;
    int num_indices = 0;
    Vertex *vertices = malloc(sizeof(*vertices) * 1000000); //allocate way too much (test)
    int *indices = malloc(sizeof(*indices) * 1000000);

    uint64_t *vals;
    uint32_t rad = get_tiles(star, planet, &vals);
    uint32_t map = gen_mesh_from_tiles(vals, rad, vertices, indices, &num_vertices, &num_indices);
    Texture *textures = load_textures((const char*[]){"tilemap.png"}, (const TexType[]){TEX_DIFFUSE}, 1);

    Mesh cube = mesh_init(vertices, num_vertices, indices, num_indices, "shaders/vertex.glsl", "shaders/frag.glsl", textures, 1);
    cube.map = map;
    cube.rad = rad;
    return cube;
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
    glEnable(GL_PRIMITIVE_RESTART);
    glPrimitiveRestartIndex(65535);
    
    cube = mesh_surface(star, planet);
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
        cam_view_matrix(&cam, view);
        
        glBindVertexArray(cube.vao);
        mat4 model = GLM_MAT4_IDENTITY_INIT;
        glUseProgram(cube.shader);
        glUniformMatrix4fv(glGetUniformLocation(cube.shader, "view"), 1, GL_FALSE, (float*)view);
        glUniformMatrix4fv(glGetUniformLocation(cube.shader, "projection"), 1, GL_FALSE, (float*)projection);
        glUniform3fv(glGetUniformLocation(cube.shader, "viewPos"), 1, (float*)cam.pos);
        glUniformMatrix4fv(glGetUniformLocation(cube.shader, "model"), 1, GL_FALSE, (float*)model);

        glUniform1i(glGetUniformLocation(cube.shader, "tex1"), 0);
        glUniform1i(glGetUniformLocation(cube.shader, "map"), 1);
        glUniform1i(glGetUniformLocation(cube.shader, "rad"), cube.rad);
        glActiveTexture(GL_TEXTURE0);
        glBindTexture(GL_TEXTURE_2D, cube.textures[0].id);
        glActiveTexture(GL_TEXTURE1);
        glBindTexture(GL_TEXTURE_2D, cube.map);

        glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, cube.ebo);
        glDrawElements(GL_TRIANGLES, cube.num_vertices, GL_UNSIGNED_INT, 0);

        glfwSwapBuffers(window);
        glfwPollEvents();
        frames++;
    }

    glfwTerminate();
    
    mesh_delete(&cube);
    return 0;
}

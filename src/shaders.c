#include <stdio.h>
#include <stdlib.h>
#include "../include/shaders.h"
#include "../include/glad/glad.h"

char *read_file(const char *fname) {
    FILE *fp = fopen(fname, "r");
    if (fp == NULL) {
        return NULL;
    }
    fseek(fp, 0, SEEK_END);
    size_t fsize = ftell(fp);
    fseek(fp, 0, SEEK_SET);

    char *contents = malloc(fsize + 1);
    fread(contents, 1, fsize, fp);
    contents[fsize] = 0;
    fclose(fp);
    return contents;
}

uint32_t make_shader(const char *fname, int shader_type) {
    char *shader_source = read_file(fname);

    uint32_t shader = glCreateShader(shader_type);
    
    glShaderSource(shader, 1, (const char **)&shader_source, NULL);
    glCompileShader(shader);

    int success;
    char info[512];

    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if (!success) {
        glGetShaderInfoLog(shader, 512, NULL, info);
        fprintf(stderr, "Shader compile failed! Traceback:\n%s\n", info);
        return 0;
    }

    free(shader_source);

    return shader;
}

uint32_t make_shader_program(const char *vertex_path, const char *frag_path) {
    uint32_t frag_shader = make_shader(frag_path,  GL_FRAGMENT_SHADER);
    uint32_t vertex_shader = make_shader(vertex_path, GL_VERTEX_SHADER);

    if (frag_shader == 0 || vertex_shader == 0) {
        return 0;
    }
    
    uint32_t shader_program = glCreateProgram();
    glAttachShader(shader_program, vertex_shader);
    glAttachShader(shader_program, frag_shader);
    glLinkProgram(shader_program);

    int success;
    char info[512];
    
    glGetProgramiv(shader_program, GL_LINK_STATUS, &success);
    if (!success) {
        glGetProgramInfoLog(shader_program, 512, NULL, info);
        fprintf(stderr, "Shader program link failed! Traceback:\n%s\n", info);
    }
    glDeleteShader(vertex_shader);
    glDeleteShader(frag_shader);
    
    return shader_program;
}

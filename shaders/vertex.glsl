#version 330 core
layout (location = 0) in vec3 aPos;

in vec2 resolution;
out vec3 fragPos;


void main() {
    fragPos = aPos;
    gl_Position = vec4(fragPos, 1.0);
}

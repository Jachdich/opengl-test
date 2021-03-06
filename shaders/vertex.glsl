#version 330 core
layout (location = 0) in vec3 aPos;
layout (location = 1) in vec2 uv;
layout (location = 2) in vec3 aNormal;
out vec2 texCoord;
out vec3 normal;
out vec3 fragPos;

uniform mat4 model;
uniform mat4 view;
uniform mat4 projection;

void main() {
    fragPos = vec3(model * vec4(aPos, 1.0));
    texCoord = uv;
    //TODO will not work with unequal scaling, but that sounds hard to deal with
    normal = mat3(model) * aNormal;
    gl_Position = projection * view * vec4(fragPos, 1.0);
}

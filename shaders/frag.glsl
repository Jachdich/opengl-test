#version 330 core

out vec4 FragColor;
in vec2 texCoord;
in vec3 fragPos;

uniform sampler2D tex1;
uniform vec3 viewPos;

void main() {
    FragColor = texture(tex1, texCoord);
}

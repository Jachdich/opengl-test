#version 330 core

out vec4 FragColor;
in vec2 texCoord;
in vec3 fragPos;
in vec3 normal;

uniform sampler2D tex1;
uniform vec3 viewPos;
uniform sampler2D map;
uniform int rad;

void main() {
    vec4 ty = texture(map, fragPos.xz / rad / 2);
    // FragColor = texture(tex1, texCoord + vec2(ty.x * 8, 0));
    FragColor = vec4(normalize(-normal), 1.0);
    
    // FragColor = vec4(ty.xyw, 1.0);
    // FragColor = vec4(fragPos.xz / rad / 2, 0.0, 1.0);
    // FragColor = vec4(1.0, 1.0, 1.0, 1.0);
}

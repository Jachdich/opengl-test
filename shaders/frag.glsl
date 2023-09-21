#version 330 core

out vec4 FragColor;
in vec3 fragPos;

uniform vec2 resolution;
uniform vec3 camLook;
uniform vec3 viewPos;

float smin(float a, float b, float k) {
  float h = clamp(0.5 + 0.5 * (b - a) / k, 0.0, 1.0);
  return mix(b, a, h) - k * h * (1.0 - h);
}

float dist(vec3 pos) {
    
    float cube = length(max(abs(pos - vec3(0.7, 0.5, 3.0))-vec3(0.1, 0.1, 0.1),0.0));
    
    vec3 d = pos - vec3(0.6, 0.5, 2.95);
    vec3 e = pos - vec3(sin(0.0) / 2.4 + 1.3, 0.5, 3);
    //return smin(min(length(d) - 0.1, (length(e) - 0.1)/*, sin(iTime) / 2.0*/), cube, 0.2);
    return max(cube, -(length(d) - 0.1));
}

vec3 normal(vec3 pos) {
    const highp float epsilon = 0.0001;
    float centre = dist(pos);
    float x = dist(pos + vec3(epsilon, 0, 0));
    float y = dist(pos + vec3(0, epsilon, 0));
    float z = dist(pos + vec3(0, 0, epsilon));
    float nx = dist(pos - vec3(epsilon, 0, 0));
    float ny = dist(pos - vec3(0, epsilon, 0));
    float nz = dist(pos - vec3(0, 0, epsilon));
    return normalize((vec3(x-nx, y-ny, z-nz) - centre));
}

void main() {
    // vec2 uv = fragPos.xy/resolution.y;
    vec2 uv = fragPos.xy;
    uv.x *= resolution.x / resolution.y;

    float norm_xz = camLook.z / camLook.x;
    float norm_xy = camLook.y / camLook.x;
    
    vec3 ray = vec3(uv, 0.0) + viewPos;
    vec3 look = normalize(camLook);
    //vec3 ray = vec3(uv + vec2(-3, -3), 0.0);
    //const vec3 look = normalize(vec3(1.0, 1.0, 1.0));
    int it;    
    const int maxIt = 100;
    for (it = 0; it < maxIt; it++) {
         float d = dist(ray);
         if (abs(d) < 0.0000005) {
             vec3 norm = normal(ray);
             float directional = max(0.0, dot(norm, normalize(vec3(0.0, 1.0, -1.0))));
             //directional += max(0.0, dot(norm, normalize(vec3(0.0, -0.5, 1.0))));
             float global = 0.2;
             float n = min(1.0, directional * 0.8 + global);
             FragColor = vec4(norm / 2.0 + vec3(0.5, 0.5, 0.5), 1.0);
             FragColor = vec4(n, n, n, 1.0);
             break;
         }
         ray += look * d;
    }


}

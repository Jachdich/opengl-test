#version 330 core
//TODO lighting in view space?

out vec4 FragColor;
in vec2 texCoord;
in vec3 normal;
in vec3 fragPos;

struct Material {
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct DirLight {
    vec3 direction;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

struct PointLight {
    vec3 position;
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float linear, quadratic;
};

uniform PointLight light;
uniform DirLight dLight;
uniform sampler2D tex1;
uniform vec3 viewPos;
uniform Material material;

vec3 point_light(PointLight pl, vec3 norm, vec3 viewDir) {
    float distance = length(pl.position - fragPos);
    float attenuation = 1.0 / (1.0 + distance * pl.linear + distance * distance * pl.quadratic);
    
    vec3 lightDir = normalize(pl.position - fragPos);
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = texture(material.diffuse, texCoord).xyz         * pl.ambient  * attenuation;
    vec3 diffuse  = texture(material.diffuse, texCoord).xyz  * diff * pl.diffuse  * attenuation;
    vec3 specular = texture(material.specular, texCoord).xyz * spec * pl.specular * attenuation;

    return ambient + diffuse + specular;
}

vec3 dir_light(DirLight dl, vec3 norm, vec3 viewDir) {
    vec3 lightDir = normalize(-dl.direction);
    
    float diff = max(dot(norm, lightDir), 0.0);

    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

    vec3 ambient  = texture(material.diffuse, texCoord).xyz         * dl.ambient;
    vec3 diffuse  = texture(material.diffuse, texCoord).xyz  * diff * dl.diffuse;
    vec3 specular = texture(material.specular, texCoord).xyz * spec * dl.specular;

    return ambient + diffuse + specular;
}


void main() {
    vec3 norm = normalize(normal);
    vec3 viewDir = normalize(viewPos - fragPos);

    FragColor = vec4(point_light(light, norm, viewDir) + dir_light(dLight, norm, viewDir), 1.0);
}

/*
vec2 poss   [10] = vec2[10](vec2(555, 6), vec2(345, 286), vec2(539, 259), vec2(442, 307), vec2(519, 94), vec2(507, 273), vec2(24, 262), vec2(409, 60), vec2(362, 283), vec2(181, 82));
vec3 colours[10] = vec3[10](vec3(0.7890625, 0.21875, 0.8359375), vec3(0.1796875, 0.0625, 0.3515625), vec3(0.87890625, 0.8515625, 0.40234375), vec3(0.34375, 0.671875, 0.8984375), vec3(0.30859375, 0.55859375, 0.37109375), vec3(0.34375, 0.60546875, 0.55078125), vec3(0.15234375, 0.30078125, 0.5078125), vec3(0.78125, 0.52734375, 0.390625), vec3(0.18359375, 0.87109375, 0.984375), vec3(0.640625, 0.0234375, 0.3671875));


void main() {
    float minDist = 1000000.0;
    int minIdx = 0;
    for (int i = 0; i < 10; i++) {
        vec2 delta = poss[i] - gl_FragCoord.xy;
        if ((delta.x * delta.x + delta.y * delta.y) < 10.3) {
            FragColor = vec4(0, 0, 0, 1);
            return;
        }
        float dist = abs(delta.x) + abs(delta.y);
        if (dist < minDist) {
            minDist = dist;
            minIdx = i;
        }
    }
    FragColor = mix(mix(texture(tex1, texCoord), texture(tex2, texCoord), 0.2) * vec4(colour, 1.0), vec4(colours[minIdx], 1.0), 0.3);
}
*/

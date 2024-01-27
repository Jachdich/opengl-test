#version 330 core

out vec4 FragColor;
in vec3 fragPos;

uniform vec2 resolution;


// A single iteration of Bob Jenkins' One-At-A-Time hashing algorithm.
uint hash( uint x ) {
    x += ( x << 10u );
    x ^= ( x >>  6u );
    x += ( x <<  3u );
    x ^= ( x >> 11u );
    x += ( x << 15u );
    return x;
}

// Construct a float with half-open range [0:1] using low 23 bits.
// All zeroes yields 0.0, all ones yields the next smallest representable value below 1.0.
float floatConstruct( uint m ) {
    const uint ieeeMantissa = 0x007FFFFFu; // binary32 mantissa bitmask
    const uint ieeeOne      = 0x3F800000u; // 1.0 in IEEE binary32

    m &= ieeeMantissa;                     // Keep only mantissa bits (fractional part)
    m |= ieeeOne;                          // Add fractional part to 1.0

    float  f = uintBitsToFloat( m );       // Range [1:2]
    return f - 1.0;                        // Range [0:1]
}



// Pseudo-random value in half-open range [0:1].
float random( float x ) { return floatConstruct(hash(floatBitsToUint(x))); }


bool point_in_tri(vec2 p, vec2 a, vec2 b, vec2 c) {
    vec3 ab = vec3(b - a, 0.0);
    vec3 bc = vec3(c - b, 0.0);
    vec3 ca = vec3(a - c, 0.0);

    vec3 ap = vec3(p - a, 0.0);
    vec3 bp = vec3(p - b, 0.0);
    vec3 cp = vec3(p - c, 0.0);
    vec3 c1 = cross(ab, ap);
    vec3 c2 = cross(bc, bp);
    vec3 c3 = cross(ca, cp);

    return c1.z < 0.0 && c2.z < 0.0 && c3.z < 0.0 ||
           c1.z > 0.0 && c2.z > 0.0 && c3.z > 0.0;
}

float dot2(vec3 v) {
    return dot(v, v);
}

float udTriangle( vec3 p, vec3 a, vec3 b, vec3 c ) {
  vec3 ba = b - a; vec3 pa = p - a;
  vec3 cb = c - b; vec3 pb = p - b;
  vec3 ac = a - c; vec3 pc = p - c;
  vec3 nor = cross( ba, ac );

  return sqrt(
    (sign(dot(cross(ba,nor),pa)) +
     sign(dot(cross(cb,nor),pb)) +
     sign(dot(cross(ac,nor),pc))<2.0)
     ?
     min( min(
     dot2(ba*clamp(dot(ba,pa)/dot2(ba),0.0,1.0)-pa),
     dot2(cb*clamp(dot(cb,pb)/dot2(cb),0.0,1.0)-pb) ),
     dot2(ac*clamp(dot(ac,pc)/dot2(ac),0.0,1.0)-pc) )
     :
     dot(nor,pa)*dot(nor,pa)/dot2(nor) );
}


vec3 draw_star(vec2 uv, vec2 centre, vec3 colour, float radius, float luminance) {
    float strength = 0.80;

    float dist = length(uv - centre) - radius;
    float halo_radius = radius * 1.0 * luminance + luminance * 0.005;
    if (dist > halo_radius * 5.0) return vec3(0.0, 0.0, 0.0);
    dist /= halo_radius;

    float ray_strength = pow(luminance, 2.0) / 10.0;
    if (ray_strength > 1.0) {
        float ray_w = luminance / 4000.0;
        float ray_l = halo_radius * 5.0;
        const float PI = 3.141592;
        for (float theta = -PI + PI/6.0; theta < PI + PI/6.0; theta += (PI/3.0)) {
            vec2 ray_dir = vec2(cos(theta), sin(theta));
            vec2 ray_perp_dir = vec2(cos(theta + 3.141592 / 2.0), sin(theta + 3.141592 / 2.0));
            vec2 a = centre + ray_perp_dir * ray_w;
            vec2 b = centre - ray_perp_dir * ray_w;
            vec2 c = centre + ray_l * ray_dir;

            float dist_to_tri = udTriangle(vec3(uv, 0.0), vec3(a, 0.0), vec3(b, 0.0), vec3(c, 0.0));
            const float threshhold = 0.008;
            if (dist_to_tri <= threshhold) {
                dist /= max(ray_strength * (threshhold - dist_to_tri) / threshhold, 1.0);
            }
            //if (point_in_tri(uv, a, b, c)) {
            //    dist /= ray_strength;
            //}
        }
    }

    if (dist <= 0.0) {
        dist = 0.0;
    }

    float angle = dot(normalize(uv - centre), vec2(1.0, 0.0));
    float delta = random(round(angle * 700.0 + centre.x * centre.y * 600.0));
    if (delta > 0.6) {
        dist /= delta * 0.1 + 1.0;
    }

    if (dist > 1.0) dist = 1.0;

    dist = pow(dist, 1.0 - strength);



    vec3 col = colour * luminance * (1.0 - dist);
    if (col.r > 1.0) col.r = 1.0;
    if (col.g > 1.0) col.g = 1.0;
    if (col.b > 1.0) col.b = 1.0;
    return col;
}

void main() {
    // Normalized pixel coordinates (from 0 to 1)
    // vec2 uv = fragCoord / 360.0; //fragCoord/iResolution.y;
    vec2 d = resolution.xy / 360.0;
    vec2 uv = (fragPos.xy + 0.5) * d;

    vec3 col;
    col += draw_star(uv, vec2(0.5, 0.5), vec3(1.0, 1.0, 0.7), 0.006, 3.0);
    col += draw_star(uv, vec2(1.2, 0.5), vec3(1.0, 0.4, 0.2), 0.015, 4.0);
    col += draw_star(uv, vec2(0.8, 0.6), vec3(1.0, 0.2, 0.2), 0.002, 2.0);
    col += draw_star(uv, vec2(0.9, 0.4), vec3(0.7, 0.7, 1.0), 0.010, 5.0);
    col += draw_star(uv, vec2(0.7, 0.2), vec3(0.86, 0.9, 1.0), 0.006, 3.0);
    col += draw_star(uv, vec2(1.0, 0.1), vec3(1.0, 1.0, 0.9), 0.005, 3.5);
    for (int i = 0; i < 100; i++) {
        col += draw_star(uv, vec2(random(float(i)), random(float(i) * 100.0)) * d, vec3(1.0, 1.0, 0.9), 0.005, 3.5);
    }
    FragColor = vec4(col, 1.0);
}

#version 410 core

// GLSL vertex shader

layout (location = 0) in vec3 a_vertexPosition;
layout (location = 1) in vec3 a_height;
layout (location = 2) in vec3 a_slope;

out vec3 v_eyeSpacePos; // eyeSpacePos
out vec3 v_eyeSpaceNormal; // eyeSpaceNormal
out vec3 v_worldSpaceNormal; // worldSpaceNormal

uniform float heightScale; // = 0.5;
uniform float chopiness;   // = 1.0;
uniform vec2  size;        // = vec2(256.0, 256.0);

uniform mat4 v_MVPMatrix;
uniform mat4 v_MVMatrix;
uniform mat3 v_NMatrix;

void main() {
    float height = a_height.x;
    vec2 slope = a_slope.xy;

    // calculate surface normal from slope for shading
    vec3 normal = normalize(cross(vec3(0.0, slope.y * heightScale, 2.0 / size.x), vec3(2.0 / size.y, slope.x * heightScale, 0.0)));
    v_worldSpaceNormal = normal;

    // calculate position and transform to homogeneous clip space
    vec4 pos = vec4(a_vertexPosition.x, height * heightScale, a_vertexPosition.z, 1.0);
    gl_Position = v_MVPMatrix * pos;

    v_eyeSpacePos = (v_MVMatrix * pos).xyz;
    v_eyeSpaceNormal = (v_NMatrix * normal).xyz;
}

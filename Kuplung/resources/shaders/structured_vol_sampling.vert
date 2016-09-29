#version 410 core

in vec3 a_vertexPosition;

uniform mat4 TileMatrix = mat4(1.0);
uniform vec2 vs_screenResolution;

out vec4 glFragCoord;

void main() {
    vec2 v = a_vertexPosition.xy;
    v.x = v.x * TileMatrix[0][0] + TileMatrix[3][0];
    v.y = v.y * TileMatrix[1][1] + TileMatrix[3][1];
    v = v * 0.5 + 0.5;
    glFragCoord.xy = v * vs_screenResolution;

    gl_Position = vec4(a_vertexPosition, 1.0);
}



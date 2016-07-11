#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

in vec3 gs_vertexPosition[3];
in vec2 gs_textureCoord[3];
in vec3 gs_vertexNormal0[3];
in vec3 gs_vertexNormal[3];
in vec3 gs_tangent0[3];
in vec3 gs_tangent[3];
in vec3 gs_bitangent0[3];
in vec3 gs_bitangent[3];

out vec3 fs_vertexPosition;
out vec2 fs_textureCoord;
out vec3 fs_vertexNormal0;
out vec3 fs_vertexNormal;
out vec3 fs_tangent0;
out vec3 fs_tangent;
out vec3 fs_bitangent0;
out vec3 fs_bitangent;

void main() {
    for (int i=0; i<gl_in.length(); i++) {
        gl_Position = gl_in[i].gl_Position;
        fs_vertexPosition = gs_vertexPosition[i];
        fs_textureCoord = gs_textureCoord[i];
        fs_vertexNormal0 = gs_vertexNormal0[i];
        fs_vertexNormal = gs_vertexNormal[i];
        fs_tangent0 = gs_tangent0[i];
        fs_tangent = gs_tangent[i];
        fs_bitangent0 = gs_bitangent0[i];
        fs_bitangent = gs_bitangent[i];
        EmitVertex();
    }
    EndPrimitive();
}

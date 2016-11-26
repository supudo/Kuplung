#version 410 core

layout(triangles) in;
layout(triangle_strip, max_vertices = 3) out;

uniform mat4 vs_MVPMatrix;
uniform mat4 vs_MVMatrix;
uniform bool tcs_UseTessellation;
uniform int tcs_TessellationSubdivision;

in vec3 gs_vertexPosition[3];
in vec2 gs_textureCoord[3];
in vec3 gs_vertexNormal0[3];
in vec3 gs_vertexNormal[3];
in vec3 gs_tangent0[3];
in vec3 gs_tangent[3];
in vec3 gs_bitangent0[3];
in vec3 gs_bitangent[3];
in vec3 gs_displacementLocation[3];
in float gs_isBorder[3];
in float gs_height[3];
in vec3 gs_shadow_FragPos[3];
in vec3 gs_shadow_Normal[3];
in vec4 gs_shadow_FragPosLightSpace[3];

out vec3 fs_vertexPosition;
out vec2 fs_textureCoord;
out vec3 fs_vertexNormal0;
out vec3 fs_vertexNormal;
out vec3 fs_tangent0;
out vec3 fs_tangent;
out vec3 fs_bitangent0;
out vec3 fs_bitangent;
out vec3 fs_outlineColor;
out float fs_isBorder;
out vec3 fs_shadow_Normal;
out vec4 fs_shadow_FragPosLightSpace;

void main() {
    // outlining
    // http://prideout.net/blog/?p=54

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
        fs_isBorder = gs_isBorder[i];
        fs_shadow_Normal = gs_shadow_Normal[i];
        fs_shadow_FragPosLightSpace = gs_shadow_FragPosLightSpace[i];
        EmitVertex();
    }
    EndPrimitive();

    if (gs_displacementLocation[0].x > 0.0 ||
        gs_displacementLocation[0].y > 0.0 ||
        gs_displacementLocation[0].z > 0.0) {
        for (int i=0; i<gl_in.length(); i++) {
            vec3 newPosition = gs_displacementLocation[i].xyz;
            float dx = newPosition.x;
            float dy = newPosition.y;
            float dz = newPosition.z;
            gl_Position = gl_in[i].gl_Position + vec4(dx, dy, dz, 1.0);

            fs_vertexPosition = gs_vertexPosition[i];
            fs_textureCoord = gs_textureCoord[i];
            fs_vertexNormal0 = gs_vertexNormal0[i];
            fs_vertexNormal = gs_vertexNormal[i];
            fs_tangent0 = gs_tangent0[i];
            fs_tangent = gs_tangent[i];
            fs_bitangent0 = gs_bitangent0[i];
            fs_bitangent = gs_bitangent[i];
            fs_isBorder = gs_isBorder[i];
            fs_shadow_Normal = gs_shadow_Normal[i];
            fs_shadow_FragPosLightSpace = gs_shadow_FragPosLightSpace[i];
            EmitVertex();
        }
        EndPrimitive();
    }
}

layout (triangles) in;
layout (triangle_strip, max_vertices = 3) out;

in VS_OUT {
    vec3 gs_vertexPosition;
    vec2 gs_textureCoord;
    vec3 gs_vertexNormal;
} gs_in[];

out vec3 fs_vertexPosition;
out vec2 fs_textureCoord;
out vec3 fs_vertexNormal;

void main() {
    int i;
    for (i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position;// + vec4(-0.1, 0.0, 0.0, 0.0);

        fs_vertexPosition = gs_in[i].gs_vertexPosition;
        fs_textureCoord = gs_in[i].gs_textureCoord;
        fs_vertexNormal = gs_in[i].gs_vertexNormal;

        EmitVertex();
    }

    EndPrimitive();
}

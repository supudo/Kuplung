layout (triangles) in;
layout (triangle_strip, max_vertices = 6) out;

in VS_OUT {
    vec3 gs_vertexPosition;
    vec2 gs_textureCoord;
    vec3 gs_vertexNormal;
    vec3 gs_displacementLocation;
} gs_in[];

out vec3 fs_vertexPosition;
out vec2 fs_textureCoord;
out vec3 fs_vertexNormal;

void main() {
    for (int i=0; i<gl_in.length(); i++) {
        gl_Position = gl_in[i].gl_Position;
        fs_vertexPosition = gs_in[i].gs_vertexPosition;
        fs_textureCoord = gs_in[i].gs_textureCoord;
        fs_vertexNormal = gs_in[i].gs_vertexNormal;
        EmitVertex();
    }
    EndPrimitive();

    if (gs_in[0].gs_displacementLocation != vec3(0.0, 0.0, 0.0)) {
        for (int i=0; i<gl_in.length(); i++) {
            vec3 newPosition = gs_in[i].gs_displacementLocation.xyz;
            float dx = 1.0 + newPosition.x;
            float dy = 1.0 + newPosition.y;
            float dz = 1.0 + newPosition.z;
            gl_Position = gl_in[i].gl_Position + vec4(dx, dy, dz, 1.0);

            fs_textureCoord = gs_in[i].gs_textureCoord;
            fs_vertexNormal = gs_in[i].gs_vertexNormal;
            EmitVertex();
        }
        EndPrimitive();
    }
}

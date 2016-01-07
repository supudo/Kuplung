layout (triangles) in;
layout (triangle_strip, max_vertices = 4) out;

void main() {
    int i;
    for (i = 0; i < gl_in.length(); i++)
    {
        gl_Position = gl_in[i].gl_Position + vec4(-1.0, 0.0, 0.0, 0.0);
        EmitVertex();
    }
    EndPrimitive();
}

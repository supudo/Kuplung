#version 410 core

layout (location = 0) out vec3 gPosition;
layout (location = 1) out vec3 gNormal;
layout (location = 2) out vec4 gAlbedoSpec;

in vec2 TexCoords;
in vec3 FragPos;
in vec3 Normal;

uniform sampler2D texture_diffuse;
uniform sampler2D texture_specular;

void main() {
    // Store the fragment position vector in the first gbuffer texture
    gPosition = FragPos;

    // Also store the per-fragment normals into the gbuffer
    gNormal = normalize(Normal);

    // And the diffuse per-fragment color
    gAlbedoSpec.rgb = texture(texture_diffuse, TexCoords).rgb;

    // Store specular intensity in gAlbedoSpec's alpha component
    gAlbedoSpec.a = texture(texture_specular, TexCoords).r;
}

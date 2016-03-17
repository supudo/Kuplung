// =================================================
//
// Bump map
//
// =================================================

vec3 calculateBumpedNormal() {
    mat3 normalMatrix = transpose(inverse(mat3(fs_ModelMatrix)));
    vec3 Normal = normalize(normalMatrix * fs_vertexNormal0);
    vec3 T = normalize(normalMatrix * fs_tangent0);
    vec3 B = normalize(normalMatrix * fs_bitangent0);
    vec3 N = normalize(normalMatrix * fs_vertexNormal0);
    mat3 TBN = transpose(mat3(T, B, N));

    vec3 vertexNewNormal = Normal;
    vertexNewNormal = texture(material.sampler_bump, fs_textureCoord).rgb;
    vertexNewNormal = normalize(vertexNewNormal * 2.0 - 1.0);

    return vertexNewNormal;
}

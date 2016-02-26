#version 410

const vec3 Xunitvec = vec3(1.0, 0.0, 0.0);
const vec3 Yunitvec = vec3(0.0, 1.0, 0.0);

// Color of tint to apply (blue)
const vec4 tintColor = vec4(0.0, 0.0, 1.0, 1.0);

// Amount of tint to apply
const float tintFactor = 0.2;

in vec3 fs_normal;
in vec3 fs_eyeDirection;
out vec4 fragColor;

uniform sampler2D diffuseTexture;

void main (void) {
    // Compute reflection vector
    vec3 reflectDir = reflect(fs_eyeDirection, fs_normal);

    // Compute altitude and azimuth angles
    vec2 texcoord;

    texcoord.y = dot(normalize(reflectDir), Yunitvec);
    reflectDir.y = 0.0;
    texcoord.x = dot(normalize(reflectDir), Xunitvec) * 0.5;

    // Translate index values into proper range
    if (reflectDir.z >= 0.0)
        texcoord = (texcoord + 1.0) * 0.5;
    else {
        texcoord.t = (texcoord.t + 1.0) * 0.5;
        texcoord.s = (-texcoord.s) * 0.5 + 1.0;
    }

    // Do a lookup into the environment map.
    vec4 texColor = texture(diffuseTexture, texcoord);

    // Add some blue tint to the image so it looks more like a mirror or glass
    fragColor = mix(texColor, tintColor, tintFactor);
}

#version 410 core

const vec3 xUnitVec = vec3(1.0, 0.0, 0.0);
const vec3 yUnitVec = vec3(0.0, 1.0, 0.0);

// Color of tint to apply (blue)
const vec4 tintColor = vec4(0.0, 0.0, 1.0, 1.0);

// Amount of tint to apply
const float tintFactor = 0.2;

in vec3 fs_normal;
in vec3 fs_eyeDirection;
out vec4 fragColor;

uniform sampler2D diffuseTexture;

void main (void) {
    // reflection vector
    vec3 reflectDirection = reflect(fs_eyeDirection, fs_normal);

    // altitude and azimuth angles
    vec2 texCoordinate;

    texCoordinate.y = dot(normalize(reflectDirection), yUnitVec);
    reflectDirection.y = 0.0;
    texCoordinate.x = dot(normalize(reflectDirection), xUnitVec) * 0.5;

    // translate index values into proper range
    if (reflectDirection.z >= 0.0)
        texCoordinate = (texCoordinate + 1.0) * 0.5;
    else {
        texCoordinate.t = (texCoordinate.t + 1.0) * 0.5;
        texCoordinate.s = (-texCoordinate.s) * 0.5 + 1.0;
    }

    // Do a lookup into the environment map.
    vec4 texColor = texture(diffuseTexture, texCoordinate);

    // Add some blue tint to the image so it looks more like a mirror or glass
    fragColor = mix(texColor, tintColor, tintFactor);
}

uniform mat4 fs_MMatrix;
uniform sampler2D fs_sampler;
uniform vec3 fs_cameraPosition;
uniform float fs_screenResX, fs_screenResY;
uniform float fs_alpha;
uniform vec3 fs_outlineColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;

    float shininess;
    float refraction;
};

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    float strengthAmbient;
    float strengthDiffuse;
    float strengthSpecular;

    vec3 position;
    vec3 direction;
};

in vec3 fs_vertexPosition;
in vec2 fs_textureCoord;
in vec3 fs_vertexNormal;
in float fs_isBorder;

out vec4 fragColor;

uniform Light directionalLight[1];
uniform Material material;

// https://github.com/planetspace/Overlord/blob/master/Overlord/Rendering/Shaders/Default.frag

void main(void) {
    if (fs_isBorder > 0.0) {
        fragColor = vec4(fs_outlineColor, 1.0);
    }
    else {
        vec4 texturedColor = texture(fs_sampler, fs_textureCoord);
        vec3 processedColor = texturedColor.rgb;

        // Ambient
        vec3 ambient = directionalLight[0].strengthAmbient * directionalLight[0].ambient;

        // Diffuse
        vec3 normalDirection = normalize(fs_vertexNormal);
        vec3 lightDir = normalize(directionalLight[0].position - fs_vertexPosition);
        float diff = max(dot(normalDirection, lightDir), 0.0);
        vec3 diffuse = directionalLight[0].strengthDiffuse * diff * directionalLight[0].diffuse * processedColor;

        // Specular
        vec3 viewDir = normalize(fs_cameraPosition - fs_vertexPosition);
        vec3 reflectDir = reflect(-directionalLight[0].direction, normalDirection);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), directionalLight[0].strengthSpecular);
        vec3 specular = directionalLight[0].strengthSpecular * spec * directionalLight[0].specular;

        if (material.refraction > 1.0) {
            // Refraction (Optical Density)
            vec3 refraction = normalize(refract(fs_vertexPosition, normalDirection, material.refraction));
            vec3 refractionColor = mix(texture(fs_sampler, fs_textureCoord + refraction.xy * 0.1), texturedColor, fs_alpha).rgb;
            vec2 pixelTexCoords = vec2(gl_FragCoord.x / fs_screenResX, gl_FragCoord.y / fs_screenResY);
            processedColor = (ambient + diffuse + specular) * texture(fs_sampler, pixelTexCoords + refraction.xy * 0.1).rgb;
        }
        else
            processedColor = (ambient + diffuse + specular) * processedColor;

        // final color
        fragColor = vec4(processedColor, fs_alpha);
    }
}

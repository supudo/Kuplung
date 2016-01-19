uniform mat4 fs_MMatrix;
uniform vec3 fs_cameraPosition;
uniform float fs_screenResX, fs_screenResY;
uniform float fs_alpha;
uniform vec3 fs_outlineColor;

struct Material {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
    vec3 emission;

    float shininess;
    float refraction;

    sampler2D sampler_ambient; // map_Ka
    sampler2D sampler_diffuse; // map_Kd
    sampler2D sampler_color; // map_Ks
    sampler2D sampler_specular; // map_Ns
    sampler2D sampler_dissolve; // map_d
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
        vec4 texturedColor_Diffuse = texture(material.sampler_diffuse, fs_textureCoord);
        vec3 processedColor = texturedColor_Diffuse.rgb;

        // misc
        vec3 normalDirection = normalize(fs_vertexNormal);
        vec3 lightDirection = normalize(directionalLight[0].position - fs_vertexPosition);
        vec3 viewDirection = normalize(fs_cameraPosition - fs_vertexPosition);

        // Ambient
        vec3 ambient = directionalLight[0].strengthAmbient * directionalLight[0].ambient * material.ambient;

        // Diffuse
        float diff = max(dot(normalDirection, lightDirection), 0.0);
        vec3 diffuse = directionalLight[0].strengthDiffuse * diff * directionalLight[0].diffuse * material.diffuse;// * processedColor;

        // Specular
        vec3 reflectDirection = reflect(-directionalLight[0].direction, normalDirection);
        float spec = pow(max(dot(viewDirection, reflectDirection), 0.0), directionalLight[0].strengthSpecular);
        vec3 specular = directionalLight[0].strengthSpecular * spec * directionalLight[0].specular;

        if (material.refraction > 1.0) {
            // Refraction (Optical Density)
            vec3 refraction = normalize(refract(fs_vertexPosition, normalDirection, material.refraction));
            vec3 refractionColor = mix(texture(material.sampler_diffuse, fs_textureCoord + refraction.xy * 0.1), texturedColor_Diffuse, fs_alpha).rgb;
            vec2 pixelTexCoords = vec2(gl_FragCoord.x / fs_screenResX, gl_FragCoord.y / fs_screenResY);
            processedColor = (ambient + diffuse + specular) * texture(material.sampler_diffuse, pixelTexCoords + refraction.xy * 0.1).rgb;
        }
        else
            processedColor = (material.emission + ambient + diffuse + specular) * processedColor;

        // final color
        fragColor = vec4(processedColor, fs_alpha);
    }
}

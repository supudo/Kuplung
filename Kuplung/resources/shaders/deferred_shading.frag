#version 410 core

out vec4 fragColor;
in vec2 TexCoords;

uniform sampler2D gPosition;
uniform sampler2D gNormal;
uniform sampler2D gAlbedoSpec;

struct Light {
    vec3 Position;
    vec3 Color;

    float Linear;
    float Quadratic;
    float Radius;
};

const int NR_LIGHTS = 32;
uniform Light lights[NR_LIGHTS];
uniform vec3 viewPos;

uniform int draw_mode;

void main() {
    // Retrieve data from gbuffer
    vec3 FragPos = texture(gPosition, TexCoords).rgb;
    vec3 Normal = texture(gNormal, TexCoords).rgb;
    vec3 Diffuse = texture(gAlbedoSpec, TexCoords).rgb;
    float Specular = texture(gAlbedoSpec, TexCoords).a;

    // Then calculate lighting as usual
    vec3 lighting = Diffuse * 0.1; // hard-coded ambient component
    vec3 viewDir  = normalize(viewPos - FragPos);
    for (int i=0; i<NR_LIGHTS; ++i) {
        // Calculate distance between light source and current fragment
        float distance = length(lights[i].Position - FragPos);
        if (distance < lights[i].Radius) {
            // Diffuse
            vec3 lightDir = normalize(lights[i].Position - FragPos);
            vec3 diffuse = max(dot(Normal, lightDir), 0.0) * Diffuse * lights[i].Color;
            // Specular
            vec3 halfwayDir = normalize(lightDir + viewDir);
            float spec = pow(max(dot(Normal, halfwayDir), 0.0), 16.0);
            vec3 specular = lights[i].Color * spec * Specular;
            // Attenuation
            float attenuation = 1.0 / (1.0 + lights[i].Linear * distance + lights[i].Quadratic * distance * distance);
            diffuse *= attenuation;
            specular *= attenuation;
            lighting += diffuse + specular;
        }
    }

    if (draw_mode == 1)
        fragColor = vec4(lighting, 1.0);
    else if (draw_mode == 2)
        fragColor = vec4(FragPos, 1.0);
    else if (draw_mode == 3)
        fragColor = vec4(Normal, 1.0);
    else if (draw_mode == 4)
        fragColor = vec4(Diffuse, 1.0);
    else if (draw_mode == 5)
        fragColor = vec4(vec3(Specular), 1.0);
}

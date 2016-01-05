uniform mat4 u_MMatrix;
uniform sampler2D u_sampler;
uniform float u_alpha;
uniform float u_refraction;

uniform vec3 u_lightPosition;
uniform vec3 u_lightDirection;

uniform vec3 u_ambientColor;
uniform vec3 u_diffuseColor;
uniform vec3 u_specularColor;

uniform vec3 u_cameraPosition;

in vec3 v_vertexPosition;
in vec2 v_textureCoord;
in vec3 v_vertexNormal;

out vec4 fragColor;

// https://github.com/planetspace/Overlord/blob/master/Overlord/Rendering/Shaders/Default.frag

void main(void) {
//    fragColor = vec4(1.0, 0.0, 0.0, 1.0);
//    fragColor = texture(u_sampler, v_textureCoord);
//
//    vec4 texelColor = texture(u_sampler, v_textureCoord);
//    fragColor = vec4(texelColor.rgb, texelColor.a * u_alpha);

    // ---
//    
//    vec3 norm = normalize(v_vertexNormal);
//
//    vec3 fragPos = vec3(u_MMatrix * vec4(v_vertexPosition, 1.0f));
//    vec3 lightDirection = normalize(u_lightPosition - fragPos);
//
//    float diff = max(dot(norm, lightDirection), 0.0);
//    vec3 diffuseColor = diff * u_lightColor;
//    
//    vec3 processedColor = diffuseColor * texture(u_sampler, v_textureCoord).rgb;
//    fragColor = vec4(processedColor, u_alpha);

    // ---
    
    // Ambient
    float ambientStrength = 0.1f;
    vec3 ambient = ambientStrength * u_ambientColor;//u_diffuseColor;
    
    // Diffuse
    vec3 norm = normalize(v_vertexNormal);
    vec3 lightDir = normalize(u_lightPosition - v_vertexPosition);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * u_diffuseColor;
    
    // Specular
    float specularStrength = 0.5f;
    vec3 viewDir = normalize(u_cameraPosition - v_vertexPosition);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), 32);
    vec3 specular = specularStrength * spec * u_specularColor;//u_diffuseColor;
    
    // Refraction (Optical Density)
    //vec3 refraction = normalize(Refract(v_vertexPosition, norm, 1.20)); // u_refraction
    //vec3 refractionColor = mix(texture2D(Texture, v_textureCoord + refraction.xy * 0.1), fragColor, u_alpha).rgb;
    
    // Final color
    vec3 processedColor = (ambient + diffuse + specular) * texture(u_sampler, v_textureCoord).rgb;
    //vec3 processedColor = (ambient + diffuse + specular) * texture(u_sampler, v_textureCoord + refraction.xy * 0.1).rgb;
    fragColor = vec4(processedColor, u_alpha);
}

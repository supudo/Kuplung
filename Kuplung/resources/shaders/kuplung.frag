uniform mat4 fs_MMatrix;
uniform sampler2D fs_sampler;
uniform float fs_alpha;
uniform float fs_refraction;
uniform float fs_shininess;

uniform vec3 fs_outlineColor;
uniform vec3 fs_lightPosition;
uniform vec3 fs_lightDirection;
uniform float fs_specularStrength;
uniform float fs_ambientStrength;
uniform float fs_diffuseStrength;

uniform vec3 fs_ambientColor;
uniform vec3 fs_diffuseColor;
uniform vec3 fs_specularColor;

uniform vec3 fs_cameraPosition;
uniform float fs_screenResX, fs_screenResY;

in vec3 fs_vertexPosition;
in vec2 fs_textureCoord;
in vec3 fs_vertexNormal;
in float fs_isBorder;

out vec4 fragColor;

// https://github.com/planetspace/Overlord/blob/master/Overlord/Rendering/Shaders/Default.frag

void main(void) {
    if (fs_isBorder > 0.0) {
        fragColor = vec4(fs_outlineColor, 1.0);
    }
    else {
        vec4 texturedColor = texture(fs_sampler, fs_textureCoord);
        vec3 processedColor = texturedColor.rgb;

        // Ambient
        vec3 ambient = fs_ambientStrength * fs_ambientColor; // fs_diffuseColor;

        // Diffuse
        vec3 normalDirection = normalize(fs_vertexNormal);
        vec3 lightDir = normalize(fs_lightPosition - fs_vertexPosition);
        float diff = max(dot(normalDirection, lightDir), 0.0);
        vec3 diffuse = fs_diffuseStrength * diff * fs_diffuseColor;

        // Specular
        vec3 viewDir = normalize(fs_cameraPosition - fs_vertexPosition);
        vec3 reflectDir = reflect(-fs_lightDirection, normalDirection);
        float spec = pow(max(dot(viewDir, reflectDir), 0.0), fs_specularStrength);
        vec3 specular = fs_specularStrength * spec * fs_specularColor;

        if (fs_refraction > 1.0) {
            // Refraction (Optical Density)
            vec3 refraction = normalize(refract(fs_vertexPosition, normalDirection, fs_refraction));
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

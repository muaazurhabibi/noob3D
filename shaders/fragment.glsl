#version 330 core
out vec4 FragColor;

in vec2 texCoord;
in vec3 Norm;
in vec3 FragPos;

struct Material {
    sampler2D texture_diffuse1;
    sampler2D texture_specular1;
};
uniform Material material;

uniform bool isLight;
uniform vec3 lightPos;

void main() {
    if (isLight)
    {
        FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    } else
    {   
        float ambientStrength = 0.1;
        vec3 ambient = ambientStrength * vec3(1.0f, 1.0f, 1.0f);

        vec3 norm = normalize(Norm);
        vec3 lightDir = normalize(lightPos - FragPos);

        float diff = max(dot(norm, lightDir), 0.0);
        vec3 diffuse = diff * vec3(1.0f, 1.0f, 1.0f);

        vec3 texColor = texture(material.texture_diffuse1, texCoord).rgb;
        vec3 lighting = ambient + diffuse;
        //vec3 result = lighting * vec3(0.5f, 0.5f, 0.5f);
        vec3 result = lighting * texColor;

        FragColor = vec4(result, 1.0);
        //FragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    }
}
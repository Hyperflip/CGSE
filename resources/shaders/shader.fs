#version 330 core
struct Material {
    sampler2D ambient;
    sampler2D diffuse;
    sampler2D specular;
    float shininess;
};

struct Light {
    vec3 position;

    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// reminder: uniforms are "global" variables and can be accessed from outside the shader program
uniform sampler2D texture1;
uniform sampler2D texture2;
uniform sampler2D texture3;

uniform vec3 viewPos;

uniform Material material;
uniform Light light;

uniform float alpha;

void main() {
    // create color maps from the materials textures
    vec3 albedoMap = vec3(texture(material.ambient, TexCoord));
    vec3 diffuseMap = vec3(texture(material.diffuse, TexCoord));
    vec3 specularMap = vec3(texture(material.specular, TexCoord));

    // ambient component
    vec3 ambient = light.ambient * albedoMap;

    // diffuse component
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(light.position - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = light.diffuse * (diff * diffuseMap);

    // specular component
    vec3 viewDir = normalize(viewPos - FragPos);
    vec3 reflectDir = reflect(-lightDir, norm);
    float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);
    vec3 specular = light.specular * (spec * specularMap);

    // combined
    vec3 result = (ambient + diffuse + specular);
    FragColor = vec4(result, 1.0);

    // transparency
    FragColor.a = alpha;
}
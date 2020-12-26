#version 330 core
out vec4 FragColor;

in vec3 FragPos;
in vec3 Normal;
in vec2 TexCoord;

// reminder: uniforms are "global" variables and can be accessed from outside the shader program
uniform sampler2D texture1;
uniform sampler2D texture2;

uniform vec3 lightPos;
uniform vec3 lightColor;
uniform vec3 objectColor;

uniform float alpha;

void main() {
    vec3 albedo = vec3(texture(texture1, TexCoord));
    vec3 diffuseMap = vec3(texture(texture2, TexCoord));
    //FragColor.a = 0.3;

    float ambientStrength = 0.1;
    vec3 ambient = ambientStrength * lightColor;

    // diffuse
    vec3 norm = normalize(Normal);
    vec3 lightDir = normalize(lightPos - FragPos);
    float diff = max(dot(norm, lightDir), 0.0);
    vec3 diffuse = diff * lightColor;

    vec3 result = (ambient + diffuse) * albedo;
    FragColor = vec4(result, 1.0);

    // transparency
    FragColor.a = alpha;
}
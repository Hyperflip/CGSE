#version 330 core
out vec4 FragColor;

uniform sampler2D texture_diffuse1;
uniform sampler2D texture_specular1;
uniform sampler2D texture_normal1;

struct Light {
    vec3 ambient;
    vec3 diffuse;
    vec3 specular;
};

in VS_OUT {
    vec3 FragPos;
    vec2 TexCoord;
    vec3 TangentLightPos;
    vec3 TangentViewPos;
    vec3 TangentFragPos;
} fs_in;

uniform vec3 viewPos;
uniform Light light;

uniform float alpha;

void main() {
    float shininess = 32;

    // obtain normal from normal map in range [0, 1]
    vec3 normal = texture(texture_normal1, fs_in.TexCoord).rgb;
    // transform normal vector to range [-1, 1]
    normal = normalize(normal * 2.0 - 1.0);     // this normal in tangent space

    // get diffuse color
    vec3 color = texture(texture_diffuse1, fs_in.TexCoord).rgb;
    // ambient
    vec3 ambient = light.ambient * color;

    // diffuse
    vec3 lightDir = normalize(fs_in.TangentLightPos - fs_in.TangentFragPos);
    float diff = max(dot(lightDir, normal), 0.0);
    vec3 diffuse = diff * color;

    // specular
    vec3 viewDir = normalize(fs_in.TangentViewPos - fs_in.TangentFragPos);
    vec3 reflectDir = reflect(-lightDir, normal);
    vec3 halfwayDir = normalize(lightDir + viewDir);
    float spec = pow(max(dot(normal, halfwayDir), 0.0), shininess);

    //vec3 test = texture(texture_normal1, fs_in.TexCoord).rgb;

    vec3 specular = light.specular * spec;
    FragColor = vec4(ambient + diffuse + specular, alpha);
    //FragColor = vec4(test, 1.0);
}
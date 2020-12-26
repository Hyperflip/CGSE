#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// reminder: uniforms are "global" variables and can be accessed from outside the shader program
uniform sampler2D albedo;
uniform sampler2D diffuse;

uniform vec3 objectColor;
uniform vec3 lightColor;

void main() {

    //FragColor = texture(albedo, TexCoord);
    FragColor = vec4(lightColor * objectColor, 1.0);
    //FragColor.a = 0.3;
}
#version 330 core
out vec4 FragColor;

in vec2 TexCoord;

// reminder: uniform are "global" variables and can be accessed from outside the shader program
uniform sampler2D texture1;
uniform sampler2D texture2;

void main() {
    FragColor = mix(texture(texture1, TexCoord),
                    texture(texture2, TexCoord), 0.2);
    FragColor.a = 0.3;
}
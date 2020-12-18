#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>

/*
shaders are written in GLSL
version: corresponds to the GLFW version
input of the shader: vec3 called aPos (location = 0 sets its location specifically)
output: vec4 gl_Position, currently just casts aPos into a vec4
*/
const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "void main() {\n"
                                 "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
                                 "}\0";

/*
this fragment shader only has the output variable FragColor
colors are defined as vec4 RGBA values
*/
const char* fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "void main() {\n"
                                   "FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);\n"
                                   "}\0";

void checkShaderCompiled(unsigned int shader) {
    int success;
    char infoLog[512];
    glGetShaderiv(shader, GL_COMPILE_STATUS, &success);
    if(!success) {
        glGetShaderInfoLog(shader, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::VERTEX::COMPILATION_FAILED\n" <<
            infoLog << std::endl;
    }
    else {
        std::cout << "Shader compiled successfully." << std::endl;
    }
}

void checkShadersLinked(unsigned int program) {
    int success;
    char infoLog[512];
    glGetProgramiv(program, GL_LINK_STATUS, &success);
    if(!success) {
        glGetProgramInfoLog(program, 512, NULL, infoLog);
        std::cout << "ERROR::SHADER::PROGRAM::LINKAGE_FAILED\n" <<
            infoLog << std::endl;
    }
    else {
        std::cout << "Shader program linked successfully." << std::endl;
    }
}

// callback function when viewport gets resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
    glViewport(0, 0, width, height);
}

// input handler
void processInput(GLFWwindow* window) {
    if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
        glfwSetWindowShouldClose(window, true);
    }
}

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // macOS specific
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(800, 600, "CGSE", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
    glfwMakeContextCurrent(window);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, 800, 600);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // create new VERTEX SHADER
    unsigned int vertexShader;
    vertexShader = glCreateShader(GL_VERTEX_SHADER);
    // set and compile shader (second argument represents string count of the source)
    glShaderSource(vertexShader, 1, &vertexShaderSource, NULL);
    glCompileShader(vertexShader);
    // check compilation
    checkShaderCompiled(vertexShader);

    // similar procedure for FRAGMENT SHADER
    unsigned int fragmentShader;
    // NOTE: GL_FRAGMENT_SHADER is used here
    fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);
    glShaderSource(fragmentShader, 1, &fragmentShaderSource, NULL);
    glCompileShader(fragmentShader);
    // check compilation
    checkShaderCompiled(fragmentShader);

    // creating a SHADER PROGRAM object
    unsigned int shaderProgram;
    shaderProgram = glCreateProgram();
    // attach and link shaders. the inputs to linked shaders are the previous outputs
    glAttachShader(shaderProgram, vertexShader);
    glAttachShader(shaderProgram, fragmentShader);
    glLinkProgram(shaderProgram);
    // check linkage
    checkShadersLinked(shaderProgram);

    // activate shader program (necessary in every frame)
    glUseProgram(shaderProgram);
    // delete shader objects
    glDeleteShader(vertexShader);
    glDeleteShader(fragmentShader);

    /*
    normalized device coordinates (outside -1 to 1 will be clipped)
    these represent a triangle
    */
    float vertices[] = {
            -0.5f, -0.5f, 0.0f,
            0.5f, -0.5f, 0.0f,
            0.0f,  0.5f, 0.0f,
    };

    /*
    VBO: vertex buffer object, source of the vertex array data
    VAO: vertex array object,
    hold the data about the state needed to describe the vertex array data
    */
    unsigned int VBO, VAO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    /*
    1) bind the VAO
    2) bind and set the VBO(s)
    3) configure vertex attributes
    */

    // 1)
    glBindVertexArray(VAO);

    // 2)
    // bind the VBO to the array buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // copy the buffer data into the array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 3)
    /*
    configuring (linking) the vertex attributes
    WHY?: because OpenGL doesn't know how to input into the vertes shader PER SE
    the vertex buffer (VBO from above) is formatted as follows:
    x   y   z   x   y   z   ...
    vert1       vert2       vertn
    that means:
    * the vertices start at the very beginning of the buffer (pos 0)
    * after a vertex immediately follows another vertex
    * each coordinate is a 4 byte float value
    * each vertex consists of 3 of those values
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float),
                          (void*)0);
    /*
    EXPLANATION OF glVertexAttribPointer:
    0: where does the information go? to aPos in vertex shader (aka location = 0)
    3: it's a vec3 of 3 values
    GL_FLOAT: using float values
    GL_FALSE: data will not be normalized
    3 * sizeof(float): the "stride" aka breadth of each vertex
    (void*)0: starting pos of the vertices in the buffer (cast to void* for some reason)
    */
    glEnableVertexAttribArray(0);

    /*
    unbind VAO and VBO (binding to 0)
    this will allow the configuration of other VAOs/VBOs, since configuration
    only works on the currently bound VAO/VBO.
    NOTE: this is not necessary
    HINT: something something state machine
    */
    glBindBuffer(GL_ARRAY_BUFFER, 0);
    glBindVertexArray(0);

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // rendering commands here:

        // color used for clearing
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);

        // draw triangle
        glUseProgram(shaderProgram);
        // binding the only VAO in this program is technically not necessary
        glBindVertexArray(VAO);
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glBindVertexArray(0); // no need to unbind every time

        // swap buffer and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
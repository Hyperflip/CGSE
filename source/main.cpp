#include <GLFW/glfw3.h>
#include <glad/glad.h>

#include <iostream>

/*
shaders are written in GLSL
version: corresponds to the GLFW version
inputs of the shader:
vec3 called aPos (location = 0 sets the vertex attribute position)
vec3 called aColor (at location = 1)
output: ourColor
*/
const char* vertexShaderSource = "#version 330 core\n"
                                 "layout (location = 0) in vec3 aPos;\n"
                                 "layout (location = 1) in vec3 aColor;\n"
                                 "out vec3 ourColor;\n"
                                 "void main() {\n"
                                 "gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0f);\n"
                                 "ourColor = aColor;\n"
                                 "}\0";

/*
this fragment shader only has the output variable FragColor
colors are defined as vec4 RGBA values
input: ourColor, which is the output from the vertex shader
*/
const char* fragmentShaderSource = "#version 330 core\n"
                                   "out vec4 FragColor;\n"
                                   "in vec3 ourColor;\n"
                                   "void main() {\n"
                                   "FragColor = vec4(ourColor, 1.0f);\n"
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

    // vertices of a rectangle
    float verticesRectangle[] = {
             0.5f,  0.5f, 0.0f,     // top right
             0.5f, -0.5f, 0.0f,     // bottom right
            -0.5f, -0.5f, 0.0f,     // bottom left
            -0.5f,  0.5f, 0.0f      // top left
    };

    // triangle vertex data
    float vertices[] = {
            // positions        // colors
             0.5f, -0.5f, 0.0f, 1.0f, 0.0f, 0.0f,   // bottom right
            -0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f,   // bottom left
             0.0f,  0.5f, 0.0f, 0.0f, 0.0f, 1.0f,   // top
    };

    // drawing indices for the rectangle (as a rectangle consists of 2 triangles)
    unsigned int indices[] = {
            0, 1, 3,    // first triangle
            1, 2, 3     // second triangle
    };

    /*
    VBO: vertex buffer object, source of the vertex array data
    VAO: vertex array object,
    hold the data about the state needed to describe the vertex array data
    EBO: element buffer object. for when indexed drawing is used,
    (telling OpenGL the indices for the desired vertices, which are stored in a buffer)
    */
    unsigned int VBO, VAO, EBO;
    glGenVertexArrays(1, &VAO);
    glGenBuffers(1, &VBO);
    //glGenBuffers(1, &EBO);
    /*
    1) bind the VAO
    2) bind and set the VBO(s) or EBO(s)
    3) configure vertex attributes
    */

    // 1)
    glBindVertexArray(VAO);

    // 2a)
    // bind the VBO to the array buffer
    glBindBuffer(GL_ARRAY_BUFFER, VBO);
    // copy the buffer data into the array buffer
    glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

    // 2b)
    // NOTE: element array buffer
    //glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
    // copy data to buffer and specify the vertex indices
    //glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

    // 3)
    /*
    configuring (linking) the vertex attributes
    WHY?: because OpenGL doesn't know how to input into the vertes shader PER SE
    the vertex buffer (VBO from above) is formatted as follows:
    x   y   z   r   g   b   x   y   z   r   g   b   ...
    vert1                   vert2                   vertn
    that means:
    * the vertices start at the very beginning of the buffer (pos 0)
    * a vertex has position and color attributes, then follows the next vertex
    * each field is a 4 byte float value
    * each vertex consists of 6 of those values
    */

    /* position attribute
    location = 0 (first argument),
    starts at pos 0 of each vertex (last argument)
    */
    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)0);
    // 0 means location = 0 aka aPos in the shader
    glEnableVertexAttribArray(0);
    /* color attribute
    location = 1 (first argument),
    starts at pos 3 of each vertex (last argument)
    */
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float),
                          (void*)(3 * sizeof(float)));
    // 1 means location = 1 aka aColor in the shader
    glEnableVertexAttribArray(1);


    /*
    unbind VAO and VBO (binding to 0)
    this will allow the configuration of other VAOs/VBOs, since configuration
    only works on the currently bound VAO/VBO.
    NOTE: this is not necessary
    HINT: something something state machine

    DON'T unbind EBO while VAO is active, as the bound element array buffer is stored in the active VAO
    again: unbinding means binding to 0: glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, 0);
    */
    glBindBuffer(GL_ARRAY_BUFFER, 0);   // unbind VBO
    glBindVertexArray(0);               // unbind VAO

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // rendering commands here:

        // clear colors:
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT);


        glUseProgram(shaderProgram);

        // binding the only VAO in this program every frame is technically not necessary
        glBindVertexArray(VAO);

        // drawing
        glDrawArrays(GL_TRIANGLES, 0, 3);
        // glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);    // indexed drawing mode

        // glBindVertexArray(0); // no need to unbind VAO every time

        // swap buffer and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
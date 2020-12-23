#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <iostream>

#include "shader.h"

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

const unsigned int SCR_WIDTH = 800;
const unsigned int SCR_HEIGHT = 600;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
    // macOS specific
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGSE", NULL, NULL);
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

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // enable depth test (z-buffer)
    glEnable(GL_DEPTH_TEST);

    // building the shader from the vertex and fragment shader paths
    Shader shader("resources/shaders/shader.vs", "resources/shaders/shader.fs");

    /*
    normalized device coordinates (outside -1 to 1 will be clipped)
    these represent a triangle
    */

    // vertex data of a rectangle
	float verticesTriangle[] = {
			 // positions       // colors         // texture coords
			 0.5f,  0.5f, 0.0f, 1.0f, 0.0f, 0.0f, 1.0f, 1.0f, 	// top right
			 0.5f, -0.5f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 	// bottom right
			-0.5f, -0.5f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 	// bottom left
			-0.5f,  0.5f, 0.0f, 1.0f, 1.0f, 0.0f, 0.0f, 1.0f  	// top left
	};

	// indices for indexed drawing mode (drawing two triangles to represent one rectangle)
	unsigned int indices[] = {
			0, 1, 3, // first triangle
			1, 2, 3  // second triangle
	};

	// vertices of a cube
	float vertices[] = {
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			0.5f, -0.5f, -0.5f,  1.0f, 1.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			0.5f, -0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  0.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f,
			0.5f,  0.5f, -0.5f,  1.0f, 1.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			0.5f,  0.5f,  0.5f,  1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  0.0f, 1.0f
	};
	// world space positions of 10 cubes
	glm::vec3 cubePositions[] = {
			glm::vec3( 0.0f,  0.0f,  0.0f),
			glm::vec3( 2.0f,  5.0f, -15.0f),
			glm::vec3(-1.5f, -2.2f, -2.5f),
			glm::vec3(-3.8f, -2.0f, -12.3f),
			glm::vec3( 2.4f, -0.4f, -3.5f),
			glm::vec3(-1.7f,  3.0f, -7.5f),
			glm::vec3( 1.3f, -2.0f, -2.5f),
			glm::vec3( 1.5f,  2.0f, -2.5f),
			glm::vec3( 1.5f,  0.2f, -1.5f),
			glm::vec3(-1.3f,  1.0f, -1.5f)
	};

	// VERTEX DATA BINDING AND CONFIGURATION
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
	glGenBuffers(1, &EBO);
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
	x   y   z   r   g   b	s	t	x   y   z   r   g   b	s	t   ...
	vert1                   		vert2                   		vertn
	*/

	/* position attribute
	location = 0 (first argument),
	starts at pos 0 of each vertex (last argument)
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
						  (void*)0);
	// 0 means location = 0 aka aPos in the shader
	glEnableVertexAttribArray(0);

	// uv attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float),
						  (void*)(3 * sizeof(float)));
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

    // generating TEXTURE
    unsigned int texture1, texture2;
    glGenTextures(1, &texture1);
    glGenTextures(1, &texture2);

    // FIRST TEXTURE
    glBindTexture(GL_TEXTURE_2D, texture1);
    // set wrapping/filtering options (on the currently bound texture)
    // wrapping mode on S & T direction:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// sampling mode for minification/magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate texture:
	int width, height, nrChannels;
	unsigned char* data = stbi_load("resources/textures/bamboo_wall.jpg", &width, &height, &nrChannels, 0);
	if(data) {
		// generating an RGB texture from the loaded data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		// generating the corresponding mipmap
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	// free loaded image as it's now generated and bound
	stbi_image_free(data);

	// SECOND TEXTURE
	glBindTexture(GL_TEXTURE_2D, texture2);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	data = stbi_load("resources/textures/creepy_smiley.png", &width, &height, &nrChannels, 0);
	if(data) {
		// NOTE: loading as GL_RGBA, since png's contain alpha values
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGBA, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture" << std::endl;
	}
	stbi_image_free(data);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	shader.use();
	// setting the textures to the corresponding samplers in the shader
	shader.setInt("texture1", 0);
	shader.setInt("texture2", 1);

    // RENDER LOOP
    while(!glfwWindowShouldClose(window)) {
        // input
        processInput(window);

        // rendering commands here:

        // clear colors:
        glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
        glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		/*
		a texture object can hold multiple texture units,
		for this image, two textures need to be loaded
		*/
		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture1);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, texture2);

		// shader needs to be activated before accessing its uniforms
        shader.use();
        /*
        create transformations
        1) model = local to world space
        2) view = world to view space
        3) projection = view to clip space
        */
        glm::mat4 view = glm::mat4(1.0f);
        glm::mat4 projection = glm::mat4(1.0f);
        view = glm::translate(view, glm::vec3(0.0f, 0.0f, -3.0f));
        projection = glm::perspective(glm::radians(45.0f), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.f);
		// set uniforms (via self-implemented helper functions)
		shader.setMat4("view", view);
		shader.setMat4("projection", projection);

		// binding the only VAO in this program every frame is technically not necessary
		glBindVertexArray(VAO);

		// iterate over 10 cube positions and calculate corresponding model matrix
		for(unsigned int i = 0; i < 10; i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);
			float angle = 20.0f * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader.setMat4("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

        // glBindVertexArray(0); // no need to unbind VAO every time

        // swap buffer and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}
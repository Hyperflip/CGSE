#include <GLFW/glfw3.h>
#include <glad/glad.h>

#define STB_IMAGE_IMPLEMENTATION
#include <stb_image.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include <camera.h>

#include <iostream>

#include "shader.h"

// callback function when viewport gets resized
void framebuffer_size_callback(GLFWwindow* window, int width, int height);
// input handler
void processInput(GLFWwindow* window);
// mouse input handlers
void mouse_callback(GLFWwindow* window, double xPos, double yPos);
void scroll_callback(GLFWwindow* window, double xOffset, double yOffset);

// settings
const unsigned int SCR_WIDTH = 1600;
const unsigned int SCR_HEIGHT = 1200;

// button press detection
bool buttonPressed = false;

// camera
Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
float lastX = SCR_WIDTH / 2.0f;
float lastY = SCR_WIDTH / 2.0f;
bool firstMouse = true;

// timing
float deltaTime = 0.0f;
float lastFrame = 0.0f;

// lighting
glm::vec3 lightPos(1.2f, 1.0f, 2.0f);
glm::vec3 ambientIntensity(0.2f, 0.2f, 0.2f);
glm::vec3 diffuseIntensity(0.5f, 0.5f, 0.5f);
glm::vec3 specularIntensity(1.0f, 1.0f, 1.0f);

// material
glm::vec3 ambientColor = glm::vec3(1.0f, 0.5f, 0.31f);
glm::vec3 diffuseColor = glm::vec3(1.0f, 0.5f, 0.31f);
glm::vec3 specularColor = glm::vec3(0.5f, 0.5f, 0.5f);
// transparency
float alpha = 1.0f;

int main() {
    glfwInit();
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
#ifdef __APPLE__
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);
#endif

    GLFWwindow* window = glfwCreateWindow(SCR_WIDTH, SCR_HEIGHT, "CGSE", NULL, NULL);
    if(window == NULL) {
        std::cout << "Failed to create GLFW window" << std::endl;
        glfwTerminate();
        return -1;
    }
	glfwMakeContextCurrent(window);

    // correct mismatching window and viewport size
    glfwSetWindowSize(window, SCR_WIDTH / 2, SCR_HEIGHT / 2);

	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);
	glfwSetCursorPosCallback(window, mouse_callback);
	glfwSetScrollCallback(window, scroll_callback);

	// tell GLFW to capture our mouse
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);

    if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
        std::cout << "Failed to initialize GLAD" << std::endl;
        return -1;
    }

    glViewport(0, 0, SCR_WIDTH, SCR_HEIGHT);
    glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

    // enable depth test (z-buffer)
    glEnable(GL_DEPTH_TEST);

    // enable blending
    glEnable(GL_BLEND);
    glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

    // building the shader from the vertex and fragment shader paths
    Shader shader("resources/shaders/shader.vs", "resources/shaders/shader.fs");
	Shader lampShader("resources/shaders/lamp_shader.vs", "resources/shaders/lamp_shader.fs");

    /*
    normalized device coordinates (outside -1 to 1 will be clipped)
    these represent a triangle
    */

	// vertices of a cube
	float vertices[] = {
			// position				// normal				// uv
			-0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,
			 0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,	1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,	1.0f, 1.0f,
			-0.5f,  0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,	0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f,	 0.0f,  0.0f, -1.0f,	0.0f, 0.0f,

			-0.5f, -0.5f,  0.5f,  	 0.0f,  0.0f,  1.0f, 	0.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  	 0.0f,  0.0f,  1.0f, 	1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  	 0.0f,  0.0f,  1.0f, 	1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  	 0.0f,  0.0f,  1.0f, 	1.0f, 1.0f,
			-0.5f,  0.5f,  0.5f,  	 0.0f,  0.0f,  1.0f, 	0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f,  	 0.0f,  0.0f,  1.0f, 	0.0f, 0.0f,

			-0.5f,  0.5f,  0.5f, 	-1.0f,  0.0f,  0.0f, 	1.0f, 0.0f,
			-0.5f,  0.5f, -0.5f, 	-1.0f,  0.0f,  0.0f, 	1.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 	-1.0f,  0.0f,  0.0f, 	0.0f, 1.0f,
			-0.5f, -0.5f, -0.5f, 	-1.0f,  0.0f,  0.0f, 	0.0f, 1.0f,
			-0.5f, -0.5f,  0.5f, 	-1.0f,  0.0f,  0.0f, 	0.0f, 0.0f,
			-0.5f,  0.5f,  0.5f, 	-1.0f,  0.0f,  0.0f, 	1.0f, 0.0f,

			 0.5f,  0.5f,  0.5f,  	 1.0f,  0.0f,  0.0f, 	1.0f, 0.0f,
			 0.5f,  0.5f, -0.5f,  	 1.0f,  0.0f,  0.0f, 	1.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  	 1.0f,  0.0f,  0.0f, 	0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  	 1.0f,  0.0f,  0.0f, 	0.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  	 1.0f,  0.0f,  0.0f, 	0.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  	 1.0f,  0.0f,  0.0f, 	1.0f, 0.0f,

			-0.5f, -0.5f, -0.5f,  	 0.0f, -1.0f,  0.0f, 	0.0f, 1.0f,
			 0.5f, -0.5f, -0.5f,  	 0.0f, -1.0f,  0.0f, 	1.0f, 1.0f,
			 0.5f, -0.5f,  0.5f,  	 0.0f, -1.0f,  0.0f, 	1.0f, 0.0f,
			 0.5f, -0.5f,  0.5f,  	 0.0f, -1.0f,  0.0f, 	1.0f, 0.0f,
			-0.5f, -0.5f,  0.5f,  	 0.0f, -1.0f,  0.0f, 	0.0f, 0.0f,
			-0.5f, -0.5f, -0.5f,  	 0.0f, -1.0f,  0.0f, 	0.0f, 1.0f,

			-0.5f,  0.5f, -0.5f,  	 0.0f,  1.0f,  0.0f, 	0.0f, 1.0f,
			 0.5f,  0.5f, -0.5f,  	 0.0f,  1.0f,  0.0f, 	1.0f, 1.0f,
			 0.5f,  0.5f,  0.5f,  	 0.0f,  1.0f,  0.0f, 	1.0f, 0.0f,
			 0.5f,  0.5f,  0.5f,  	 0.0f,  1.0f,  0.0f, 	1.0f, 0.0f,
			-0.5f,  0.5f,  0.5f,  	 0.0f,  1.0f,  0.0f, 	0.0f, 0.0f,
			-0.5f,  0.5f, -0.5f,  	 0.0f,  1.0f,  0.0f, 	0.0f, 1.0f
	};
	// world space positions of 10 cubes
	int cubesCount = 1;
	glm::vec3 cubePositions[] = {
			glm::vec3( 0.0f,  0.0f,  0.0f),
	};

	// VERTEX DATA BINDING AND CONFIGURATION
	/*
	VBO: vertex buffer object, source of the vertex array data
	VAO: vertex array object,
	hold the data about the state needed to describe the vertex array data
	EBO: element buffer object. for when indexed drawing is used,
	(telling OpenGL the indices for the desired vertices, which are stored in a buffer)
	*/
	unsigned int VBO, cubeVAO, EBO;
	glGenVertexArrays(1, &cubeVAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);
	/*
	1) bind the VAO
	2) bind and set the VBO(s) or EBO(s)
	3) configure vertex attributes
	*/

	// 1)
	glBindVertexArray(cubeVAO);

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
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
						  (void*)0);
	// 0 means location = 0 aka aPos in the shader
	glEnableVertexAttribArray(0);

	// normal attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
						  (void*)(3 * sizeof(float)));
	glEnableVertexAttribArray(1);

	// uv attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float),
						  (void*)(6 * sizeof(float)));
	glEnableVertexAttribArray(2);

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

	// LAMP CUBE VAO and VBO
	unsigned int lampCubeVAO;
	glGenVertexArrays(1, &lampCubeVAO);
	glBindVertexArray(lampCubeVAO);

	// just bind previous VBO
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	/*
	take 3 arguments starting from (void*)0 as to skip unneccessary vertex data
	(the lamp only needs the pos vertices)
	*/
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);


    // generating TEXTURE
    unsigned int ambientTexture, diffuseTexture, specularTexture;
    glGenTextures(1, &ambientTexture);
    glGenTextures(1, &diffuseTexture);
    glGenTextures(1, &specularTexture);

    // FIRST TEXTURE
    glBindTexture(GL_TEXTURE_2D, ambientTexture);
    // set wrapping/filtering options (on the currently bound texture)
    // wrapping mode on S & T direction:
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	// sampling mode for minification/magnification
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	// load and generate texture:
	int width, height, nrChannels;
	unsigned char* data = stbi_load("resources/textures/bricks/bricks_albedo.jpg", &width, &height, &nrChannels, 0);
	if(data) {
		// generating an RGB texture from the loaded data
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		// generating the corresponding mipmap
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture0" << std::endl;
	}
	// free loaded image as it's now generated and bound
	stbi_image_free(data);

	// SECOND TEXTURE
	glBindTexture(GL_TEXTURE_2D, diffuseTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	data = stbi_load("resources/textures/bricks/bricks_diffuse.jpg", &width, &height, &nrChannels, 0);
	if(data) {
		// NOTE: loading as GL_RGBA (7th argument) if loading a .png
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture1" << std::endl;
	}
	stbi_image_free(data);

	// THIRD TEXTURE
	glBindTexture(GL_TEXTURE_2D, specularTexture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
	//stbi_set_flip_vertically_on_load(true); // tell stb_image.h to flip loaded texture's on the y-axis.
	data = stbi_load("resources/textures/bricks/bricks_specular.jpg", &width, &height, &nrChannels, 0);
	if(data) {
		// NOTE: loading as GL_RGBA (7th argument) if loading a .png
		glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
	}
	else {
		std::cout << "Failed to load texture1" << std::endl;
	}
	stbi_image_free(data);

    // uncomment this call to draw in wireframe polygons.
    //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);

	shader.use();
	// setting the textures to the corresponding samplers in the shader
	shader.setInt("material.ambient", 0);
	shader.setInt("material.diffuse", 1);
	shader.setInt("material.specular", 2);

	/*
	relevant transformation matrices
	1) model = local to world space
	2) view = world to view space
	3) projection = view to clip space
	*/
	// RENDER LOOP
	while(!glfwWindowShouldClose(window)) {

		// per-frame time logic
		float currentFrame = glfwGetTime();
		deltaTime = currentFrame - lastFrame;
		lastFrame = currentFrame;

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
		glBindTexture(GL_TEXTURE_2D, ambientTexture);
		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, diffuseTexture);
		glActiveTexture(GL_TEXTURE2);
		glBindTexture(GL_TEXTURE_2D, specularTexture);

		// shader needs to be activated before accessing its uniforms
        shader.use();
        // lighting
        shader.setVec3("light.position", lightPos);
        shader.setVec3("light.ambient", ambientIntensity);
        shader.setVec3("light.diffuse", diffuseIntensity);
        shader.setVec3("light.specular", specularIntensity);
        // material
		shader.setVec3("material.ambient", ambientColor);
		shader.setVec3("material.diffuse", diffuseColor);
		shader.setVec3("material.specular", specularColor);
		shader.setFloat("material.shininess", 32.0f);
		// set alpha val
		shader.setFloat("alpha", alpha);
		// pass cam pos
		shader.setVec3("viewPos", camera.Position);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), (float)SCR_WIDTH / (float)SCR_HEIGHT, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		shader.setMat4("projection", projection);
		shader.setMat4("view", view);
		// world position
		glm::mat4 model = glm::mat4(1.0f);
		glBindVertexArray(cubeVAO);
		for(unsigned int i = 0; i < cubesCount; i++) {
			model = glm::translate(model, cubePositions[i]);
			shader.setMat4("model", model);
			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		// lamp object
		lampShader.use();
		lampShader.setMat4("projection", projection);
		lampShader.setMat4("view", view);
		model = glm::mat4(1.0f);
		model = glm::translate(model, lightPos);
		model = glm::scale(model, glm::vec3(0.2f));
		lampShader.setMat4("model", model);

		glBindVertexArray(lampCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

        // glBindVertexArray(0); // no need to unbind VAO every time

        // swap buffer and poll IO events
        glfwSwapBuffers(window);
        glfwPollEvents();
    }

    glfwTerminate();
    return 0;
}

void processInput(GLFWwindow* window) {
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);

	if (glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(FORWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(BACKWARD, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(LEFT, deltaTime);
	if (glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(RIGHT, deltaTime);

	// toggle transparency
	if(glfwGetKey(window, GLFW_KEY_T) == GLFW_PRESS && !buttonPressed)
		buttonPressed = true;
	if(glfwGetKey(window, GLFW_KEY_T) == GLFW_RELEASE && buttonPressed) {
		alpha = alpha == 1.0f ? 0.3f : 1.0f;
		buttonPressed = false;
	}
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

void mouse_callback(GLFWwindow* window, double xpos, double ypos)
{
	if (firstMouse)
	{
		lastX = xpos;
		lastY = ypos;
		firstMouse = false;
	}

	float xOffset = xpos - lastX;
	float yOffset = lastY - ypos; // reversed since y-coordinates go from bottom to top

	lastX = xpos;
	lastY = ypos;

	camera.ProcessMouseMovement(xOffset, yOffset);
}

// glfw: whenever the mouse scroll wheel scrolls, this callback is called
// ----------------------------------------------------------------------
void scroll_callback(GLFWwindow* window, double xoffset, double yoffset)
{
	camera.ProcessMouseScroll(yoffset);
}
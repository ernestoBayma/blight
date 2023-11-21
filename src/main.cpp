#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <math.h>

#include <Shader.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define GLFW_RESIZE_CALLBACK(name) void name(GLFWwindow* window, int width, int height)
GLFW_RESIZE_CALLBACK(framebuffer_resize_callback) {
	glViewport(0, 0, width, height);
}

static float vertices[] = {
	  // positions   		     // colors		  // texture coords
	  0.5f, 0.5f, 0.0f, 		     1.0f, 0.0f, 0.0f,    1.0f, 1.0f,  // top right
	  0.5f, -0.5f, 0.0f, 		     0.0f, 1.0f, 0.0f,    1.0f, 0.0f,  // bottom right
	 -0.5f, -0.5f, 0.0f,		     0.0f, 0.0f, 1.0f,    0.0f, 0.0f,  // bottom left
	 -0.5f,  0.5f, 0.0f,		     1.0f, 1.0f, 0.0f,    0.0f, 1.0f   // top left
};
static int indices[] = {
	0, 1, 3, // first triangle
	1, 2, 3  // second triangle
};

void process_input(GLFWwindow *window);

int main(int argc, char **argv) 
{
GLFWwindow 	*window;
unsigned int 	VBO, VAO, EBO, texture;
int		success, width, height, channels;
unsigned char   *data = NULL;

	if(!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(640, 480, "Hello", NULL, NULL);
	if(window == NULL) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failure to initialize GLAD\n");
		return -1;
	}

	Blight::Shader shader_prog("shaders/shader.vs", "shaders/shader.fs");
	if(shader_prog.error) {
		fprintf(stderr, "Error creating shader program\n");
		return -1;
	}


	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);
	glGenBuffers(1, &EBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glBindBuffer(GL_ELEMENT_ARRAY_BUFFER, EBO);
	glBufferData(GL_ELEMENT_ARRAY_BUFFER, sizeof(indices), indices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Color attribute
	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	// Texture coord attribute
	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(float), (void*)(6*sizeof(float)));
	glEnableVertexAttribArray(2);

	data = stbi_load("res/container.jpg", &width, &height, &channels, 0);
	if(data == NULL) {
		fprintf(stderr, "Failure to load image\n");
		return -1;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	shader_prog.use();
	shader_prog.setUniformInt("texture1", 0);

	

	while(!glfwWindowShouldClose(window)) {
		process_input(window);
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		shader_prog.use();

		glm::mat4 trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(0.5f, -0.5f, 0.0f));
		trans = glm::rotate(trans, static_cast<float>(glfwGetTime()), glm::vec3(0.0f, 0.0f, 1.0f));
		
		unsigned int transformLocation = glGetUniformLocation(shader_prog.id, "transform");
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, glm::value_ptr(trans));

		glBindVertexArray(VAO);
		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);


		trans = glm::mat4(1.0f);
		trans = glm::translate(trans, glm::vec3(-0.5f, -0.5f, 0.0f));
		float scale_amount = static_cast<float>(sin((2*glm::pi<float>()* 0.5) * glfwGetTime()));
		trans = glm::scale(trans, glm::vec3(scale_amount, scale_amount, scale_amount));
		glUniformMatrix4fv(transformLocation, 1, GL_FALSE, &trans[0][0]);

		glDrawElements(GL_TRIANGLES, 6, GL_UNSIGNED_INT, 0);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteBuffers(1, &EBO);

	glfwTerminate();
	return 0;
}

void process_input(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

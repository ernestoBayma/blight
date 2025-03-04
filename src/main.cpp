#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <math.h>

#include <Shader.h>
#include <Camera.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STATIC_ARRAY_SIZE(array) (sizeof((array))/sizeof((array)[0]))

typedef unsigned int u32;

static u32 Viewport_Width = 800;
static u32 Viewport_Height = 600;

static u32 Window_Width;
static u32 Window_Height;

static Blight::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
static float Last_X = static_cast<float>(Viewport_Width) / 2.0f;
static float Last_Y = static_cast<float>(Viewport_Height) / 2.0f;
static bool firstMouse = true;

static float Last_Time	= 0.0f;
static float Delta_Time = 0.0f;

#define GLFW_RESIZE_CALLBACK(name) void name(GLFWwindow* window, int width, int height)
GLFW_RESIZE_CALLBACK(framebuffer_resize_callback) {
	glViewport(0, 0, width, height);
	Window_Height = height;
	Window_Width = width;
}


#define GLFW_MOUSE_CALLBACK(name) void name(GLFWwindow* window, double xoffset, double yoffset)
GLFW_MOUSE_CALLBACK(mouse_callback) {
	float xpos = static_cast<float>(xoffset);
	float ypos = static_cast<float>(yoffset);

	if(firstMouse) {
		Last_X = xpos;
		Last_Y = ypos;
		firstMouse = false;
	}

	float xoff = xpos - Last_X;
	float yoff = Last_Y - ypos;

	Last_X = xpos;
	Last_Y = ypos;

	camera.ProcessMouseMovement(xoff, yoff);
}

#define GLFW_SCROLL_CALLBACK(name) void name(GLFWwindow* window, double xoffset, double yoffset)
GLFW_SCROLL_CALLBACK(window_scroll_callback) {
	camera.ProcessMouseScroll(static_cast<float>(yoffset));
}

static float vertices[] = {
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

static glm::vec3 cubePositions[] = {
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


void process_input(GLFWwindow *window);

int main(int argc, char **argv) 
{
GLFWwindow 	*window;
unsigned int 	VBO, VAO, texture;
int		success, width, height, channels;
unsigned char   *data = NULL;

	if(!glfwInit()) return -1;

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	if(!Window_Width) Window_Width = Viewport_Width;
	if(!Window_Height) Window_Height = Viewport_Height;

	window = glfwCreateWindow(Window_Width, Window_Height, "Hello", NULL, NULL);
	if(window == NULL) {
		glfwTerminate();
		return -1;
	}

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_resize_callback);
	glfwSetScrollCallback(window, window_scroll_callback);
	glfwSetCursorPosCallback(window, mouse_callback);

	Last_Time = static_cast<float>(glfwGetTime());

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failure to initialize GLAD\n");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Blight::Shader shader_prog("shaders/shader.vs", "shaders/shader.fs");
	if(shader_prog.error) {
		fprintf(stderr, "Error creating shader program\n");
		return -1;
	}

	glGenVertexArrays(1, &VAO);
	glGenBuffers(1, &VBO);

	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	// Position attribute
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	// Texture coord attribute
	glVertexAttribPointer(1, 2, GL_FLOAT, GL_FALSE, 5 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);

	data = stbi_load("res/container.jpg", &width, &height, &channels, 0);
	if(data == NULL) {
		fprintf(stderr, "Failure to load image\n");
		return -1;
	}

	glGenTextures(1, &texture);
	glBindTexture(GL_TEXTURE_2D, texture);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, data);
	glGenerateMipmap(GL_TEXTURE_2D);

	stbi_image_free(data);

	shader_prog.use();
	shader_prog.setUniformInt("texture1", 0);

	float resolution = static_cast<float>(Viewport_Width) / static_cast<float>(Viewport_Height);

	while(!glfwWindowShouldClose(window)) {

		float currentFrame = static_cast<float>(glfwGetTime());
		Delta_Time = currentFrame - Last_Time;
		Last_Time = currentFrame;

		process_input(window);
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, texture);

		shader_prog.use();

		glm::mat4 projection = glm::mat4(1.0f);
		projection = glm::perspective(glm::radians(camera.Zoom), resolution, 0.1f, 100.0f);
		shader_prog.setUniformMat("projection", projection);

		glm::mat4 view = glm::mat4(1.0f);
		view = camera.GetViewMatrix();
		shader_prog.setUniformMat("view", view);
		
		glBindVertexArray(VAO);
		for(u32 i = 0; i < STATIC_ARRAY_SIZE(cubePositions); i++) {
			glm::mat4 model = glm::mat4(1.0f);
			model = glm::translate(model, cubePositions[i]);

			float initialAngleValue = 20.0f;

			if (i == 0 || i % 3 == 0 ) initialAngleValue = static_cast<float>(glfwGetTime());

			float angle = initialAngleValue * i;
			model = glm::rotate(model, glm::radians(angle), glm::vec3(1.0f, 0.3f, 0.5f));
			shader_prog.setUniformMat("model", model);

			glDrawArrays(GL_TRIANGLES, 0, 36);
		}

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void process_input(GLFWwindow *window)
{
	using namespace Blight;

	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
	if(glfwGetKey(window, GLFW_KEY_W) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraDirection::Forward, Delta_Time);
	else if(glfwGetKey(window, GLFW_KEY_S) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraDirection::Backward, Delta_Time);
	else if(glfwGetKey(window, GLFW_KEY_A) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraDirection::Left, Delta_Time);
	else if(glfwGetKey(window, GLFW_KEY_D) == GLFW_PRESS)
		camera.ProcessKeyboard(CameraDirection::Right, Delta_Time);
}


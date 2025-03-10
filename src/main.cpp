#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <stdio.h>
#include <math.h>

#include <Shader.h>
#include <Camera.h>
#include <BlightTypes.h>

#define STB_IMAGE_IMPLEMENTATION
#include "stb_image.h"

#define STATIC_ARRAY_SIZE(array) (sizeof((array))/sizeof((array)[0]))

static void process_input(GLFWwindow *window);
static void framebuffer_resize_callback(GLFWwindow* window, s32 width, s32 height);
static void mouse_callback(GLFWwindow* window, f64 xoffset, f64 yoffset);
static void window_scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset);
static u32 loadTexture(const char *path);

static u32 Window_Width;
static u32 Window_Height;

static Blight::Camera camera(glm::vec3(0.0f, 0.0f, 3.0f));
static glm::vec3 lightPos(1.2f, 1.0f, 2.0f);

static u32 Viewport_Width 	= 800;
static u32 Viewport_Height 	= 600;
static f32 Last_X 		= static_cast<f32>(Viewport_Width)  / 2.0f;
static f32 Last_Y 		= static_cast<f32>(Viewport_Height) / 2.0f;
static b32 firstMouse 		= true;
static f32 Last_Time		= 0.0f;
static f32 Delta_Time 		= 0.0f;

#if 0
static f32 vertices[] = {
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f, 

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f
};
#endif
static f32 vertices[] = {
    // positions          // normals           // texture coords
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,
     0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  1.0f, 1.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f,  0.0f,  0.0f, -1.0f,  0.0f, 0.0f,

    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   1.0f, 1.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f,  0.0f,  0.0f, 1.0f,   0.0f, 0.0f,

    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f, -0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
    -0.5f, -0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f,  0.5f, -1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  0.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  1.0f,  0.0f,  0.0f,  1.0f, 0.0f,

    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,
     0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 1.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
     0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f, -0.5f,  0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f, -0.5f, -0.5f,  0.0f, -1.0f,  0.0f,  0.0f, 1.0f,

    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f,
     0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 1.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
     0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  1.0f, 0.0f,
    -0.5f,  0.5f,  0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 0.0f,
    -0.5f,  0.5f, -0.5f,  0.0f,  1.0f,  0.0f,  0.0f, 1.0f
};

int main(int argc, char **argv) 
{
GLFWwindow 	*window;
u32 		VBO, cubeVAO, lightCubeVAO, diffuseMap;
s32		success;
u8   		*data = NULL;

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

	Last_Time = static_cast<f32>(glfwGetTime());

	if(!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		fprintf(stderr, "Failure to initialize GLAD\n");
		return -1;
	}

	glEnable(GL_DEPTH_TEST);

	Blight::Shader lightingShader("shaders/materials_texture.vs", "shaders/materials_texture.fs");
	if(lightingShader.error) {
		fprintf(stderr, "Error creating lightingShader\n");
		return -1;
	}

	Blight::Shader lightCubeShader("shaders/light_cube.vs", "shaders/light_cube.fs");
	if(lightCubeShader.error) {
		fprintf(stderr, "Error creating lightCubeShader\n");
		return -1;
	}

	glGenVertexArrays(1, &cubeVAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glBindVertexArray(cubeVAO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(3 * sizeof(f32)));
	glEnableVertexAttribArray(1);

	glVertexAttribPointer(2, 2, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)(6 * sizeof(f32)));
	glEnableVertexAttribArray(2);

	glGenVertexArrays(1, &lightCubeVAO);
	glBindVertexArray(lightCubeVAO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 8 * sizeof(f32), (void*)0);
	glEnableVertexAttribArray(0);

	diffuseMap = loadTexture("res/container2.png");
	if(diffuseMap == -1) {
		fprintf(stderr, "Failed to load texture [%d]\n", __LINE__);
		return -1;
	}
	u32 specularMap = loadTexture("res/container2_specular.png");
	if(specularMap  == -1) {
		fprintf(stderr, "Failed to load texture [%d]\n", __LINE__);
		return -1;
	}
	lightingShader.use();
	lightingShader.setUniformInt("material.diffuse", 0);
	lightingShader.setUniformInt("material.specular", 1);

	f32 resolution = static_cast<f32>(Viewport_Width) / static_cast<f32>(Viewport_Height);

	while(!glfwWindowShouldClose(window)) {
		f32 currentFrame = static_cast<f32>(glfwGetTime());
		Delta_Time = currentFrame - Last_Time;
		Last_Time = currentFrame;

		process_input(window);
		
		glClearColor(0.1f, 0.1f, 0.1f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		glActiveTexture(GL_TEXTURE0);
		glBindTexture(GL_TEXTURE_2D, diffuseMap);

		glActiveTexture(GL_TEXTURE1);
		glBindTexture(GL_TEXTURE_2D, specularMap);

		glm::vec3 lightColor = glm::vec3(1.0f);

		f32 lightAngle = currentFrame * 0.5f;
		f32 lightX     = 5.0f * cos(lightAngle);
		f32 lightZ     = 5.0f * sin(lightAngle);

		glm::vec3 lightCurrentPosition = glm::vec3(lightX, 0.0f, lightZ);
		lightingShader.use();
		lightingShader.setUniformVec3("material.specular", 0.5f, 0.5f, 0.5f);
		lightingShader.setUniformFloat("material.shininess", 54.0f);
		lightingShader.setUniformVec3("lightColor", lightColor);

		lightingShader.setUniformVec3("light.ambient", 0.2f, 0.2f, 0.2f);
		lightingShader.setUniformVec3("light.diffuse", 0.5f, 0.5f, 0.5f);
		lightingShader.setUniformVec3("light.specular", 1.0f, 1.0f, 1.0f);

		lightingShader.setUniformVec3("light.position", lightCurrentPosition);
		lightingShader.setUniformVec3("viewPos", camera.Position);

		glm::mat4 projection = glm::perspective(glm::radians(camera.Zoom), resolution, 0.1f, 100.0f);
		glm::mat4 view = camera.GetViewMatrix();
		lightingShader.setUniformMat("projection", projection);
		lightingShader.setUniformMat("view", view);
		
		// world transform 
		glm::mat4 model = glm::mat4(1.0f);
		lightingShader.setUniformMat("model", model);

		//render cube 
		glBindVertexArray(cubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		lightCubeShader.use();
		lightCubeShader.setUniformVec3("lightColor", lightColor);
		lightCubeShader.setUniformMat("projection", projection);
		lightCubeShader.setUniformMat("view", view);

		model = glm::mat4(1.0f);
		model = glm::translate(model, lightCurrentPosition);
		model = glm::scale(model, glm::vec3(0.2f));
		lightCubeShader.setUniformMat("model", model);

		//render cube 
		glBindVertexArray(lightCubeVAO);
		glDrawArrays(GL_TRIANGLES, 0, 36);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &cubeVAO);
	glDeleteVertexArrays(1, &lightCubeVAO);
	glDeleteBuffers(1, &VBO);

	glfwTerminate();
	return 0;
}

void process_input(GLFWwindow *window) {
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

void framebuffer_resize_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	Window_Height = height;
	Window_Width = width;
}

void mouse_callback(GLFWwindow* window, f64 xoffset, f64 yoffset) {
	f32 xpos = static_cast<f32>(xoffset);
	f32 ypos = static_cast<f32>(yoffset);

	if(firstMouse) {
		Last_X = xpos;
		Last_Y = ypos;
		firstMouse = false;
	}

	f32 xoff = xpos - Last_X;
	f32 yoff = Last_Y - ypos;

	Last_X = xpos;
	Last_Y = ypos;

	camera.ProcessMouseMovement(xoff, yoff);
}

void window_scroll_callback(GLFWwindow* window, f64 xoffset, f64 yoffset) {
	camera.ProcessMouseScroll(static_cast<f32>(yoffset));
}

static u32 loadTexture(const char *path) {
	u32 textureId;
	glGenTextures(1, &textureId);

	s32 width, height, nrComponents;
	u8 *data = stbi_load(path, &width, &height, &nrComponents, 0);

	if(data) {
		GLenum format;
		if(nrComponents == 1)
			format = GL_RED;
		if(nrComponents == 3)
			format = GL_RGB;
		if(nrComponents == 4)
			format = GL_RGBA;

		glBindTexture(GL_TEXTURE_2D, textureId);
		glTexImage2D(GL_TEXTURE_2D, 0, format, width, height, 0, format, GL_UNSIGNED_BYTE, data);
		glGenerateMipmap(GL_TEXTURE_2D);
		
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	} else {
		glDeleteTextures(1, &textureId);
		textureId = -1;
	}
	stbi_image_free(data);
	return textureId;
}

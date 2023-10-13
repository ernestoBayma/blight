#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <stdio.h>
#include <math.h>

#include <Shader.h>

#define GLFW_RESIZE_CALLBACK(name) void name(GLFWwindow* window, int width, int height)
GLFW_RESIZE_CALLBACK(framebuffer_resize_callback) {
	glViewport(0, 0, width, height);
}

static float vertices[] = {
	  // positions ( left, right, top )  // colors
	 -0.5f, -0.5f, 0.0f, 		     1.0f, 0.0f , 0.0f, // bottom right
	  0.5,  -0.5f, 0.0f, 		     0.0f, 1.0f , 0.0f, // bottom left
	  0.0,   0.5f, 0.0f,		     0.0f, 0.0f , 1.0f  // top
};


void process_input(GLFWwindow *window);

int main(int argc, char **argv) 
{
GLFWwindow 	*window;
unsigned int 	VBO, VAO;
char	  	*vertex_shader_src, *frag_shader_src;
int		success, vertex_color_location;
float		timeValue, greenValue;

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

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);

	glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 6 * sizeof(float), (void*)(3*sizeof(float)));
	glEnableVertexAttribArray(1);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	glBindVertexArray(0);

	while(!glfwWindowShouldClose(window)) {
		process_input(window);
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		shader_prog.use();

		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

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
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

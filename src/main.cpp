#include <stdio.h>
#include <glad/glad.h>
#include <GLFW/glfw3.h>

#define GLFW_RESIZE_CALLBACK(name) void name(GLFWwindow* window, int width, int height)
GLFW_RESIZE_CALLBACK(framebuffer_resize_callback) {
	glViewport(0, 0, width, height);
}

static float vertices[] = {
	 -0.5f, -0.5f, 0.0f, // left
	  0.5,  -0.5f, 0.0f, // right
	  0.0,   0.5f, 0.0f  // top
};

#define SHADER_CODE(code)  "#version 330 core\n"#code

void process_input(GLFWwindow *window);

int main(int argc, char **argv) 
{
GLFWwindow 	*window;
unsigned int 	VBO, VAO, vertex_shader, frag_shader, shader_program;
char	  	*vertex_shader_src, *frag_shader_src, info_log[512];
int		success;

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

	vertex_shader = glCreateShader(GL_VERTEX_SHADER);
	vertex_shader_src = SHADER_CODE(
		layout (location = 0) in vec3 aPos;
		void main()
		{
			gl_Position = vec4(aPos.x, aPos.y, aPos.z, 1.0);
		}
	);
	
	glShaderSource(vertex_shader, 1, &vertex_shader_src, NULL);
	glCompileShader(vertex_shader);
	glGetShaderiv(vertex_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(vertex_shader, sizeof(info_log), NULL, info_log);
		fprintf(stderr, "Vertex compilation failed: %s\n", info_log);
		glfwTerminate();
		return -1;
	}

	frag_shader = glCreateShader(GL_FRAGMENT_SHADER);
	frag_shader_src = SHADER_CODE(
		out vec4 FragColor;
		void main() 
		{
			FragColor = vec4(1.0f, 0.5f, 0.2f, 1.0f);
		}
	);

	glShaderSource(frag_shader, 1, &frag_shader_src, NULL);
	glCompileShader(frag_shader);
	glGetShaderiv(frag_shader, GL_COMPILE_STATUS, &success);
	if(!success) {
		glGetShaderInfoLog(frag_shader, sizeof(info_log), NULL, info_log);
		fprintf(stderr, "Frag compilation failed: %s\n", info_log);
		glfwTerminate();
		return -1;
	}

	shader_program = glCreateProgram();
	glAttachShader(shader_program, vertex_shader);
	glAttachShader(shader_program, frag_shader);
	glLinkProgram(shader_program);

	glDeleteShader(vertex_shader);
	glDeleteShader(frag_shader);

	glGenBuffers(1, &VBO);
	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);

	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	
	glBindBuffer(GL_ARRAY_BUFFER, 0);	
	glBindVertexArray(0);

	while(!glfwWindowShouldClose(window)) {
		process_input(window);
		
		glClearColor(0.2f, 0.3f, 0.3f, 1.0f);
		glClear(GL_COLOR_BUFFER_BIT);

		glUseProgram(shader_program);
		glBindVertexArray(VAO);
		glDrawArrays(GL_TRIANGLES, 0, 3);

		glfwSwapBuffers(window);
		glfwPollEvents();
	}

	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	glDeleteProgram(shader_program);

	glfwTerminate();
	return 0;
}

void process_input(GLFWwindow *window)
{
	if(glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS)
		glfwSetWindowShouldClose(window, true);
}

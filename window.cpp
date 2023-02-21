#include "window.h"
#include <glad/glad.h> 
#include <GLFW/glfw3.h> 
#include <iostream>
#include <algorithm>
#include <limits>

static void glfw_error_callback(int error, const char* description) {
	fprintf(stderr, "Glfw Error %d: %s\n", error, description);
}

void framebuffer_size_callback(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

bool WinRender::init(unsigned width, unsigned height, const char* const& title, const char* const& vertexPath, const char* const& fragmentPath)
{
	// GLFW: Setup window
	glfwSetErrorCallback(glfw_error_callback);
	if (!glfwInit()) {
		return false;
	}

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 3);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	window = glfwCreateWindow(width, height, title, nullptr, nullptr);
	if (!window)
	{
		std::cerr << "ERROR: Failed to create GLFW window" << std::endl;
		return false;
	}

	displayW = width;
	displayH = height;

	glfwMakeContextCurrent(window);
	glfwSetFramebufferSizeCallback(window, framebuffer_size_callback);

	// glad: загрузка всех указателей на OpenGL-функции
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress))
	{
		std::cerr << "ERROR: Failed to initialize GLAD" << std::endl;
		return false;
	}

	float vertices[] = {
		-1.0f, -1.0f, 0.0f,
		1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, -1.0f, 0.0f,
		1.0f, 1.0f, 0.0f,
		-1.0f, 1.0f, 0.0f
	};

	glGenVertexArrays(1, &VAO);
	glBindVertexArray(VAO);

	glGenBuffers(1, &VBO);
	glBindBuffer(GL_ARRAY_BUFFER, VBO);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertices), vertices, GL_STATIC_DRAW);
	glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (void*)0);
	glEnableVertexAttribArray(0);
	glBindVertexArray(0);

	shader = std::move(Shader(load_shader(vertexPath, fragmentPath)));

	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

	return true;
}

void WinRender::useRender()
{
	shader.use();
	glBindVertexArray(VAO);
}

bool WinRender::windowShouldClose() const {
	return (bool)glfwWindowShouldClose(window);
}

void WinRender::pollEvents() const {
	glfwPollEvents();
}

void WinRender::processInput() const
{
	if (glfwGetKey(window, GLFW_KEY_ESCAPE) == GLFW_PRESS) {
		glfwSetWindowShouldClose(window, true);
	}
}

void WinRender::putPixel(int x, int y, const glm::vec3& color, int width, int height) const
{
	shader.setVec4("pColor", color.x, color.y, color.z, 1.0);

	glEnable(GL_SCISSOR_TEST);
	glScissor(x, y, width, height); // draw one pixel
	glDrawArrays(GL_TRIANGLES, 0, 6);
	glDisable(GL_SCISSOR_TEST);
}

void WinRender::flush(float r, float g, float b, float a)
{
	glfwGetFramebufferSize(window, &displayW, &displayH);
	glfwSwapBuffers(window);
	glClearColor(r, g, b, a);
	glClear(GL_COLOR_BUFFER_BIT);
}

//void WinRender::flush(const jd::vec4& color) {
//	flush(color.x_, color.y_, color.z_, color.w_);
//}

void WinRender::shutdown()
{
	glDeleteVertexArrays(1, &VAO);
	glDeleteBuffers(1, &VBO);
	shader.destroy();
	glfwDestroyWindow(window);
	glfwTerminate();
}

GLFWwindow* WinRender::getWindow() {
	return window;
}

int WinRender::getWidth() const {
	return displayW;
}

int WinRender::getHeight() const {
	return displayH;
}

int WinRender::getDepth() const {
	return wDEPTH;
}
#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "shader.h"
struct GLFWwindow;

class WinRender
{
public:
	/* ������� ������������� ����
	 * @param[in]: ����� ����, ������ ����, ��������� ����, ��������� ������, ����������� ������ (��� �������� �� ������ ����� ������ �� ����� ���� ��������)
	 * @param[out]: true - ������������� �������, false - � ��������� ������
	 * */
	bool init(unsigned width, unsigned height, const char* const& title, const char* const& vertexPath, const char* const& fragmentPath);
	bool windowShouldClose() const;
	void pollEvents() const;
	void processInput() const;
	/* ������������ ������� �� ������
	 * @param[in]: ����������, ������ � ������ �������
	 * */
	void putPixel(int x, int y, const glm::vec3& color, int width = 1, int height = 1) const;
	void useRender();
	void flush(float r, float g, float b, float a);
	void shutdown();
	GLFWwindow* getWindow();

	int getWidth() const;
	int getHeight() const;
	int getDepth() const;

	inline static const int wDEPTH = 255;

private:
	GLFWwindow* window;
	int displayW;
	int displayH;

	unsigned VAO, VBO;
	Shader shader;
};
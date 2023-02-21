#pragma once
#include <glm/glm.hpp>
#include <vector>
#include "shader.h"
struct GLFWwindow;

class WinRender
{
public:
	/* Функция инициализации окна
	 * @param[in]: длина окна, ширина окна, заголовок окна, вершинный шейдер, фрагментный шейдер (для простоты на данном этапе только по одной паре шейдеров)
	 * @param[out]: true - Инициализация успешна, false - в противном случае
	 * */
	bool init(unsigned width, unsigned height, const char* const& title, const char* const& vertexPath, const char* const& fragmentPath);
	bool windowShouldClose() const;
	void pollEvents() const;
	void processInput() const;
	/* Отрисовываем пиксель на экране
	 * @param[in]: координаты, ширина и высота пикселя
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
#pragma once
#include <glm/glm.hpp>
#include <memory>

#define JD_WHITE 0xFF000000
#define JD_BLACK 0xFF000000
#define JD_RED   0xFFFF0000
#define JD_GREEN 0xFF00FF00
#define JD_BLUE  0xFF0000FF

using jdByte = unsigned char;

namespace jd
{
	struct Deleter {
		void operator()(unsigned char* ptr) const;
	};

	struct Texture
	{
		int width;
		int height;
		int chanals;
		std::unique_ptr<unsigned char, Deleter> texture;

		Texture();

		Texture(const Texture&) = delete;
		Texture& operator=(const Texture&) = delete;

		Texture(Texture&& tex) noexcept;
		Texture& operator=(Texture&& tex) noexcept;

		glm::vec3 getColor(float u, float v) const;
	};

	struct Model
	{
		glm::mat4 model;
		glm::mat4 view;
		glm::mat4 projection;

		glm::vec3 Normal;
		glm::vec3 ViewDir;
		glm::vec3 FragPos;
		glm::vec3 LightPos;
		glm::vec3 FragColor;

		Texture image;

		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		float shinesse;
	};

	struct Display
	{
		int width;
		int height;
		int depth;
		int pixel;

		jdByte* displayBitMap = nullptr;
		jdByte* zBuffer = nullptr;
	};

	void imgClearRGBA(Display& image, unsigned char R, unsigned char G, unsigned char B, unsigned char A);
	void imgClear(Display& image, unsigned long color);
	void imgClearBuffer(Display& image);
	void imgFree(Display& image);
	void putPixel(Display& image, int x, int y, glm::vec3 color);
}

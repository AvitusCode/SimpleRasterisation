#include "Model.h"
#include "stb_image.h"

namespace jd
{
	void Deleter::operator()(unsigned char* ptr) const {
		stbi_image_free(ptr);
	}

	Texture::Texture() : width(0), height(0), chanals(0), texture(nullptr) {};

	Texture::Texture(Texture&& tex) noexcept {
		*this = std::move(tex);
	}
	Texture& Texture::operator=(Texture&& tex) noexcept {
		if (this != &tex)
		{
			width = tex.width;
			height = tex.height;
			chanals = tex.chanals;
			tex.chanals = tex.height = tex.width = 0;
			texture = std::move(tex.texture);
		}

		return *this;
	}

	glm::vec3 Texture::getColor(float u, float v) const
	{
		const int x = static_cast<int>(u * width);
		const int y = static_cast<int>(v * height);

		if (x < 0 || y < 0 || x > width || y > height) {
			return glm::vec3(0.0f);
		}

		const int indx = (x + y * width) * chanals;
		const uint8_t* ptr_image = texture.get() + indx;

		glm::vec3 color = glm::vec3(0.0f);
		color.x = static_cast<float>(ptr_image[0]) / 255.0f;
		color.y = static_cast<float>(ptr_image[1]) / 255.0f;
		color.z = static_cast<float>(ptr_image[2]) / 255.0f;

		return color;
	}

	void imgClearRGBA(Display& image, unsigned char R, unsigned char G, unsigned char B, unsigned char A)
	{
		for (int i = 0, size = image.width * image.height * 4; i != size; i += 4)
		{
			image.displayBitMap[i] = B;
			image.displayBitMap[i + 1] = G;
			image.displayBitMap[i + 2] = R;
			image.displayBitMap[i + 3] = A;
		}
	}

	void imgClear(Display& image, unsigned long color)
	{
		unsigned long* pBitImage = reinterpret_cast<unsigned long*>(image.displayBitMap);
		for (int i = 0, size = image.width * image.height; i != size; i++) {
			pBitImage[i] = color;
		}
	}

	void imgClearBuffer(Display& image) {
		memset(image.zBuffer, 255, image.width * image.height * sizeof(jdByte));
	}

	void imgFree(Display& image) 
	{
		delete[] image.displayBitMap;
		delete[] image.zBuffer;
	}

	void putPixel(Display& image, int x, int y, glm::vec3 color)
	{
		unsigned int index = (x + y * image.width) << 2;

		image.displayBitMap[index + 0] = static_cast<jdByte>(255 * color.z);
		image.displayBitMap[index + 1] = static_cast<jdByte>(255 * color.y);
		image.displayBitMap[index + 2] = static_cast<jdByte>(255 * color.x);
	}
}
#include "Model.h"
#include "stb_image.h"

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
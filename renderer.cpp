#include "renderer.h"
#include <vector>
#include "stb_image.h"

namespace jd
{
	// Render functions
	void DrawLine(glm::ivec2 p0, glm::ivec2 p1, glm::vec3 color, WinRender& render)
	{
		bool steep = false;

		if (std::abs(p0.x - p1.x) < std::abs(p0.y - p1.y))
		{
			std::swap(p0.x, p0.y);
			std::swap(p1.x, p1.y);
			steep = true;
		}
		if (p0.x > p1.x)
		{
			std::swap(p0.x, p1.x);
			std::swap(p0.y, p1.y);
		}

		int dx = p1.x - p0.x;
		int dy = p1.y - p0.y;
		int y = p0.y;
		int d1 = std::abs(dy) << 1;
		int d2 = 0;

		for (int x = p0.x; x <= p1.x; x++)
		{
			if (steep) {
				render.putPixel(y, x, color);
			}
			else {
				render.putPixel(x, y, color);
			}

			d2 += d1;

			if (d2 > dx) {
				d2 -= dx * 2;
				y += (p1.y > p0.y) ? 1 : -1;
			}
		}
	}

	glm::vec3 barycentric(const std::array<glm::ivec3, 3>& pts, const glm::ivec3 P)
	{
		glm::vec3 u = glm::cross(glm::vec3(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x),
			glm::vec3(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y));

		if (std::abs(u.z) < 1) {
			return glm::vec3(-1.0f, 1.0f, 1.0f);
		}

		return glm::vec3(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	}

	void DrawTriangle(const std::array<glm::ivec3, 3>& pts, const std::array<glm::vec2, 3>& tex, Model& data, WinRender& render)
	{
		glm::ivec2 bboxmin(render.getWidth() - 1, render.getHeight() - 1);
		glm::ivec2 bboxmax(0, 0);
		glm::ivec2 clamp(bboxmin);

		// Finding min AABB
		for (uint16_t i = 0; i < 3; i++)
		{
			bboxmin.x = std::max(0, std::min(bboxmin.x, pts[i].x));
			bboxmin.y = std::max(0, std::min(bboxmin.y, pts[i].y));
			bboxmax.x = std::min(clamp.x, std::max(bboxmax.x, pts[i].x));
			bboxmax.y = std::min(clamp.y, std::max(bboxmax.y, pts[i].y));
		}

		glm::ivec3 frag;
		for (frag.x = bboxmin.x; frag.x <= bboxmax.x; frag.x++)
		{
			for (frag.y = bboxmin.y; frag.y <= bboxmax.y; frag.y++)
			{
				glm::vec3 bc = barycentric(pts, frag);
				if (bc.x < 0 || bc.y < 0 || bc.z < 0) {
					continue;
				}

				float u = tex[0].x * bc.x + tex[1].x * bc.y + tex[2].x * bc.z;
				float v = tex[0].y * bc.x + tex[1].y * bc.y + tex[2].y * bc.z;
				data.FragColor = data.image.getColor(u, v);

				// pixel Blin-Phong model
				glm::vec3 lightDir = glm::normalize(-data.LightPos);
				float diff = glm::max(glm::dot(data.Normal, lightDir), 0.0f);
				glm::vec3 HalfDir = glm::normalize(lightDir + data.ViewDir);
				float spec = glm::pow(glm::max(glm::dot(data.Normal, HalfDir), 0.0f), data.shinesse);
				glm::vec3 colorPhong = (data.ambient + diff * data.diffuse + spec * data.specular) * data.FragColor;

				render.putPixel(frag.x, frag.y, colorPhong);
			}
		}
	}

	// Transforms
	glm::ivec2 Viewport2D(glm::vec3 vec, int x, int y, int width, int height){
		return glm::ivec2((vec.x + 1.0f) * width / 2.0f + x, (vec.y + 1.0f) * height / 2.0f + y);
	}

	glm::ivec3 Viewport3D(glm::vec3 vec, int x, int y, int width, int height, int depth) {
		return glm::ivec3((vec.x + 1.0f) * width / 2.0f + x, (vec.y + 1.0f) * height / 2.0f + y, (vec.z + 1.0f) * depth / 2.0f);
	}

	[[nodiscard]] Texture LoadTexture(const char* filename, bool inverse)
	{
		int width, height, nrChanals;
		stbi_set_flip_vertically_on_load(inverse);
		std::unique_ptr<unsigned char, Deleter> image(stbi_load(filename, &width, &height, &nrChanals, 0));

		if (image.get())
		{
			Texture texture;
			texture.width = width;
			texture.height = height;
			texture.chanals = nrChanals;
			texture.texture = std::move(image);
			return texture;
		}

		return {};
	}
}
#include "renderer.h"
#include <vector>
#include <glm/gtc/matrix_transform.hpp>
#include "stb_image.h"

#include "Fractal.h"
namespace jd
{
	// Render functions
	void DrawLine(glm::ivec2 p0, glm::ivec2 p1, glm::vec3 color, Display& render)
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
				putPixel(render, y, x, color);
			}
			else {
				putPixel(render, x, y, color);
			}

			d2 += d1;

			if (d2 > dx) {
				d2 -= dx * 2;
				y += (p1.y > p0.y) ? 1 : -1;
			}
		}
	}

	[[nodiscard]] glm::vec3 barycentric(const std::array<glm::ivec3, 3>& pts, const glm::ivec3 P)
	{
		glm::vec3 u = glm::cross(glm::vec3(pts[2].x - pts[0].x, pts[1].x - pts[0].x, pts[0].x - P.x),
			glm::vec3(pts[2].y - pts[0].y, pts[1].y - pts[0].y, pts[0].y - P.y));

		if (std::abs(u.z) < 1) {
			return glm::vec3(-1.0f, 1.0f, 1.0f);
		}

		return glm::vec3(1.0f - (u.x + u.y) / u.z, u.y / u.z, u.x / u.z);
	}

	void DrawTriangle(const std::array<glm::ivec3, 3>& pts, const std::array<glm::vec2, 3>& tex, Model& data, Display& render)
	{
		glm::ivec2 bboxmin(render.width - 1, render.height - 1);
		glm::ivec2 bboxmax(0, 0);
		glm::ivec2 clamp(bboxmin);

		// Finding min AABB
		for (uint16_t i = 0; i < 3; i++)
		{
			bboxmin.x = MAX(0, MIN(bboxmin.x, pts[i].x));
			bboxmin.y = MAX(0, MIN(bboxmin.y, pts[i].y));
			bboxmax.x = MIN(clamp.x, MAX(bboxmax.x, pts[i].x));
			bboxmax.y = MIN(clamp.y, MAX(bboxmax.y, pts[i].y));
		}

		glm::ivec3 frag;
		const glm::vec3 z_pts = glm::vec3(pts[0].z, pts[1].z, pts[2].z);
		for (frag.x = bboxmin.x; frag.x <= bboxmax.x; frag.x++)
		{
			for (frag.y = bboxmin.y; frag.y <= bboxmax.y; frag.y++)
			{
				glm::vec3 bc = barycentric(pts, frag);
				jdByte frag_depth = static_cast<jdByte>(glm::dot(z_pts, bc));
				
				if (bc.x < 0 || bc.y < 0 || bc.z < 0 || frag_depth > render.zBuffer[frag.x + frag.y * render.width]) {
					continue;
				}

				render.zBuffer[frag.x + frag.y * render.width] = frag_depth;
				float u = tex[0].x * bc.x + tex[1].x * bc.y + tex[2].x * bc.z;
				float v = tex[0].y * bc.x + tex[1].y * bc.y + tex[2].y * bc.z;
				data.FragColor = data.image.getColor(u, v);

				// pixel Blin-Phong model
				glm::vec3 lightDir = glm::normalize(-data.LightPos);
				float diff = MAX(glm::dot(data.Normal, lightDir), 0.0f);
				glm::vec3 HalfDir = glm::normalize(lightDir + data.ViewDir);
				float spec = glm::pow(MAX(glm::dot(data.Normal, HalfDir), 0.0f), data.shinesse);
				glm::vec3 colorPhong = (data.ambient + diff * data.diffuse + spec * data.specular) * data.FragColor;

				putPixel(render, frag.x, frag.y, colorPhong);
			}
		}
	}

	// Transforms
	[[nodiscard]] glm::ivec2 Viewport2D(glm::vec3 vec, int x, int y, int width, int height){
		return glm::ivec2((vec.x + 1.0f) * width / 2.0f + x, (vec.y + 1.0f) * height / 2.0f + y);
	}

	[[nodiscard]] glm::ivec3 Viewport3D(glm::vec3 vec, int x, int y, int width, int height, int depth) {
		return glm::ivec3((vec.x + 1.0f) * width / 2.0f + x, (vec.y + 1.0f) * height / 2.0f + y, (vec.z + 1.0f) * depth / 2.0f);
	}

	[[nodiscard]] std::vector<glm::ivec3> TriangulatePolygon(const std::vector<glm::vec3> vertex, glm::vec3 Normal)
	{
		constexpr float EPS = 0.001f;
		std::vector<glm::ivec3> indexes;
		std::vector<bool> active(vertex.size(), true);
		glm::ivec3 triangle;

		auto GetNext = [](int64_t x, int64_t size, const std::vector<bool>& active) {
			while (true)
			{
				if (++x == size) {
					x = 0;
				}
				if (active[x]) {
					return x;
				}
			}
		};

		auto GetPrev = [](int64_t x, int64_t size, const std::vector<bool>& active) {
			while (true)
			{
				if (--x == -1) {
					x = size - 1;
				}
				if (active[x]) {
					return x;
				}
			}
		};

		int64_t triangleCount = 0;
		int64_t start = 0;
		int64_t p1 = 0;
		int64_t p2 = 1;
		int64_t m1 = vertex.size() - 1;
		int64_t m2 = vertex.size() - 2;

		bool lastPositive = false;
		while (true)
		{
			if (p2 == m2)
			{
				// Only three vertices remain
				triangle.x = m1;
				triangle.y = p1;
				triangle.z = p2;
				triangleCount++;
				indexes.push_back(triangle);
				break;
			}

			const glm::vec3& vp1 = vertex[p1];
			const glm::vec3& vp2 = vertex[p2];
			const glm::vec3& vm1 = vertex[m1];
			const glm::vec3& vm2 = vertex[m2];
			bool positive = false;
			bool negative = false;

			glm::vec3 n1 = glm::cross(Normal, glm::normalize(vm1 - vp2));
			if (glm::dot(n1, vp1 - vp2) > EPS)
			{
				positive = true;
				glm::vec3 n2 = glm::cross(Normal, glm::normalize(vp1 - vm1));
				glm::vec3 n3 = glm::cross(Normal, glm::normalize(vp2 - vp1));

				for (int64_t i = 0; i < vertex.size(); i++)
				{
					if (active[i] && i != p1 && i != p2 && i != m1)
					{
						const glm::vec3& vi = vertex[i];

						if (glm::dot(n1, glm::normalize(vi - vp2)) > -EPS
							&& glm::dot(n2, glm::normalize(vi - vm1)) > -EPS
							&& glm::dot(n3, glm::normalize(vi - vp1)) > -EPS)
						{
							positive = false;
							break;
						}
					}
				}
			}

			n1 = glm::cross(Normal, glm::normalize(vm2 - vp1));
			if (glm::dot(n1, vm1 - vp1) > EPS)
			{
				negative = true;
				glm::vec3 n2 = glm::cross(Normal, glm::normalize(vm1 - vm2));
				glm::vec3 n3 = glm::cross(Normal, glm::normalize(vp1 - vm1));

				for (int64_t i = 0; i < vertex.size(); i++)
				{
					if (active[i] && i != m1 && i != m2 && i != p1)
					{
						const glm::vec3& vi = vertex[i];

						if (glm::dot(n1, glm::normalize(vi - vp1)) > -EPS
							&& glm::dot(n2, glm::normalize(vi - vm2)) > -EPS
							&& glm::dot(n3, glm::normalize(vi - vm1)) > -EPS)
						{
							negative = false;
							break;
						}
					}
				}
			}

			if (positive && negative)
			{
				float pd = glm::dot(glm::normalize(vp2 - vm1), glm::normalize(vm2 - vm1));
				float md = glm::dot(glm::normalize(vm2 - vp1), glm::normalize(vp2 - vp1));
			
				if (glm::abs(pd - md) < EPS)
				{
					if (lastPositive) {
						positive = false;
					}
					else {
						negative = false;
					}
				}
				else
				{
					if (pd < md) {
						negative = false;
					}
					else {
						positive = false;
					}
				}
			}

			if (positive)
			{
				active[p1] = false;
				triangle.x = m1;
				triangle.y = p1;
				triangle.z = p2;
				triangleCount++;
				indexes.push_back(triangle);

				p1 = GetNext(p1, vertex.size(), active);
				p2 = GetNext(p2, vertex.size(), active);
				lastPositive = true;
				start = -1;
			}
			else if (negative)
			{
				active[m1] = false;
				triangle.x = m2;
				triangle.y = m1;
				triangle.z = p1;
				triangleCount++;
				indexes.push_back(triangle);

				m1 = GetPrev(m1, vertex.size(), active);
				m2 = GetPrev(m2, vertex.size(), active);
				lastPositive = false;
				start = -1; 
			}
			else
			{
				if (start == -1) {
					start = p2;
				}
				else if (start == p2) {
					break;
				}

				m2 = m1;
				m1 = p1;
				p1 = p2;
				p2 = GetNext(p2, vertex.size(), active);
			}
		}

		return indexes;
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

	void Render(glm::vec4 p0, glm::vec4 p1, glm::vec4 p2, glm::vec2 t0, glm::vec2 t1, glm::vec2 t2, Model& data, Display& image)
	{
		// примен€ем трансформации и делаем перспективное деление
		glm::mat4 transform = data.projection * data.view * data.model;
		glm::vec4 temp = transform * p0;
		glm::vec3 res1 = glm::vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);
		temp = transform * p1;
		glm::vec3 res2 = glm::vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);
		temp = transform * p2;
		glm::vec3 res3 = glm::vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);

		// ќтсекаем грани, что смотр€т в противоположную от зрител€ сторону
		glm::vec3 normal = glm::cross(res2 - res1, res3 - res1);
		if (glm::dot(normal, data.ViewDir) > 0) {
			return;
		}

		std::array<glm::ivec3, 3> triangle = { {jd::Viewport3D(res1, 0, 0, image.width, image.height, image.depth),
								   jd::Viewport3D(res2, 0, 0, image.width, image.height, image.depth),
								   jd::Viewport3D(res3, 0, 0, image.width, image.height, image.depth)} };
		data.Normal = glm::normalize(glm::cross(glm::vec3(data.model * (p1 - p0)), glm::vec3(data.model * (p2 - p0))));
		std::array<glm::vec2, 3> tex;
		tex[0] = t0; tex[1] = t1; tex[2] = t2;

		jd::DrawTriangle(triangle, tex, data, image);
	}

	void rast_cube(Display& image, const std::vector<glm::vec4>& vertices, const std::vector<glm::vec2>& tex)
	{
		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);
		glm::mat4 model = glm::mat4(1.0f);
		glm::mat4 view = glm::mat4(1.0f);
		glm::mat4 projection = glm::mat4(1.0f);

		glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
		glm::vec3 fragPos = glm::vec3(0.0f, 0.0f, -3.0f);
		glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, -5.0f);

		view = glm::translate(view, fragPos);
		model = glm::scale(model, glm::vec3(0.2f));
		model = glm::rotate(model, (float)timeGetTime() / 1000 * glm::radians(50.0f), glm::vec3(0.5f, 1.0f, 1.0f));
		projection = glm::perspective(glm::radians(45.0f), (float)image.width / image.height, 0.1f, 100.f);

		Model data = {
			model,
			view,
			projection,
			glm::vec3(0.0f),
			fragPos - cameraPos,
			fragPos,
			lightPos,
			color,
			std::move(jd::LoadTexture("box.png", true)),
			glm::vec3(0.3f, 0.24f, 0.14f),
			glm::vec3(0.7f, 0.42f, 0.26f),
			glm::vec3(0.5f, 0.5f, 0.5f),
			64.0f
		};

		for (uint16_t i = 0; i < vertices.size(); i += 3) {
			Render(vertices[i], vertices[i + 1], vertices[i + 2], tex[i], tex[i + 1], tex[i + 2], data, image);
		}
	}

	void draw_fractal(Display& display, Window& window, IFractal& m_fractal)
	{
		if (window.getButtonClick() == BUTTON::RIGHT) {
			m_fractal.CurveNext();
			window.setButtonClick(BUTTON::NOTHING);
		}
		else if (window.getButtonClick() == BUTTON::LEFT) {
			m_fractal.CurvePrev();
			window.setButtonClick(BUTTON::NOTHING);
		}

		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		m_fractal.CurveDraw(display, color);
	}

	// @param vertices: in display coords
	// @param indexes: indexes of triangulated polygon
	// @param triangulate: 0 or 1 - off/on triangulation mode
	void draw_triangulation(Display& display, Window& window, const std::vector<glm::vec3>& vertices, std::vector<glm::ivec3>& indexes, int& triangulate)
	{
		if (window.getButtonClick() == BUTTON::RIGHT)
		{
			indexes = TriangulatePolygon(vertices, glm::vec3(0.0f, 0.0f, -1.0f));
			triangulate = 1;
			window.setButtonClick(BUTTON::NOTHING);
		}
		else if (window.getButtonClick() == BUTTON::LEFT)
		{
			indexes.clear();
			triangulate = 0;
			window.setButtonClick(BUTTON::NOTHING);
		}

		glm::vec3 color = glm::vec3(1.0f, 1.0f, 1.0f);

		if (triangulate)
		{
			for (size_t i = 0; i < indexes.size(); i++)
			{
				jd::DrawLine(vertices[indexes[i].x], vertices[indexes[i].y], color, display);
				jd::DrawLine(vertices[indexes[i].y], vertices[indexes[i].z], color, display);
				jd::DrawLine(vertices[indexes[i].z], vertices[indexes[i].x], color, display);
			}
		}
		else
		{
			for (size_t i = 0; i < vertices.size(); i++) {
				jd::DrawLine(vertices[i], vertices[(i + 1) % vertices.size()], color, display);
			}
		}
	}

	void startProgramm(Window& window)
	{
		std::vector<glm::vec4> vertices = {
			// задн€€ грань
			glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // нижн€€-лева€
			glm::vec4(1.0f,  1.0f, -1.0f, 1.0f), // верхн€€-права€
			glm::vec4(1.0f, -1.0f, -1.0f, 1.0f),  // нижн€€-права€         
			glm::vec4(1.0f,  1.0f, -1.0f, 1.0f),  // верхн€€-права€
			glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),  // нижн€€-лева€
			glm::vec4(-1.0f,  1.0f, -1.0f, 1.0f),   // верхн€€-лева€

			// передн€€ грань
			glm::vec4(-1.0f, -1.0f,  1.0f, 1.0f),  // нижн€€-лева€
			glm::vec4(1.0f, -1.0f,  1.0f, 1.0f),  // нижн€€-права€
			glm::vec4(1.0f,  1.0f,  1.0f, 1.0f),  // верхн€€-права€
			glm::vec4(1.0f,  1.0f,  1.0f, 1.0f),   // верхн€€-права€
			glm::vec4(-1.0f,  1.0f,  1.0f, 1.0f),   // верхн€€-лева€
			glm::vec4(-1.0f, -1.0f,  1.0f, 1.0f),   // нижн€€-лева€

			// грань слева
			glm::vec4(-1.0f,  1.0f,  1.0f, 1.0f),  // верхн€€-права€
			glm::vec4(-1.0f,  1.0f, -1.0f, 1.0f),  // верхн€€-лева€
			glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),  // нижн€€-лева€
			glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f),  // нижн€€-лева€
			glm::vec4(-1.0f, -1.0f,  1.0f, 1.0f), // нижн€€-права€
			glm::vec4(-1.0f,  1.0f,  1.0f, 1.0f),  // верхн€€-права€

		    // грань справа
			glm::vec4(1.0f,  1.0f,  1.0f, 1.0f), // верхн€€-лева€
			glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), // нижн€€-права€
			glm::vec4(1.0f,  1.0f, -1.0f, 1.0f), // верхн€€-права€         
			glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), // нижн€€-права€
			glm::vec4(1.0f,  1.0f,  1.0f, 1.0f), // верхн€€-лева€
			glm::vec4(1.0f, -1.0f,  1.0f, 1.0f), // нижн€€-лева€     

		    // нижн€€ грань
			glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // верхн€€-права€
			glm::vec4(1.0f, -1.0f, -1.0f, 1.0f), // верхн€€-лева€
			glm::vec4(1.0f, -1.0f,  1.0f, 1.0f), // нижн€€-лева€
			glm::vec4(1.0f, -1.0f,  1.0f, 1.0f), // нижн€€-лева€
			glm::vec4(-1.0f, -1.0f,  1.0f, 1.0f), // нижн€€-права€
			glm::vec4(-1.0f, -1.0f, -1.0f, 1.0f), // верхн€€-права€

            // верхн€€ грань
			glm::vec4(-1.0f,  1.0f, -1.0f, 1.0f), // верхн€€-лева€
			glm::vec4(1.0f,  1.0f , 1.0f, 1.0f), // нижн€€-права€
			glm::vec4(1.0f,  1.0f, -1.0f, 1.0f), // верхн€€-права€     
			glm::vec4(1.0f,  1.0f,  1.0f, 1.0f),  // нижн€€-права€
			glm::vec4(-1.0f,  1.0f, -1.0f, 1.0f), // верхн€€-лева€
			glm::vec4(-1.0f,  1.0f,  1.0f, 1.0f)  // нижн€€-лева€        
		};

		std::vector<glm::vec2> textures = {
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),

			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),

			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),

			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 0.0f),

			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),

			glm::vec2(0.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(1.0f, 1.0f),
			glm::vec2(1.0f, 0.0f),
			glm::vec2(0.0f, 1.0f),
			glm::vec2(0.0f, 0.0f)
		};
		
		window.setPause(16); // fps ~= 60 per sec
		Display* image = (Display*)GetWindowLongPtr(window.getHWND(), 0);
		mFractal m_fractal;
		m_fractal.CurveCreate(image->width / 4.0f, image->height / 2.0f, image->width * 3.0f / 4.0f, image->height * 3.0f / 4.0f);

		std::vector<glm::vec3> polygon = {
			glm::vec3(1.0f / 9.0f * image->width, 8.9f / 9.0f * image->height, 0.0f),
			glm::vec3(3.0f / 9.0f * image->width, 8.4f / 9.0f * image->height, 0.0f),
			glm::vec3(5.0f / 9.0f * image->width, 5.0f / 9.0f * image->height, 0.0f),
			glm::vec3(7.0f / 9.0f * image->width, 7.0f / 9.0f * image->height, 0.0f),
			glm::vec3(8.5f / 9.0f * image->width, 3.5f / 9.0f * image->height, 0.0f),
			glm::vec3(5.5f / 9.0f * image->width, 3.0f / 9.0f * image->height, 0.0f),
			glm::vec3(4.5f / 9.0f * image->width, 1.0f / 9.0f * image->height, 0.0f),
			glm::vec3(2.5f / 9.0f * image->width, 1.3f / 9.0f * image->height, 0.0f),
			glm::vec3(0.5f / 9.0f * image->width, 3.5f / 9.0f * image->height, 0.0f)
		};

		std::vector<glm::ivec3> indexes;
		// Triangulare on/off
		int triangulate = 0;

		while (window.work())
		{
			imgClear(*image, JD_BLACK);
			imgClearBuffer(*image);

			switch (window.getRender())
			{
			case RENDER::CUBE:
				rast_cube(*image, vertices, textures);
				break;

			case RENDER::TRIANGE:
				draw_triangulation(*image, window, polygon, indexes, triangulate);
				break;

			case RENDER::FRACTAL:
				draw_fractal(*image, window, m_fractal);
				break;
			}

			window.drawBuffer(*image);
		}
	}
}
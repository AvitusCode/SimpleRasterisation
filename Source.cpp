#include <iostream>
#include <algorithm>
#include <cmath>
#include <vector>
#include <array>

#include <glad/glad.h>
#include <GLFW/glfw3.h>

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>

#include "Model.h"
#include "renderer.h"
#include "window.h"

constexpr int WIDTH  = 800;
constexpr int HEIGHT = 800;
constexpr int DEPTH = 255;

glm::vec3 cameraPos = glm::vec3(0.0f, 0.0f, 0.0f);
glm::vec3 fragPos = glm::vec3(0.0f, 0.0f, -3.0f);
glm::vec3 lightPos = glm::vec3(5.0f, 5.0f, -5.0f);

using namespace glm;

void Render(vec4 p0, vec4 p1, vec4 p2, vec2 t0, vec2 t1, vec2 t2, Model& data, WinRender& wr)
{
	// примен€ем трансформации и делаем перспективное деление
	mat4 transform = data.projection * data.view * data.model;
	vec4 temp = transform * p0;
	vec3 res1 = vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);
	temp = transform * p1;
	vec3 res2 = vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);
	temp = transform * p2;
	vec3 res3 = vec3(temp.x / temp.w, temp.y / temp.w, temp.z / temp.w);

	// ќтсекаем грани, что смотр€т в противоположную от зрител€ сторону
	vec3 normal = cross(res2 - res1, res3 - res1);
	if (dot(normal, data.ViewDir) > 0) {
		return;
	}

	std::array<glm::ivec3, 3> triangle = { {jd::Viewport3D(res1, 0, 0, wr.getWidth(), wr.getHeight(), wr.getDepth()),
			                   jd::Viewport3D(res2, 0, 0, wr.getWidth(), wr.getHeight(), wr.getDepth()),
			                   jd::Viewport3D(res3, 0, 0, wr.getWidth(), wr.getHeight(), wr.getDepth())} };
	data.Normal = normalize(cross(vec3(data.model * (p1 - p0)), vec3(data.model * (p2 - p0))));
	std::array<glm::vec2, 3> tex;
	tex[0] = t0; tex[1] = t1; tex[2] = t2;

	jd::DrawTriangle(triangle, tex, data, wr);
}

/*
ambient = vec3(0.0f, 0.05f, 0.0f),
*/

void rast_cube(WinRender& wr, const std::vector<vec4>& vertices, const std::vector<vec2>& tex)
{
	vec3 color = vec3(1.0f, 1.0f, 1.0f);
	mat4 model = mat4(1.0f);
	mat4 view = mat4(1.0f);
	mat4 projection = mat4(1.0f);

	view = glm::translate(view, fragPos);
	model = glm::scale(model, vec3(0.2f));
	model = glm::rotate(model, (float)glfwGetTime() * glm::radians(50.0f), vec3(0.5f, 1.0f, 1.0f));
	projection = glm::perspective(glm::radians(45.0f), (float)wr.getWidth() / wr.getHeight(), 0.1f, 100.f);

	Model data = { 
		model, 
		view, 
		projection, 
		vec3(0.0f), 
		fragPos - cameraPos, 
		fragPos, 
		lightPos, 
		color,
		std::move(jd::LoadTexture("box.png", true)),
		vec3(0.3f, 0.24f, 0.14f),
		vec3(0.7f, 0.42f, 0.26f),
		vec3(0.5f, 0.5f, 0.5f),
		64.0f
	};

	for (uint16_t i = 0; i < vertices.size(); i += 3) {
		Render(vertices[i], vertices[i + 1], vertices[i + 2], tex[i], tex[i + 1], tex[i + 2], data, wr);
	}
}

int main(void) 
{
	WinRender wr;
	if (!wr.init(WIDTH, HEIGHT, "rasterization project", "pixel.vert", "pixel.frag")) {
		return -1;
	}

	std::vector<vec4> vertices = {
		// задн€€ грань
		vec4(-1.0f, -1.0f, -1.0f, 1.0f), // нижн€€-лева€
		vec4(1.0f,  1.0f, -1.0f, 1.0f), // верхн€€-права€
		vec4(1.0f, -1.0f, -1.0f, 1.0f),  // нижн€€-права€         
		vec4(1.0f,  1.0f, -1.0f, 1.0f),  // верхн€€-права€
		vec4(-1.0f, -1.0f, -1.0f, 1.0f),  // нижн€€-лева€
		vec4(-1.0f,  1.0f, -1.0f, 1.0f),   // верхн€€-лева€

		// передн€€ грань
		vec4(-1.0f, -1.0f,  1.0f, 1.0f),  // нижн€€-лева€
		vec4(1.0f, -1.0f,  1.0f, 1.0f),  // нижн€€-права€
		vec4(1.0f,  1.0f,  1.0f, 1.0f),  // верхн€€-права€
		vec4(1.0f,  1.0f,  1.0f, 1.0f),   // верхн€€-права€
		vec4(-1.0f,  1.0f,  1.0f, 1.0f),   // верхн€€-лева€
		vec4(-1.0f, -1.0f,  1.0f, 1.0f),   // нижн€€-лева€

		 // грань слева
		vec4(-1.0f,  1.0f,  1.0f, 1.0f),  // верхн€€-права€
		vec4(-1.0f,  1.0f, -1.0f, 1.0f),  // верхн€€-лева€
		vec4(-1.0f, -1.0f, -1.0f, 1.0f),  // нижн€€-лева€
		vec4(-1.0f, -1.0f, -1.0f, 1.0f),  // нижн€€-лева€
		vec4(-1.0f, -1.0f,  1.0f, 1.0f), // нижн€€-права€
		vec4(-1.0f,  1.0f,  1.0f, 1.0f),  // верхн€€-права€

		   // грань справа
	    vec4(1.0f,  1.0f,  1.0f, 1.0f), // верхн€€-лева€
		vec4(1.0f, -1.0f, -1.0f, 1.0f), // нижн€€-права€
		vec4(1.0f,  1.0f, -1.0f, 1.0f), // верхн€€-права€         
		vec4(1.0f, -1.0f, -1.0f, 1.0f), // нижн€€-права€
		vec4(1.0f,  1.0f,  1.0f, 1.0f), // верхн€€-лева€
		vec4(1.0f, -1.0f,  1.0f, 1.0f), // нижн€€-лева€     

			   // нижн€€ грань
		vec4(-1.0f, -1.0f, -1.0f, 1.0f), // верхн€€-права€
		vec4(1.0f, -1.0f, -1.0f, 1.0f), // верхн€€-лева€
		vec4(1.0f, -1.0f,  1.0f, 1.0f), // нижн€€-лева€
		vec4(1.0f, -1.0f,  1.0f, 1.0f), // нижн€€-лева€
		vec4(-1.0f, -1.0f,  1.0f, 1.0f), // нижн€€-права€
		vec4(-1.0f, -1.0f, -1.0f, 1.0f), // верхн€€-права€

				   // верхн€€ грань
		vec4(-1.0f,  1.0f, -1.0f, 1.0f), // верхн€€-лева€
		vec4(1.0f,  1.0f , 1.0f, 1.0f), // нижн€€-права€
		vec4(1.0f,  1.0f, -1.0f, 1.0f), // верхн€€-права€     
		vec4(1.0f,  1.0f,  1.0f, 1.0f),  // нижн€€-права€
		vec4(-1.0f,  1.0f, -1.0f, 1.0f), // верхн€€-лева€
		vec4(-1.0f,  1.0f,  1.0f, 1.0f)  // нижн€€-лева€        
	};

	std::vector<vec2> textures = {
		vec2(0.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),

		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f),
		vec2(0.0f, 0.0f),

		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f),
		vec2(0.0f, 1.0f),
		vec2(0.0f, 0.0f),
		vec2(1.0f, 0.0f),

		vec2(1.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(0.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),

		vec2(0.0f, 1.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 0.0f),
		vec2(0.0f, 1.0f),

		vec2(0.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(1.0f, 1.0f),
		vec2(1.0f, 0.0f),
		vec2(0.0f, 1.0f),
		vec2(0.0f, 0.0f)
	};

	while (!wr.windowShouldClose())
	{
		wr.pollEvents();
		wr.processInput();

		wr.useRender();
		rast_cube(wr, vertices, textures);

		wr.flush(0.3f, 0.24f, 0.14f, 1.0f);
	}

	wr.shutdown();
	return 0;
}
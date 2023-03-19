#pragma once
#include <glm/glm.hpp>
#include "Display.h"
#include <array>
#include <vector>

namespace jd
{
    // Render functions
    void DrawLine(glm::ivec2 p0, glm::ivec2 p1, glm::vec3 color, Display& render);
    glm::vec3 barycentric(const std::array<glm::ivec3, 3>& pts, const glm::ivec3 P);
    void DrawTriangle(const std::array<glm::ivec3, 3>& pts, const std::array<glm::vec2, 3>& tex, Model& data, Display& render);

    // Transforms
    glm::ivec2 Viewport2D(glm::vec3 vec, int x, int y, int width, int height);
    glm::ivec3 Viewport3D(glm::vec3 vec, int x, int y, int width, int height, int depth);

    // Trianglulation algo
    // @return: vector of vertex indexes
    [[nodiscard]] std::vector<glm::ivec3> TriangulatePolygon(const std::vector<glm::vec3> vertex, glm::vec3 Normal);

    // Texture
    [[nodiscard]] Texture LoadTexture(const char* filename, bool inverse);

    // Main programm function
    void startProgramm(Window& window);
}

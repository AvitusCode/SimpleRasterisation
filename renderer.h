#pragma once
#include <glm/glm.hpp>
#include "Model.h"
#include "window.h"
#include <array>

namespace jd
{
    // Render functions
    void DrawLine(glm::ivec2 p0, glm::ivec2 p1, glm::vec3 color, WinRender& render);
    glm::vec3 barycentric(const std::array<glm::ivec3, 3>& pts, const glm::ivec3 P);
    void DrawTriangle(const std::array<glm::ivec3, 3>& pts, const std::array<glm::vec2, 3>& tex, Model& data, WinRender& render);

    // Transforms
    glm::ivec2 Viewport2D(glm::vec3 vec, int x, int y, int width, int height);
    glm::ivec3 Viewport3D(glm::vec3 vec, int x, int y, int width, int height, int depth);

    // Texture
    [[nodiscard]] Texture LoadTexture(const char* filename, bool inverse);
}


template <typename T> 
T CLAMP(const T& value, const T& low, const T& high) {
    return value < low ? low : (value > high ? high : value);
}
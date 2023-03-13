#pragma once
#include <glm/glm.hpp>

namespace jd
{
	// POINT TYPE
	template<typename T>
	struct point_t
	{
		T x;
		T y;

		operator glm::ivec2() const {
			return glm::ivec2(static_cast<int>(x), static_cast<int>(y));
		}
	};

	using ipoint = point_t<int>;
	using fpoint = point_t<float>;
	using dpoint = point_t<double>;

	// END POINT TYPE


	// CURVE TYPE
	template<typename T>
	struct curve_t
	{
		point_t<T> point;
		T data;
		curve_t* nextPoint = nullptr;
	};

	// END CURVE TYPE
}
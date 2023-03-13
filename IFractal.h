#pragma once
#include "Display.h"

class IFractal
{
public:
	virtual void CurveCreate(float ax, float ay, float bx, float by) = 0;
	virtual void CurveNext() = 0;
	virtual void CurvePrev() = 0;
	virtual void CurveDestroy() noexcept = 0;

	virtual void CurveDraw(jd::Display& display, glm::vec3 color) const = 0;

	virtual ~IFractal() = default;
};
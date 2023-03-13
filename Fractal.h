#pragma once
#include "IFractal.h"
#include "mObjects.h"

class mFractal : public IFractal
{
public:
	mFractal() = default;
	void CurveCreate(float ax, float ay, float bx, float by) override;
	void CurveNext() override;
	void CurvePrev() override;
	void CurveDestroy() noexcept override;

	~mFractal() noexcept;
	void CurveDraw(jd::Display& display, glm::vec3 color) const override;
private:
	jd::curve_t<float>* head = nullptr;
};
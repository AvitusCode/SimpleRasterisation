#pragma once
#include "IFractal.h"
#include "mObjects.h"
#include "allocator/allocator.h"

class mFractal : public IFractal
{
public:
	mFractal(jd::mem::Allocator* allocator = nullptr);
	void CurveCreate(float ax, float ay, float bx, float by) override;
	void CurveNext() override;
	void CurvePrev() override;
	void CurveDestroy() noexcept override;

	~mFractal() noexcept;
	void CurveDraw(jd::Display& display, glm::vec3 color) const override;

private:
	template<typename... Args>
	jd::curve_t<float>* emplace(Args&&... args) {
		if (!m_allocator) {
			return new jd::curve_t<float>(std::forward<Args>(args)...);
		}

		void* raw_ptr = m_allocator->allocate(sizeof(jd::curve_t<float>), alignof(jd::curve_t<float>));
		jd::curve_t<float>* m_ptr = new (raw_ptr) jd::curve_t<float>(std::forward<Args>(args)...);

		return m_ptr;
	}

	void clear(jd::curve_t<float>*& ptr) noexcept;

private:
	jd::mem::Allocator* m_allocator = nullptr;
	jd::curve_t<float>* head = nullptr;
	size_t n_curves = 0;
};
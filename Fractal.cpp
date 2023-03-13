#include "Fractal.h"
#include "renderer.h"

using curve = jd::curve_t<float>;

void mFractal::CurveCreate(float ax, float ay, float bx, float by)
{
	curve* m_curve = new curve;
	curve* next = new curve;

	m_curve->point.x = ax;
	m_curve->point.y = ay;
	m_curve->data = 0.0f;
	m_curve->nextPoint = next;

	next->point.x = bx;
	next->point.y = by;
	next->data = 1.0f;

	head = m_curve;
}

void mFractal::CurveNext()
{
	if (!head || !head->nextPoint) {
		return;
	}

	int direction = 1;
	curve* m_curve = head;

	// inserting intermediate points
	do {
		curve* mid_curve = new curve;
		mid_curve->nextPoint = m_curve->nextPoint;

		float dx = m_curve->nextPoint->point.x - m_curve->point.x;
		float dy = m_curve->nextPoint->point.y - m_curve->point.y;

		float cx = 0.5f * (m_curve->point.x + m_curve->nextPoint->point.x + dy * direction);
		float cy = 0.5f * (m_curve->point.y + m_curve->nextPoint->point.y - dx * direction);

		m_curve->nextPoint = mid_curve;

		// calculate
		mid_curve->point.x = cx;
		mid_curve->point.y = cy;
		mid_curve->data = (mid_curve->nextPoint->data - m_curve->data) / 2.0f;

		m_curve = mid_curve->nextPoint;
		direction = -direction;
	} while (m_curve->nextPoint);
}

void mFractal::CurvePrev()
{
	curve* next = head;
	if (!next || !next->nextPoint->nextPoint) {
		return;
	}

	while (next->nextPoint)
	{
		curve* temp = next->nextPoint;
		next->nextPoint = temp->nextPoint;
		next = temp->nextPoint;
		delete temp;
	}
}

void mFractal::CurveDestroy() noexcept
{
	curve* temp = head;
	
	if (!temp) {
		return;
	}
	else if (!temp->nextPoint) {
		delete temp;
		return;
	}

	curve* next = temp;

	while (next)
	{
		next = temp->nextPoint;
		delete temp;
		temp = next;
	}
}

void mFractal::CurveDraw(jd::Display& display, glm::vec3 color) const
{
	if (!head || !head->nextPoint) {
		return;
	}

	for (curve* temp = head, *tempNext = temp->nextPoint; tempNext; temp = tempNext, tempNext = tempNext->nextPoint) {
		jd::DrawLine(temp->point, tempNext->point, color, display);
	}
}

mFractal::~mFractal() noexcept {
	CurveDestroy();
}
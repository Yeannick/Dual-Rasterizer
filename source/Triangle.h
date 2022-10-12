#pragma once
#include "structs.h"
#include "Mesh.h"
using IPoint2 = Elite::IPoint2;
using FPoint2 = Elite::FPoint2;

class Triangle
{
public:
	Triangle() = delete;
	Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2);
	Triangle(const FPoint3& v0, const FPoint3& v1, const FPoint3& v2);
	Triangle(const FPoint3& v0, const RGBColor& v0Color, const FPoint3& v1, const RGBColor& v1Color, const FPoint3& v2, const RGBColor& v2Color);
	~Triangle();

	void SwapV1V2();

	Vertex GetV0() const;
	Vertex GetV1() const;
	Vertex GetV2() const;

	float GetW0() const;
	float GetW1() const;
	float GetW2() const;

	static void GetBounds(IPoint2& topLeft, IPoint2& btmRight, int screenWidth, int screenHeight, const FPoint2& v0, const FPoint2& v1, const FPoint2& v2);
	bool IsPointInTriangle(const FPoint2& point, const FPoint2& v0, const FPoint2& v1, const FPoint2& v2);

private:

	Vertex m_v0;
	Vertex m_v1;
	Vertex m_v2;

	float m_w0;
	float m_w1;
	float m_w2;
};

#include "pch.h"
#include "Triangle.h"

Triangle::Triangle(const Vertex& v0, const Vertex& v1, const Vertex& v2):
	m_v0{v0},
	m_v1{v1},
	m_v2{v2}
{
}

//Triangle::Triangle(const FPoint3& v0, const FPoint3& v1, const FPoint3& v2)
//	: m_v0{ v0, RGBColor{1.f, 1.f, 1.f,} }
//	, m_v1{ v1, RGBColor{1.f, 1.f, 1.f,} }
//	, m_v2{ v2, RGBColor{1.f, 1.f, 1.f,} }
//{
//}
//
//Triangle::Triangle(const FPoint3& v0, const RGBColor& v0Color, const FPoint3& v1, const RGBColor& v1Color, const FPoint3& v2, const RGBColor& v2Color)
//	: m_v0{ v0, v0Color }
//	, m_v1{ v1, v1Color }
//	, m_v2{ v2, v2Color }
//{
//}

Triangle::~Triangle()
{
}

void Triangle::SwapV1V2()
{
	Vertex temp = m_v1;
	m_v1 = m_v2;
	m_v2 = temp;
}

Vertex Triangle::GetV0() const
{
	return m_v0;
}

Vertex Triangle::GetV1() const
{
	return m_v1;
}

Vertex Triangle::GetV2() const
{
	return m_v2;
}

float Triangle::GetW0() const
{
	return m_w0;
}

float Triangle::GetW1() const
{
	return m_w1;
}

float Triangle::GetW2() const
{
	return m_w2;
}

void Triangle::GetBounds(IPoint2& topLeft, IPoint2& btmRight, int screenWidth, int screenHeight, const FPoint2& v0, const FPoint2& v1, const FPoint2& v2)
{
	topLeft = { int(v0.x), int(v0.y) };

	topLeft.x = std::min(int(v1.x), topLeft.x);
	topLeft.x = std::min(int(v2.x), topLeft.x);
	topLeft.x = std::min(topLeft.x, screenWidth - 1);
	topLeft.x = std::max(topLeft.x, 0);

	topLeft.y = std::min(int(v1.y), topLeft.y);
	topLeft.y = std::min(int(v2.y), topLeft.y);
	topLeft.y = std::min(topLeft.y, screenHeight - 1);
	topLeft.y = std::max(topLeft.y, 0);

	btmRight = { int(v0.x), int(v0.y) };

	btmRight.x = std::max(int(v1.x), btmRight.x);
	btmRight.x = std::max(int(v2.x), btmRight.x);
	btmRight.x = std::min(btmRight.x, screenWidth - 1);
	btmRight.x = std::max(btmRight.x, 0);

	btmRight.y = std::max(int(v1.y), btmRight.y);
	btmRight.y = std::max(int(v2.y), btmRight.y);
	btmRight.y = std::min(btmRight.y, screenHeight - 1);
	btmRight.y = std::max(btmRight.y, 0);

	topLeft.x -= 1;
	topLeft.y -= 1;
	btmRight.x += 1;
	btmRight.y += 1;

	topLeft.x = Elite::Clamp(topLeft.x, 0, screenWidth - 1);
	topLeft.y = Elite::Clamp(topLeft.y, 0, screenHeight - 1);
	btmRight.x = Elite::Clamp(btmRight.x, 0, screenWidth - 1);
	btmRight.y = Elite::Clamp(btmRight.y, 0, screenHeight - 1);
}

bool Triangle::IsPointInTriangle(const FPoint2& point, const FPoint2& v0, const FPoint2& v1, const FPoint2& v2)
{
	const FVector2 edgeA{ v1 - v0 };
	FVector2 pointToSide{ point - v0 };

	if (Cross(edgeA, pointToSide) <= 0)
	{
		m_w2 = Cross(edgeA, pointToSide) / Cross(v0 - v1, v0 - v2);

		const FVector2 edgeB{ v2 - v1 };
		pointToSide = { point - v1 };

		if (Cross(edgeB, pointToSide) <= 0)
		{
			m_w0 = Cross(edgeB, pointToSide) / Cross(v0 - v1, v0 - v2);

			const FVector2 edgeC{ v0 - v2 };
			pointToSide = { point - v2 };

			if (Cross(edgeC, pointToSide) <= 0)
			{
				m_w1 = Cross(edgeC, pointToSide) / Cross(v0 - v1, v0 - v2);

				return true;
			}
		}
	}

	return false;
}

#pragma once
#include "EMath.h"
#include "ERGBColor.h"

#include <vector>
#include <string>

using RGBColor = Elite::RGBColor;
using FPoint3 = Elite::FPoint3;
using FVector2 = Elite::FVector2;
using FVector3 = Elite::FVector3;

struct VertexOutput
{
	bool operator==(const VertexOutput& rhs) const
	{
		return (position == rhs.position && uv == rhs.uv && normal == rhs.normal);
	}

	FPoint3 position{};
	RGBColor color = RGBColor{ 1.f, 1.f, 1.f };
	FVector2 uv{};
	FVector3 normal{};
	FVector3 tangent{};
};

// Obj  structs below:
struct Comment
{
	std::string text;
	unsigned int lineNr;
};

struct VertexInput
{
	float x, y, z;
	float w = 1.0f;
};

struct VertexTexture
{
	float u, v;
	float w = 0.f;
};

struct VertexNormal
{
	float x, y, z;
};

struct Face
{
	std::vector<uint32_t> vertexIds;
	std::vector<unsigned int> vertexTextureIds;
	std::vector<unsigned int> vertexNormalIds;
};
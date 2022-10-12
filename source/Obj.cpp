#include "pch.h"
#include "Obj.h"

Obj::Obj(const std::string& filename)
{
	if (filename.find(".obj") != filename.npos)
	{
		LoadFromObj(filename);
	}
	else
	{
		throw std::runtime_error("Obj() constructor failed - File extension is not .obj or .bobj\n");
		return;
	}
}

Obj::~Obj()
{
}

void Obj::LoadFromObj(const std::string& filename)
{
	std::ifstream input(filename);
	if (!input)
	{
		throw std::runtime_error("LoadFromObj() failed - Could not open file " + filename + " for reading\n");
		return;
	}

	unsigned int curLineNr{ 1 };
	std::string curLine;
	while (std::getline(input, curLine))
	{
		if (curLine.find("#") != std::string::npos)
		{
			ParseComment(curLine, curLineNr);
		}
		else if (curLine.find("v ") != std::string::npos)
		{
			ParseVertex(curLine);
		}
		else if (curLine.find("vn ") != std::string::npos)
		{
			ParseVertexNormal(curLine);
		}
		else if (curLine.find("vt ") != std::string::npos)
		{
			ParseVertexTexture(curLine);
		}
		else if (curLine.find("f ") != std::string::npos)
		{
			ParseFace(curLine);
		}
		else
		{
			curLineNr--;
		}

		curLineNr++;
	}

	input.close();
}

void Obj::ParseComment(const std::string& line, unsigned int lineNr)
{
	Comment c{ line, lineNr };
	m_Comments.push_back(c);
}

void Obj::ParseVertex(const std::string& line)
{
	const char delim{ ' ' };
	std::stringstream vStream(line);

	std::string x;
	std::getline(vStream, x, delim);
	std::getline(vStream, x, delim);
	std::getline(vStream, x, delim);
	std::string y;
	std::getline(vStream, y, delim);

	std::string z;
	std::getline(vStream, z, delim);

	std::string w{ "1" };
	std::getline(vStream, w, delim);

	//std::cout << x << ", " << y << ", " << z << ", " << w << std::endl;

	VertexInput v{ stof(x), stof(y), stof(z), stof(w) };
	m_Vertices.push_back(v);
}

void Obj::ParseVertexNormal(const std::string& line)
{
	const char delim{ ' ' };
	std::stringstream vnStream(line);

	std::string x;
	std::getline(vnStream, x, delim);
	std::getline(vnStream, x, delim);

	std::string y;
	std::getline(vnStream, y, delim);

	std::string z;
	std::getline(vnStream, z, delim);

	//std::cout << x << ", " << y << ", " << z << std::endl;

	VertexNormal vn{ stof(x), stof(y), stof(z) };
	m_VertexNormals.push_back(vn);
}

void Obj::ParseVertexTexture(const std::string& line)
{
	const char delim{ ' ' };
	std::stringstream vtStream(line);

	std::string u;
	std::getline(vtStream, u, delim);
	std::getline(vtStream, u, delim);

	std::string v;
	std::getline(vtStream, v, delim);

	std::string w{ "0" };
	std::getline(vtStream, w, delim);

	//std::cout << x << ", " << y << ", " << z << std::endl;

	VertexTexture vt{ stof(u), stof(v), stof(w) };
	m_VertexTextures.push_back(vt);
}

void Obj::ParseFace(const std::string& line)
{
	const char delim{ ' ' };
	std::stringstream faceStream(line);

	std::string vInfo;
	std::getline(faceStream, vInfo, delim);

	Face f;
	while (std::getline(faceStream, vInfo, delim))
	{
		if (vInfo.find("//") != std::string::npos)
		{
			std::stringstream vertexStream(vInfo);

			std::string v;
			std::getline(vertexStream, v, '/');

			std::string vn;
			std::getline(vertexStream, vn, '/');
			std::getline(vertexStream, vn, '/');

			f.vertexIds.push_back(stoi(v));
			f.vertexTextureIds.push_back(0);
			f.vertexNormalIds.push_back(stoi(vn));
		}
		else if (vInfo.find('/') != std::string::npos)
		{
			std::stringstream vertexStream(vInfo);

			std::string v;
			std::getline(vertexStream, v, '/');

			std::string vt;
			std::getline(vertexStream, vt, '/');

			std::string vn;
			std::getline(vertexStream, vn, '/');

			f.vertexIds.push_back(stoi(v));
			f.vertexTextureIds.push_back(stoi(vt));
			f.vertexNormalIds.push_back(stoi(vn));
		}
		else
		{
			f.vertexTextureIds.push_back(0);
			f.vertexNormalIds.push_back(0);
			f.vertexIds.push_back(stoi(vInfo));
		}
	}

	m_Faces.push_back(f);
}

std::vector<VertexInput> Obj::GetVerticesList()
{
	return m_Vertices;
}

std::vector<VertexTexture> Obj::GetVerticesTexturesList()
{
	return m_VertexTextures;
}

std::vector<VertexNormal> Obj::GetVerticesNormalsList()
{
	return m_VertexNormals;
}

std::vector<Face> Obj::GetFacesList()
{
	return m_Faces;
}

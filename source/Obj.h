#pragma once
#include <vector>
#include <string>
#include <fstream>

#include "Mesh.h"
#include "structs.h"
#include <sstream>
#include <iostream>

class Obj
{
public:
	Obj(const std::string& filename);
	~Obj();

	void LoadFromObj(const std::string& filename);

	void ParseComment(const std::string& line, unsigned int lineNr);
	void ParseVertex(const std::string& line);
	void ParseVertexNormal(const std::string& line);
	void ParseVertexTexture(const std::string& line);
	void ParseFace(const std::string& line);

	std::vector<VertexInput> GetVerticesList();
	std::vector<VertexTexture> GetVerticesTexturesList();
	std::vector<VertexNormal> GetVerticesNormalsList();
	std::vector<Face> GetFacesList();
private:
	std::vector<Comment> m_Comments;
	std::vector<VertexInput> m_Vertices;
	std::vector<VertexTexture> m_VertexTextures;
	std::vector<VertexNormal> m_VertexNormals;
	std::vector<Face> m_Faces;
};


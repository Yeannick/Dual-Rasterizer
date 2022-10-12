#pragma once
#include "Mesh.h"
using namespace Elite;
enum class RenderMode 
{
	Rasterizer = -1,
	DirectX = 1
};
class SceneGraph
{
public:
	static SceneGraph* GetInstance();
	void AddObjectToGraph(Mesh* pMesh);
	void RemoveFromGraph(Mesh* pMesh);
	void ClearScene();
	~SceneGraph();

	void ToggleRenderMode();
	const RenderMode GetRenderMode() const;
	std::vector<Mesh*> m_pMeshes;

private:
	SceneGraph(); // private constructor cause it's a singleton
	static SceneGraph* m_instance;
	RenderMode m_CurrentRenderMode;
};

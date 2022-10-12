#include "pch.h"
#include "SceneGraph.h"


SceneGraph* SceneGraph::m_instance{ nullptr };

SceneGraph::SceneGraph():
	m_CurrentRenderMode{ RenderMode::DirectX }
{
	ClearScene();
}

SceneGraph::~SceneGraph()
{
	ClearScene();
}

void SceneGraph::ToggleRenderMode()
{
	
	m_CurrentRenderMode = RenderMode(int(m_CurrentRenderMode) * -1);
	std::cout << "Using Render Mode: ";
	switch (m_CurrentRenderMode)
	{
	case RenderMode::Rasterizer:
		std::cout << "Rasterizer\n";
		break;
	case RenderMode::DirectX:
		std::cout << "DirectX\n";
		break;
	default:
		break;
	}
	
}

const RenderMode SceneGraph::GetRenderMode() const
{
	 return m_CurrentRenderMode;
}

void SceneGraph::ClearScene()
{
	for (Mesh* pTriangleMesh : m_pMeshes)
	{
		delete pTriangleMesh;
		pTriangleMesh = nullptr;
	}

	m_pMeshes.clear();
}

SceneGraph* SceneGraph::GetInstance()
{
	if (m_instance == nullptr)
	{
		m_instance = new SceneGraph();
	}

	return m_instance;
}

void SceneGraph::AddObjectToGraph(Mesh* pMesh)
{
	m_pMeshes.push_back(pMesh);
}

void SceneGraph::RemoveFromGraph(Mesh* pMesh)
{
	auto it = std::find(m_pMeshes.begin(),m_pMeshes.end(),pMesh);
//	delete *it;
	if (it != m_pMeshes.end())
	{
		m_pMeshes.erase(it);
	}
	
}

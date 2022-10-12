#pragma once
#include <vector>
#include "BaseEffect.h"
#include "Texture.h"
#include "Obj.h"
#include <string>

struct Vertex
{
	bool operator==(const Vertex& rhs) const
	{
		return (Position == rhs.Position && Uv == rhs.Uv);
	}

	Elite::FPoint3 Position;
	Elite::FVector2 Uv;
	Elite::FVector3 Normal;
	Elite::FVector3 Tangent;
	Elite::RGBColor Color;
};
enum class PrimitiveTopology
{
	TriangleList,
	TriangleStrip
};
enum FilterMethod
{
	point = 0,
	linear = 1,
	anisotropic = 2

};
class Mesh
{
public:
	
	Mesh(const Elite::FPoint3 position, const std::string& texturePath, const std::string& normalMapPath, const std::string& specularMapPath, const std::string& glossinessMapPath, ID3D11Device* pDevice, BaseEffect* effect , std::string effectFile , PrimitiveTopology topology = PrimitiveTopology::TriangleList );
	~Mesh();
	// DirectX11 only
	void Render(ID3D11DeviceContext* pDeviceContext);
	const Elite::FMatrix4& GetWorldMatrix()const;
	BaseEffect* GetEffect()const { return m_pEffect; }
	static void ToggleFilterMethod();
	std::vector<Vertex> GetFlippedZVertices();

	// Software and rasterizer
	const Texture* GetTexture() const;
	const Texture* GetNormalMap() const;
	const Texture* GetSpecularMap() const;
	const Texture* GetGlossinessMap() const;
	
	const Elite::RGBColor SampleTexture(const Elite::FVector2& uv) const;
	PrimitiveTopology GetTopologyType() { return m_Topology; }
	std::vector<Vertex> GetVertexBuffer()const;
	std::vector<uint32_t>GetIndexBuffer()const;
	Elite::FPoint3 GetPosition();
	Elite::FVector3 GetRotation();

	// rasterizer Only
	void RotateY(float amount, float elapsedSec);

	void ReadObj(std::string file);

	void ToggleRender();
	bool CanRender() { return m_CanRander; }
	
private:
	std::wstring m_AssetPath{ };
	BaseEffect* m_pEffect = nullptr;
	

	Texture* m_Texture;
	Texture* m_NormalMap;
	Texture* m_SpecularMap;
	Texture* m_GlossinessMap;

	ID3D11InputLayout* m_pVertexLayout = nullptr;
	ID3D11Buffer* m_pVertexBuffer = nullptr;
	ID3D11Buffer* m_pIndexBuffer = nullptr;
	uint32_t m_AmountOfIndices = 0;
	Elite::FMatrix4 m_WorldMatrix;
	std::vector<Vertex> m_VertexBuffer;
	std::vector<uint32_t> m_IndexBuffer;
	static FilterMethod m_FilterMethod;


	Elite::FPoint3 m_Position;
	Elite::FVector3 m_Rotation;
	PrimitiveTopology m_Topology;

	bool m_CanRander = true;
};


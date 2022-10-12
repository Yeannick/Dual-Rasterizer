#include "pch.h"
#include "Mesh.h"
#include "MaterialEffect.h"

FilterMethod Mesh::m_FilterMethod = FilterMethod::point;


Mesh::Mesh(const Elite::FPoint3 position, const std::string& texturePath, const std::string& normalMapPath, const std::string& specularMapPath, const std::string& glossinessMapPath, ID3D11Device* pDevice, BaseEffect* effect,  std::string File, PrimitiveTopology topology):
	m_WorldMatrix(Elite::MakeTranslation(Elite::FVector3{ position })),
	m_Texture( new Texture{ texturePath, pDevice }),
	m_NormalMap{ new Texture{normalMapPath, pDevice} },
	m_SpecularMap{ new Texture{specularMapPath, pDevice} },
	m_GlossinessMap{ new Texture{glossinessMapPath, pDevice } },
	m_pEffect {effect},
	m_Position{position},
	m_Topology {topology}

{
	
	ReadObj(File);
	// Create Vertex layout

	HRESULT result = S_OK;
	static const uint32_t numElement{ 5 };
	D3D11_INPUT_ELEMENT_DESC vertexDesc[numElement]{};

	vertexDesc[0].SemanticName = "POSITION";
	vertexDesc[0].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[0].AlignedByteOffset = 0;
	vertexDesc[0].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[1].SemanticName = "TEXCOORD";
	vertexDesc[1].Format = DXGI_FORMAT_R32G32_FLOAT;
	vertexDesc[1].AlignedByteOffset = 12;
	vertexDesc[1].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[2].SemanticName = "NORMAL";
	vertexDesc[2].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[2].AlignedByteOffset = 20;
	vertexDesc[2].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[3].SemanticName = "TANGENT";
	vertexDesc[3].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[3].AlignedByteOffset = 32;
	vertexDesc[3].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	vertexDesc[4].SemanticName = "COLOR";
	vertexDesc[4].Format = DXGI_FORMAT_R32G32B32_FLOAT;
	vertexDesc[4].AlignedByteOffset = 44;
	vertexDesc[4].InputSlotClass = D3D11_INPUT_PER_VERTEX_DATA;

	//Create the input layout

	D3DX11_PASS_DESC passDesc;
	m_pEffect->GetTechnique()->GetPassByIndex(0)->GetDesc(&passDesc);
	result = pDevice->CreateInputLayout(
		vertexDesc,
		numElement,
		passDesc.pIAInputSignature,
		passDesc.IAInputSignatureSize,
		&m_pVertexLayout);
	if (FAILED(result))
	{
		return;
	}

	// Create Vertex buffer
	std::vector<Vertex> flippedVertices = GetFlippedZVertices();

	D3D11_BUFFER_DESC bd = {};
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(Vertex) * (uint32_t)m_VertexBuffer.size();
	bd.BindFlags = D3D11_BIND_VERTEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	D3D11_SUBRESOURCE_DATA initData = { 0 };
	initData.pSysMem = flippedVertices.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pVertexBuffer);
	if (FAILED(result))
	{
		return;
	}

	// Create index Buffer

	m_AmountOfIndices = (uint32_t)m_IndexBuffer.size();
	bd.Usage = D3D11_USAGE_IMMUTABLE;
	bd.ByteWidth = sizeof(uint32_t) * m_AmountOfIndices;
	bd.BindFlags = D3D11_BIND_INDEX_BUFFER;
	bd.CPUAccessFlags = 0;
	bd.MiscFlags = 0;
	initData.pSysMem = m_IndexBuffer.data();
	result = pDevice->CreateBuffer(&bd, &initData, &m_pIndexBuffer);
	if (FAILED(result))
	{
		return;
	}
}


Mesh::~Mesh()
{
	if (m_pVertexLayout)
	{
		m_pVertexLayout->Release();
	}
	if (m_pVertexBuffer)
	{
		m_pVertexBuffer->Release();
	}
	if (m_pIndexBuffer)
	{
		m_pIndexBuffer->Release();
	}
	delete m_Texture;
	delete m_NormalMap;
	delete m_SpecularMap;
	delete m_GlossinessMap;
}

void Mesh::Render(ID3D11DeviceContext* pDeviceContext)
{
	// Set vertex Buffer

	UINT stride = sizeof(Vertex);
	UINT offset = 0;
	pDeviceContext->IASetVertexBuffers(0, 1, &m_pVertexBuffer, &stride, &offset);

	// set indexBuffer
	pDeviceContext->IASetIndexBuffer(m_pIndexBuffer, DXGI_FORMAT_R32_UINT, 0);

	// Set inPutLayout
	pDeviceContext->IASetInputLayout(m_pVertexLayout);

	//Set primitive Topology
	pDeviceContext->IASetPrimitiveTopology(D3D11_PRIMITIVE_TOPOLOGY_TRIANGLELIST);

	//Render a triangle
	D3DX11_TECHNIQUE_DESC techDesc;
	m_pEffect->GetTechnique()->GetDesc(&techDesc);
	for (UINT p = 0; p < techDesc.Passes; ++p)
	{
		m_pEffect->GetTechnique()->GetPassByIndex(m_FilterMethod)->Apply(0, pDeviceContext);
		pDeviceContext->DrawIndexed(m_AmountOfIndices, 0, 0);
	}
}

void Mesh::RotateY(float amount, float elapsedSec)
{
	m_Rotation.y += elapsedSec * amount * float(E_TO_RADIANS);
}

void Mesh::ReadObj(std::string file)
{
	Obj File{ file };
	auto verticesList = File.GetVerticesList();
	auto textCoord = File.GetVerticesTexturesList();
	auto faceList = File.GetFacesList();
	auto NormalList = File.GetVerticesNormalsList();
	std::vector<Vertex>VertexBuffer;
	std::vector<uint32_t> indexBuffer;
	for (Face f : faceList)
	{
		for (int i{}; i < 3; i++)
		{
			Vertex vo{};

			vo.Position.x = verticesList[f.vertexIds[i] - 1].x;
			vo.Position.y = verticesList[f.vertexIds[i] - 1].y;
			vo.Position.z = verticesList[f.vertexIds[i] - 1].z;

			vo.Uv.x = textCoord[f.vertexTextureIds[i] - 1].u;
			vo.Uv.y = 1 - textCoord[f.vertexTextureIds[i] - 1].v;

			vo.Normal.x = NormalList[f.vertexNormalIds[i] - 1].x;
			vo.Normal.y = NormalList[f.vertexNormalIds[i] - 1].y;
			vo.Normal.z = NormalList[f.vertexNormalIds[i] - 1].z;

			std::vector<Vertex>::iterator it = std::find(VertexBuffer.begin(), VertexBuffer.end(), vo);

			if (it != VertexBuffer.end())
			{
				indexBuffer.push_back(unsigned int(it - VertexBuffer.begin()));
			}
			else
			{
				VertexBuffer.push_back(vo);
				indexBuffer.push_back(unsigned int(VertexBuffer.size() - 1));
			}
		}
	}
	for (uint32_t i{}; i < indexBuffer.size(); i += 3)
	{
		uint32_t index0 = indexBuffer[i];
		uint32_t index1 = indexBuffer[i + 1];
		uint32_t index2 = indexBuffer[i + 2];

		const FPoint3& p0 = VertexBuffer[index0].Position;
		const FPoint3& p1 = VertexBuffer[index1].Position;
		const FPoint3& p2 = VertexBuffer[index2].Position;
		const FVector2& uv0 = VertexBuffer[index0].Uv;
		const FVector2& uv1 = VertexBuffer[index1].Uv;
		const FVector2& uv2 = VertexBuffer[index2].Uv;

		const FVector3 edge0 = p1 - p0;
		const FVector3 edge1 = p2 - p0;
		const FVector2 diffX = FVector2{ uv1.x - uv0.x, uv2.x - uv0.x };
		const FVector2 diffY = FVector2{ uv1.y - uv0.y, uv2.y - uv0.y };
		float r = 1.f / Cross(diffX, diffY);

		FVector3 tangent = (edge0 * diffY.y - edge1 * diffY.x) * r;
		VertexBuffer[index0].Tangent += tangent;
		VertexBuffer[index1].Tangent += tangent;
		VertexBuffer[index2].Tangent += tangent;
	}

	for (auto& v : VertexBuffer)
	{
		v.Tangent = Elite::GetNormalized(Reject(v.Tangent, v.Normal));
	}

	m_VertexBuffer = VertexBuffer;
	m_IndexBuffer = indexBuffer;

}

void Mesh::ToggleRender()
{
	if (m_CanRander)
	{
		m_CanRander = false;
	}
	else
	{
		m_CanRander = true;
	}
}

const Elite::FMatrix4& Mesh::GetWorldMatrix() const
{
	return m_WorldMatrix;
}

const Texture* Mesh::GetTexture() const
{
	return m_Texture;
}

const Texture* Mesh::GetNormalMap() const
{
	return m_NormalMap;
}

const Texture* Mesh::GetSpecularMap() const
{
	return m_SpecularMap;
}

const Texture* Mesh::GetGlossinessMap() const
{
	return m_GlossinessMap;
}

std::vector<Vertex> Mesh::GetFlippedZVertices()
{
	std::vector<Vertex> flippedZVertices{};

	for (const Vertex& currentVertex : m_VertexBuffer)
	{
		Elite::FPoint3 flippedPosition = { currentVertex.Position.x, currentVertex.Position.y, -currentVertex.Position.z };
		Elite::FVector3 flippedNormals = { currentVertex.Normal.x , currentVertex.Normal.y, -currentVertex.Normal.z };
		Elite::FVector3 flippedTangent = { currentVertex.Tangent.x , currentVertex.Tangent.y, -currentVertex.Tangent.z };

		Vertex flippedVertex = { flippedPosition,currentVertex.Uv,flippedNormals,currentVertex.Tangent,currentVertex.Color };

		flippedZVertices.push_back(flippedVertex);
	}
	return flippedZVertices;
}
const Elite::RGBColor Mesh::SampleTexture(const Elite::FVector2& uv) const
{
	return m_Texture->Sample(uv);
}

std::vector<Vertex> Mesh::GetVertexBuffer() const
{
	return m_VertexBuffer;
}

std::vector<uint32_t> Mesh::GetIndexBuffer() const
{
	return m_IndexBuffer;
}

Elite::FPoint3 Mesh::GetPosition()
{
	return m_Position;
}

Elite::FVector3 Mesh::GetRotation()
{
	return m_Rotation;
}

void Mesh::ToggleFilterMethod()
{
	if (m_FilterMethod == FilterMethod::point)
	{
		m_FilterMethod = FilterMethod::linear;
		std::cout << "Linear \n";
	}
	else if (m_FilterMethod == FilterMethod::linear)
	{
		m_FilterMethod = FilterMethod::anisotropic;
		std::cout << "Anisotropic \n";

	}
	else if (m_FilterMethod == FilterMethod::anisotropic)
	{
		m_FilterMethod = FilterMethod::point;
		std::cout << "Point \n";

	}
}


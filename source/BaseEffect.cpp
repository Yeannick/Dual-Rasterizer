#include "pch.h"
#include "BaseEffect.h"
#include <sstream>

BaseEffect::BaseEffect(ID3D11Device* pDevice, const std::wstring& assetPath)
	: m_Type{ }
	, m_pEffect{ }
	, m_pTechnique{ }
	, m_pMatWorldViewProjVariable{ }
	, m_pMatWorldMatrixVariable{ }
	, m_pDiffuseMapVariable { }
{
	m_pEffect = LoadEffect(pDevice, assetPath);

	m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pTechnique->IsValid())
	{
		std::wcout << L"Technique not valid\n";
	}

	// Matrices 

	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"m_pMatWorldViewProjVariable not valid\n";
	}
	m_pMatWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatWorldMatrixVariable->IsValid())
		std::wcout << L"MatWorldMatrixVariable not valid\n";

	//Textures
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"DiffuseMapVariable not valid\n";
}

BaseEffect::~BaseEffect()
{
	if (m_pEffect)
		m_pEffect->Release();

	
}

EffectType BaseEffect::GetEffectType() const
{
	return m_Type;
}

ID3DX11Effect* BaseEffect::GetEffect() const
{
	return m_pEffect;
}

ID3DX11EffectTechnique* BaseEffect::GetTechnique() const
{
	return m_pTechnique;
}

ID3DX11EffectMatrixVariable* BaseEffect::GetWorldViewProjectionMatrix() const
{
	return m_pMatWorldViewProjVariable;
}

void BaseEffect::SetWorldViewProjectionMatrix(const Elite::FMatrix4& viewMatrix, const Elite::FMatrix4& projectionMatrix, const Elite::FMatrix4& worldMatrix)
{
	if (!m_pMatWorldViewProjVariable->IsValid())
		return;
	Elite::FMatrix4 WorldRotationYMatrix{
				cosf(m_Rotation.y), 0, sinf(m_Rotation.y), 0,
				0, 1, 0, 0,
				-sinf(m_Rotation.y), 0, cosf(m_Rotation.y), 0,
				0, 0, 0, 1
	};
	Elite::FMatrix4 WorldViewProjectionMatrix = projectionMatrix * viewMatrix * worldMatrix* WorldRotationYMatrix;
	const auto* data = &WorldViewProjectionMatrix.data[0][0];

	m_pMatWorldViewProjVariable->SetMatrix(data);
}

void BaseEffect::SetDiffuseMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pDiffuseMapVariable->IsValid())
		m_pDiffuseMapVariable->SetResource(pResourceView);
}

float BaseEffect::GetLightIntensity() const
{
	return -1.0f;
}

float BaseEffect::GetShininess() const
{
	return -1.0f;
}

void BaseEffect::Rotate(float amount, float elapsedSec)
{
	m_Rotation.y += elapsedSec * amount * float(E_TO_RADIANS);
}

ID3DX11Effect* BaseEffect::LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined( DEBUG ) || defined( _DEBUG )
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetFile.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);

	if (FAILED(result)) {

		if (pErrorBlob != nullptr) {

			char* pErrors = (char*)pErrorBlob->GetBufferPointer();

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;
		}
		else {

			std::wstringstream ss;
			ss << "Effectloader: Failed to CreateEffectFromFile!\nPath: " << assetFile;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}
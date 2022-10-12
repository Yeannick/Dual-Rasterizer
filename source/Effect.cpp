#include "pch.h"
#include "Effect.h"
#include <sstream>
Effect::Effect(ID3D11Device* pDevice, const std::wstring& assetPath, float lightIntesity , float Shininess ):
m_pDevice(pDevice),
m_AssetPath(assetPath),
m_LightIntensity(lightIntesity),
m_Shininess(Shininess)
{
	m_pEffect = Loadeffect(pDevice, assetPath);

	m_pTechnique = m_pEffect->GetTechniqueByName("DefaultTechnique");
	if (!m_pTechnique->IsValid())
	{
		std::wcout << L"Technique not valid\n";
	}
	m_pFloatPIVariable = m_pEffect->GetVariableByName("gPI")->AsScalar();
	if (!m_pFloatPIVariable->IsValid())
		std::wcout << L"FloatPIVariable not valid\n";

	m_pFloatLightIntensityVariable = m_pEffect->GetVariableByName("gLightIntensity")->AsScalar();
	if (!m_pFloatLightIntensityVariable->IsValid())
		std::wcout << L"FloatLightIntensityVariable not valid\n";

	m_pFloatShininessVariable = m_pEffect->GetVariableByName("gShininess")->AsScalar();
	if (!m_pFloatShininessVariable->IsValid())
		std::wcout << L"FloatShininessVariable not valid\n";

	m_pVecLightDirectionVariable = m_pEffect->GetVariableByName("gLightDirection")->AsVector();
	if (!m_pVecLightDirectionVariable->IsValid())
		std::wcout << L"VecLightDirectionVariable not valid\n";

	//// matrices
	m_pMatWorldViewProjVariable = m_pEffect->GetVariableByName("gWorldViewProj")->AsMatrix();
	if (!m_pMatWorldViewProjVariable->IsValid())
	{
		std::wcout << L"m_pMatWorldViewProjVariable not valid\n";
	}
	m_pMatWorldMatrixVariable = m_pEffect->GetVariableByName("gWorldMatrix")->AsMatrix();
	if (!m_pMatWorldMatrixVariable->IsValid())
		std::wcout << L"MatWorldMatrixVariable not valid\n";

	m_pMatViewInverseVariable = m_pEffect->GetVariableByName("gViewInverse")->AsMatrix();
	if (!m_pMatViewInverseVariable->IsValid())
		std::wcout << L"MatViewInverseVariable not valid\n";
	// textures
	m_pDiffuseMapVariable = m_pEffect->GetVariableByName("gDiffuseMap")->AsShaderResource();
	if (!m_pDiffuseMapVariable->IsValid())
		std::wcout << L"DiffuseMapVariable not valid\n";

	m_pNormalMapVariable = m_pEffect->GetVariableByName("gNormalMap")->AsShaderResource();
	if (!m_pNormalMapVariable->IsValid())
		std::wcout << L"NormalMapVariable not valid\n";

	m_pSpecularMapVariable = m_pEffect->GetVariableByName("gSpecularMap")->AsShaderResource();
	if (!m_pSpecularMapVariable->IsValid())
		std::wcout << L"SpecularMapVariable not valid\n";

	m_pGlossinessMapVariable = m_pEffect->GetVariableByName("gGlossinessMap")->AsShaderResource();
	if (!m_pGlossinessMapVariable->IsValid())
		std::wcout << L"GlossinessMapVariable not valid\n";

	
	SetConstants();
}

Effect::~Effect()
{
	if (m_pEffect)
	{
		m_pEffect->Release();
	}
	if (m_pDevice)
	{
		m_pDevice->Release();
	}
	if (m_pTechnique)
	{
		m_pTechnique->Release();
	}
}

ID3DX11Effect* Effect::Loadeffect(ID3D11Device* pDevice, const std::wstring& assetPath)
{
	HRESULT result = S_OK;
	ID3D10Blob* pErrorBlob = nullptr;
	ID3DX11Effect* pEffect;

	DWORD shaderFlags = 0;
#if defined(DEBUG) || defined(_DEBUG)
	shaderFlags |= D3DCOMPILE_DEBUG;
	shaderFlags |= D3DCOMPILE_SKIP_OPTIMIZATION;
#endif

	result = D3DX11CompileEffectFromFile(assetPath.c_str(),
		nullptr,
		nullptr,
		shaderFlags,
		0,
		pDevice,
		&pEffect,
		&pErrorBlob);
	if (FAILED(result))
	{
		if (pErrorBlob != nullptr)
		{
			char* pErrors = (char*)pErrorBlob->GetBufferPointer();

			std::wstringstream ss;
			for (unsigned int i = 0; i < pErrorBlob->GetBufferSize(); i++)
				ss << pErrors[i];

			OutputDebugStringW(ss.str().c_str());
			pErrorBlob->Release();
			pErrorBlob = nullptr;

			std::wcout << ss.str() << std::endl;


		}
		else
		{
			std::wstringstream ss;

			ss << "EFFECTLOADER : Failed to CreateEffectFromFile\n Path:" << assetPath;
			std::wcout << ss.str() << std::endl;
			return nullptr;
		}
	}

	return pEffect;
}

ID3DX11EffectMatrixVariable* Effect::GetWorldViewProjection() const
{
	return m_pMatWorldViewProjVariable;
}

void Effect::SetWorldViewProjectionMatrix(const Elite::FMatrix4& view, const Elite::FMatrix4& projection, const Elite::FMatrix4& world)
{
	if (m_pMatWorldViewProjVariable->IsValid())
	{
		Elite::FMatrix4 worldViewProjMatrix = projection * view * world;
		const float* pData = &worldViewProjMatrix.data[0][0];

		m_pMatWorldViewProjVariable->SetMatrix(pData);
	}
}

void Effect::SetWorldMatrix(const Elite::FMatrix4& worldMatrix)
{
	if (!m_pMatWorldMatrixVariable->IsValid())
		return;

	const auto* data = &worldMatrix.data[0][0];
	m_pMatWorldMatrixVariable->SetMatrix(data);
}

void Effect::SetViewInverseMatrix(const Elite::FMatrix4& viewInverseMatrix)
{
	if (!m_pMatViewInverseVariable->IsValid())
		return;

	const auto* data = &viewInverseMatrix.data[0][0];
	m_pMatViewInverseVariable->SetMatrix(data);
}

void Effect::SetDiffuseMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pDiffuseMapVariable->IsValid())
	{
		m_pDiffuseMapVariable->SetResource(pResourceView);
	}
}

void Effect::SetNormalMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pNormalMapVariable->IsValid())
		m_pNormalMapVariable->SetResource(pResourceView);
}

void Effect::SetSpecularMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pSpecularMapVariable->IsValid())
		m_pSpecularMapVariable->SetResource(pResourceView);
}

void Effect::SetGlossinessMap(ID3D11ShaderResourceView* pResourceView)
{
	if (m_pGlossinessMapVariable->IsValid())
		m_pGlossinessMapVariable->SetResource(pResourceView);
}

void Effect::SetConstants()
{
	if (m_pFloatPIVariable->IsValid())
		m_pFloatPIVariable->SetFloat(float(M_PI));

	
	if (m_pFloatLightIntensityVariable->IsValid())
		m_pFloatLightIntensityVariable->SetFloat(m_LightIntensity);;


	if (m_pFloatShininessVariable->IsValid())
		m_pFloatShininessVariable->SetFloat(m_Shininess);;

	if (m_pVecLightDirectionVariable->IsValid()) {

		Elite::FVector3 lightDirection = { 0.577f, -0.577f, 0.577f };

		const auto* data = &lightDirection.data[0];
		m_pVecLightDirectionVariable->SetFloatVector(data);
	}
}



#pragma once
class Effect
{
	
public:
	Effect(ID3D11Device* pDevice, const std::wstring& assetPath , float lightIntesity = 7.f , float Shininess = 25.f);
	~Effect();
	static ID3DX11Effect* Loadeffect(ID3D11Device* pDevice, const std::wstring& assetPath);

	ID3DX11EffectTechnique* GetTechnique() { return m_pTechnique; }
	ID3DX11Effect* GetEffect() { return m_pEffect; }
	ID3DX11EffectMatrixVariable* GetWorldViewProjection()const;

	void SetWorldViewProjectionMatrix(const Elite::FMatrix4& view, const Elite::FMatrix4& projection, const Elite::FMatrix4& world);
	void SetWorldMatrix(const Elite::FMatrix4& worldMatrix); 
	void SetViewInverseMatrix(const Elite::FMatrix4& viewInverseMatrix);
	void SetDiffuseMap(ID3D11ShaderResourceView* pResourceView);
	void SetNormalMap(ID3D11ShaderResourceView* pResourceView);
	void SetSpecularMap(ID3D11ShaderResourceView* pResourceView);
	void SetGlossinessMap(ID3D11ShaderResourceView* pResourceView);

	void SetConstants();
private:

	float m_LightIntensity = 7.f;
	float m_Shininess = 25.f;

	ID3D11Device* m_pDevice = nullptr;
	const std::wstring m_AssetPath;
	ID3DX11Effect* m_pEffect = nullptr;
	ID3DX11EffectTechnique* m_pTechnique{};
	// matrices
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pMatWorldMatrixVariable = nullptr;
	ID3DX11EffectMatrixVariable* m_pMatViewInverseVariable;
	// Textures
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pNormalMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pSpecularMapVariable;
	ID3DX11EffectShaderResourceVariable* m_pGlossinessMapVariable;

	ID3DX11EffectScalarVariable* m_pFloatPIVariable;
	ID3DX11EffectScalarVariable* m_pFloatLightIntensityVariable;
	ID3DX11EffectScalarVariable* m_pFloatShininessVariable;

	ID3DX11EffectVectorVariable* m_pVecLightDirectionVariable;

	
};


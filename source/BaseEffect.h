#pragma once
enum class EffectType { Material, Flat };
enum class SampleMode { Point, Linear, Anisotropic };
class BaseEffect
{
public :

	BaseEffect(ID3D11Device* pDevice, const std::wstring& assetPath);
	virtual ~BaseEffect();
	BaseEffect(const BaseEffect& other) = delete;
	BaseEffect& operator=(const BaseEffect& other) = delete;
	BaseEffect(BaseEffect&& other) = delete;
	BaseEffect& operator=(BaseEffect&& other) = delete;


	EffectType GetEffectType() const;
	ID3DX11Effect* GetEffect() const;
	ID3DX11EffectTechnique* GetTechnique() const;
	ID3DX11EffectMatrixVariable* GetWorldViewProjectionMatrix() const;
	void SetWorldViewProjectionMatrix(const Elite::FMatrix4& viewMatrix, const Elite::FMatrix4& projectionMatrix, const Elite::FMatrix4& worldMatrix);
	void SetDiffuseMap(ID3D11ShaderResourceView* pResourceView);
	virtual float GetLightIntensity() const;

	virtual float GetShininess() const;
	void Rotate(float amount, float elapsedSec);
protected:
	EffectType m_Type;
	Elite::FVector3 m_Rotation;
	ID3DX11Effect* m_pEffect;

	//Everything below gets auto release when his parent (m_pEffect) gets released
	ID3DX11EffectTechnique* m_pTechnique;

	//Matrices
	ID3DX11EffectMatrixVariable* m_pMatWorldViewProjVariable;
	ID3DX11EffectMatrixVariable* m_pMatWorldMatrixVariable;

	//Textures
	ID3DX11EffectShaderResourceVariable* m_pDiffuseMapVariable;

	static ID3DX11Effect* LoadEffect(ID3D11Device* pDevice, const std::wstring& assetFile);
};


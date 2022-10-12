#include "pch.h"
#include "FlatEffect.h"

FlatEffect::FlatEffect(ID3D11Device* pDevice, const std::wstring& assetFile)
	: BaseEffect(pDevice, assetFile)
{
	m_Type = EffectType::Flat;
}

FlatEffect::~FlatEffect()
{
}



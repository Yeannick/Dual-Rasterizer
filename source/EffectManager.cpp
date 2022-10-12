#include "pch.h"
#include "EffectManager.h"


EffectManager* EffectManager::m_Instance = nullptr;

EffectManager::EffectManager()
	: m_Effects{}
	
{
	
};

EffectManager::~EffectManager()
{
	for (auto& currentObject : m_Effects)
		delete currentObject.second;
}




void EffectManager::RemoveEffect(std::string key)
{
	m_Effects.erase(key);
}

BaseEffect* EffectManager::GetEffect(std::string key)
{
	return m_Effects.at(key);
}

void EffectManager::AddEffect(std::string key, BaseEffect* effect)
{
	m_Effects.emplace(key, effect);
}

std::map<std::string, BaseEffect*> EffectManager::GetEffects()
{
	return m_Effects;
}

#include "CCheat.h"

#include <cstdio>
#include "sdk/CS2Game.h"

CCheat::CCheat(std::string name, std::string description, Category category, bool enabled)
{
	this->m_Name = std::move(name);
	this->m_Description = std::move(description);
	this->m_Category = category;
	
	m_Enabled.store(enabled, std::memory_order_relaxed);

	printf("[+] Loaded Cheat -> %s\n", this->m_Name.c_str());
}

void CCheat::Render(ImDrawList* drawList)
{
}

void CCheat::Tick()
{
}

void CCheat::HandleKey()
{
}

uintptr_t CCheat::GetBoneMatrix(uintptr_t entityPawn)
{
	uintptr_t node = g_Instance->GetMemory()->Read<uintptr_t>(entityPawn + offset::m_pGameSceneNode); // m_pGameSceneNode
	uintptr_t boneMatrix = g_Instance->GetMemory()->Read<uintptr_t>(node + offset::m_modelState + 0x80); // m_modelState 
	return boneMatrix;
}

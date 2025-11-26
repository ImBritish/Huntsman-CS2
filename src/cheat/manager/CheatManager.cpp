#include "CheatManager.h"

#include "../impl/visual/CESP.h"
#include "../impl/aim/CAimBot.h"

#include <memory>

CCheatManager::~CCheatManager()
{
    Clear();
}

void CCheatManager::LoadCheats()
{
    // Visual
    this->RegisterCheat(std::make_unique<ESP>());

	// Aim
    this->RegisterCheat(std::make_unique<AimBot>());
}

void CCheatManager::CallRenderMethod(ImDrawList* drawList)
{
    std::shared_lock lock(m_Mutex);

    for (auto& c : m_Cheats) {
        if (c->IsEnabled())
            c->Render(drawList);
    }
}

void CCheatManager::CallTickMethod()
{
    std::shared_lock lock(m_Mutex);

    for (auto& c : m_Cheats) {
        if (c->IsEnabled())
            c->Tick();

        c->HandleKey();
    }
}

void CCheatManager::Clear()
{
    std::unique_lock lock(m_Mutex);
    m_Cheats.clear();
}

void CCheatManager::RegisterCheat(std::unique_ptr<CCheat> cheat)
{
    std::unique_lock lock(m_Mutex);
    this->m_Cheats.emplace_back(std::move(cheat));
}
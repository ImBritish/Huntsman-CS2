#pragma once

#include "../CCheat.h"

#include <vector>
#include <memory>
#include <shared_mutex>

class CCheatManager 
{
public:
	CCheatManager() = default;
	~CCheatManager();

	void LoadCheats();

	void CallRenderMethod(ImDrawList* drawList);
	void CallTickMethod();

	void Clear();
private:
	void RegisterCheat(std::unique_ptr<CCheat> cheat);

	std::vector<std::unique_ptr<CCheat>> m_Cheats{};
	mutable std::shared_mutex m_Mutex;
};
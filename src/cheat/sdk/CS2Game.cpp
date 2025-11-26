#include "CS2Game.h"

#include <imgui/imgui.h>

#include <thread>
#include <string>
#include <sstream>

#include "../sdk/offsets/Offsets.h"

CS2Game::CS2Game(Memory* memory, uintptr_t* client)
{
	this->m_Memory = memory;
	this->m_Client = client;
	this->m_EntityManager = std::make_unique<EntityManager>(this->m_Memory);
	this->m_CheatManager = std::make_unique<CCheatManager>();

	this->m_CheatManager->LoadCheats();
}

EntityTeam CS2Game::GetLocalPlayerTeam()
{
	uintptr_t localPlayer = this->m_Memory->Read<uintptr_t>(this->GetClient() + offset::dwLocalPlayerPawn);

	if (!localPlayer)
		return EntityTeam::UNKNOWN;

	int localTeam = this->m_Memory->Read<int>(localPlayer + offset::m_iTeamNum);

	if (!localTeam)
		return EntityTeam::UNKNOWN;

	switch (localTeam)
	{
	case 2: 
		return EntityTeam::T;
	case 3:
		return EntityTeam::CT;
	default:
		return EntityTeam::UNKNOWN;
	}

	return EntityTeam::UNKNOWN;
}

uintptr_t CS2Game::GetLocalPlayer()
{
	return this->m_Memory->Read<uintptr_t>(this->GetClient() + offset::dwLocalPlayerPawn);
}

ViewMatrixT CS2Game::GetViewMatrix()
{
	return this->m_Memory->Read<ViewMatrixT>(this->GetClient() + offset::dwViewMatrix);
}

void CS2Game::Render()
{
	ImDrawList* drawList = ImGui::GetBackgroundDrawList();

	this->GetCheatManager()->CallRenderMethod(drawList);
}

void CS2Game::Tick()
{
	this->GetCheatManager()->CallTickMethod();
}

void CS2Game::StartEntityCollectionThread()
{
	EntityManager* entityManager = this->GetEntityManager();

	uintptr_t client = this->GetClient();

	std::thread entityCollector([this, entityManager, client]() {
		while (true)
		{
			timeBeginPeriod(1);

			static int iterated = 0;

			if (++iterated < 10)
				continue;

			entityManager->CollectEntities(client);

			iterated = 0;

			std::this_thread::sleep_for(std::chrono::microseconds(200));

			timeEndPeriod(1);
		}
	});

	entityCollector.detach();
}
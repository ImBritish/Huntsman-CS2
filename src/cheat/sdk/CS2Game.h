#pragma once

#include <iostream>

#include "manager/EntityManager.h"
#include "../manager/CheatManager.h"

class CS2Game {
private:
	Memory* m_Memory = nullptr;
	uintptr_t* m_Client = nullptr;

	std::unique_ptr<EntityManager> m_EntityManager = nullptr;
	std::unique_ptr<CCheatManager> m_CheatManager = nullptr;
public:
	CS2Game(Memory* memory, uintptr_t* client);

	EntityManager* GetEntityManager() { return this->m_EntityManager.get(); }
	CCheatManager* GetCheatManager() { return this->m_CheatManager.get(); }

	EntityTeam GetLocalPlayerTeam();
	uintptr_t GetLocalPlayer();

	uintptr_t GetClient() { return *this->m_Client; }

	Memory* GetMemory() { return this->m_Memory; }

	ViewMatrixT GetViewMatrix();

	void Render();
	void Tick();

	void StartEntityCollectionThread();
};

inline std::unique_ptr<CS2Game> g_Instance = nullptr;
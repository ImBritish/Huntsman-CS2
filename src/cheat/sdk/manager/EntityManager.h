#pragma once

#include <array>
#include <atomic>

#include "../entity/Entity.h"
#include "../../memory/memory.h"

class EntityManager
{
public:
    EntityManager(Memory* memory)
        : m_Memory(memory) {}

    void CollectEntities(uintptr_t client);

    Entity GetEntityByIndex(int index, uintptr_t entityList, uintptr_t localPlayer, int localTeam);

    const std::vector<Entity>& GetEntities();
private:
    std::vector<Entity> m_EntityList;

    Memory* m_Memory;
};
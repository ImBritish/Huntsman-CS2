#include "EntityManager.h"

#include <iostream>

#include "../offsets/Offsets.h"

void EntityManager::CollectEntities(uintptr_t client)
{
    std::vector<Entity> entityBuffer{};

    uintptr_t localPlayer = this->m_Memory->Read<uintptr_t>(client + offset::dwLocalPlayerPawn);

    if (!localPlayer)
        return;

    uintptr_t entityList = this->m_Memory->Read<uintptr_t>(client + offset::dwEntityList);

    if (!entityList)
        return;

    int localTeam = this->m_Memory->Read<int>(localPlayer + offset::m_iTeamNum);

    if (!localTeam)
        return;

    for (int i = 1; i < 64; i++) {
        Entity entity = GetEntityByIndex(i, entityList, localPlayer, localTeam);

        if (!entity.isValid())
            continue;

        if (entity.IsDead())
            continue;

        if (entity.GetEntityPtr() == localPlayer)
            continue;

        entityBuffer.push_back(entity);
    }

    this->m_EntityList = entityBuffer;
}

Entity EntityManager::GetEntityByIndex(const int index, const uintptr_t entityList, const uintptr_t localPlayer, const int localTeam)
{
    uintptr_t listEntry = this->m_Memory->Read<uintptr_t>(entityList + (8 * (index & 0x7FFF) >> 9) + 16);

    if (!listEntry)
        return{};

    uintptr_t entityController = this->m_Memory->Read<uintptr_t>(listEntry + 112 * (index & 0x1FF));

    if (!entityController)
        return {};

    uint32_t playerPawnHandle = this->m_Memory->Read<uint32_t>(entityController + offset::m_hPlayerPawn);

    if (!playerPawnHandle)
        return {};

    uintptr_t listEntry2 = this->m_Memory->Read<uintptr_t>(entityList + 0x8 * ((playerPawnHandle & 0x7FFF) >> 9) + 16);

    if (!listEntry2)
        return {};

    uintptr_t pCSPlayerPawn = this->m_Memory->Read<uintptr_t>(listEntry2 + 112 * (playerPawnHandle & 0x1FF));

    if (!pCSPlayerPawn)
        return {};

    int health = this->m_Memory->Read<int>(pCSPlayerPawn + offset::m_iHealth);
    int teamNum = this->m_Memory->Read<int>(pCSPlayerPawn + offset::m_iTeamNum);

    EntityTeam team = teamNum == 2 ? EntityTeam::T : teamNum == 3 ? EntityTeam::CT : EntityTeam::UNKNOWN;

    Entity entity{ pCSPlayerPawn, health, 0, team };

    Vec3 entityPos = this->m_Memory->Read<Vec3>(entity.GetEntityPtr() + offset::m_vOldOrigin);

    entity.SetPosition(entityPos);

    return entity;
}

const std::vector<Entity>& EntityManager::GetEntities()
{
    return this->m_EntityList;
}
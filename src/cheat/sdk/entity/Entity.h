#pragma once

#include "../math/Math.h"

enum EntityTeam {
	UNKNOWN, CT, T
};

class Entity
{
public:
	Entity() = default;
	Entity(
		uintptr_t entityPtr, int health, int armor, EntityTeam team
	) : m_EntityPtr(entityPtr), m_Health(health), m_Armor(armor), m_Team(team) {}

	Entity(const Entity&) = default;
	Entity& operator=(const Entity&) = default;

	~Entity() = default;

	int GetHealth() { return this->m_Health; }
	int GetArmor() { return this->m_Armor; }

	uintptr_t GetEntityPtr() { return this->m_EntityPtr; }

	EntityTeam GetTeam() { return this->m_Team; }

	Vec3 GetPosition() { return this->m_Position; }
	void SetPosition(Vec3 pos) { this->m_Position = pos; }

	float GetHeadHeight(float screenY, float headY) {
		return (screenY - headY) / 8;
	}

	bool IsDead() { return this->m_Health <= 0; }
	bool isValid() { return this->m_EntityPtr != 0; }
private:
	uintptr_t m_EntityPtr;
	int m_Health, m_Armor;
	EntityTeam m_Team;
	Vec3 m_Position;
};
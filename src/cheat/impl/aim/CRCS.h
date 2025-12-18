#pragma once

#include "../../CCheat.h"
#include "../../sdk/entity/Entity.h"
#include "../../sdk/math/Math.h"

class RCS : public CCheat
{
public:
	RCS() : CCheat("RCS", "Automatically Controll the recoill", Category::Aim, true) {}

	void Tick() override;
private:
	float m_oldPunchX = 0.f, m_oldPunchY = 0.f;
};
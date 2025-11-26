#pragma once

#include "../../CCheat.h"
#include "../../sdk/entity/Entity.h"
#include "../../sdk/math/Math.h"

class AimBot : public CCheat
{
public:
	AimBot() : CCheat("AimBot", "Automatically aim at a player", Category::Aim, true) {}

	void Render(ImDrawList* drawList) override;
	void Tick() override;
	void HandleKey() override;
private:
	Vec3 GetBonePosition(BoneIndex index, uintptr_t boneMatrix);
	BoneIndex GetBoneIndexFromTargetBone(int32_t index);

	Vec2 m_ClosestEntity = { 0, 0 };
};
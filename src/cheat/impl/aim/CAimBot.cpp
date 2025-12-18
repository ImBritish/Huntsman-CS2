#include "CAimBot.h"

using namespace config::aim::aimbot;

#include <windows.h>
#include <cstdio>

#include "../../sdk/CS2Game.h"

void AimBot::Render(ImDrawList* drawList)
{
	float* color = render::f_CircleColor;

	if (!b_FoV || !render::b_DrawCircle)
		return;

	drawList->AddCircle(
		{ SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f }, f_Radius, ImColor(color[0], color[1], color[2], color[3]), 0, render::f_CircleThickness
	);
}

void AimBot::Tick()
{
	Vec2 screenCenter = { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f };

	bool useRadius = config::aim::aimbot::b_FoV;
	float radius = config::aim::aimbot::f_Radius;

	float lowestDistance = 10000.0f;

	for (Entity e : g_Instance->GetEntityManager()->GetEntities()) {
		if (e.GetTeam() == g_Instance->GetLocalPlayerTeam())
			continue;

		if (e.IsDead())
			continue;

		Vec3 bonePos = this->GetBonePosition(this->GetBoneIndexFromTargetBone(config::aim::aimbot::i_TargetBoneIndex), this->GetBoneMatrix(e.GetEntityPtr()));

		Vec2 screenPos;

		if (WorldToScreen(bonePos, screenPos, g_Instance->GetViewMatrix())) {

			float distance = sqrtf(
				powf(screenPos.x - screenCenter.x, 2) +
				powf(screenPos.y - screenCenter.y, 2)
			);

			if (useRadius && distance > radius)
				continue;

			if (distance < lowestDistance)
			{
				lowestDistance = distance;
				this->m_ClosestEntity = screenPos;
			}
		}
	}

	if (lowestDistance == 10000.0f)
		this->m_ClosestEntity = { 0, 0 };
}

void AimBot::HandleKey()
{
	if (this->IsEnabled() && g_Instance->GetMemory()->InForeground("Counter-Strike 2") && GetAsyncKeyState(VK_LSHIFT)) {
		if (this->m_ClosestEntity.x == 0 && this->m_ClosestEntity.y == 0)
			return;

		Vec2 screenCenter = { SCREEN_WIDTH / 2.f, SCREEN_HEIGHT / 2.f };

		float deltaX = this->m_ClosestEntity.x - screenCenter.x;
		float deltaY = this->m_ClosestEntity.y - screenCenter.y;

		float smooth = config::aim::aimbot::f_Smoothing;

		if (smooth < 1.f)
			smooth = 1.f;

		float factor = 1.f - (1.f / smooth);

		deltaX *= (1.f - factor);
		deltaY *= (1.f - factor);

		mouse_event(MOUSEEVENTF_MOVE,
			static_cast<int>(deltaX),
			static_cast<int>(deltaY),
			0, 0
		);
	}
}

Vec3 AimBot::GetBonePosition(BoneIndex index, uintptr_t boneMatrix)
{
	return g_Instance->GetMemory()->Read<Vec3>(boneMatrix + (index * 0x20));
}

BoneIndex AimBot::GetBoneIndexFromTargetBone(int32_t index)
{
	BoneIndex boneIndex = BoneIndex::NECK;

	switch (index) {
		case 0: { // Head
			return BoneIndex::HEAD;
		}
		case 1: { // Neck
			return BoneIndex::NECK;
		}
		case 2: { // Chest
			return BoneIndex::CHEST;
		}
		case 3: { // Stomach
			return BoneIndex::STOMACH;
		}
		case 4: { // Stomach
			return BoneIndex::COCK;
		}
	}

	return boneIndex;
}

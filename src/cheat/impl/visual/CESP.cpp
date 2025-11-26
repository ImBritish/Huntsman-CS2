#include "CESP.h"

#include <windows.h>
#include <cstdio>

#include "../../sdk/CS2Game.h"

struct BoneConnection_Struct
{
	int bone1;
	int bone2;

	BoneConnection_Struct(int b1, int b2) : bone1(b1), bone2(b2) {}
};

BoneConnection_Struct boneConnections[] = {
	BoneConnection_Struct(6, 5),  // head to neck
	BoneConnection_Struct(5, 4),  // neck to spine
	BoneConnection_Struct(4, 0),  // spine to hip
	BoneConnection_Struct(4, 8), // spine to left shoulder
	BoneConnection_Struct(8, 9), // left shoulder to left arm
	BoneConnection_Struct(9, 11), // arm to hand
	BoneConnection_Struct(4, 13),
	BoneConnection_Struct(13, 14),
	BoneConnection_Struct(14, 16),
	BoneConnection_Struct(4, 2),  // spine to spine_1
	BoneConnection_Struct(0, 22), // hip to left_hip
	BoneConnection_Struct(0, 25), // hip to right_hip
	BoneConnection_Struct(22, 23), // left_hip to left_knee
	BoneConnection_Struct(23, 24), // left knee to left foot
	BoneConnection_Struct(25, 26), // right_hip to right_knee
	BoneConnection_Struct(26, 27) // right knee to right foot
};

void ESP::Render(ImDrawList* drawList)
{
	ViewMatrixT matrix = g_Instance->GetViewMatrix();

	for (Entity entity : g_Instance->GetEntityManager()->GetEntities()) 
	{
		bool isEntityFriendly = entity.GetTeam() == g_Instance->GetLocalPlayerTeam();

		if (!isEntityFriendly && !config::visual::esp::enemy::b_Enabled)
			continue;

		if (isEntityFriendly && !config::visual::esp::team::b_Enabled)
			continue;

		Vec3 entityPosition = entity.GetPosition();

		Vec3 HeadPosition = entityPosition + Vec3{ 0.0f, 0.0f, 72.0f };

		Vec2 screenPos, headScreenPos;
		Vec2 screenTop(SCREEN_WIDTH / 2.0f, 0.0f);
		Vec2 screenCenter(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT / 2.0f);
		Vec2 screenBottom(SCREEN_WIDTH / 2.0f, SCREEN_HEIGHT);

		if (config::visual::esp::skeleton::b_Enabled)
		{
			this->DrawSkeletons(entity, matrix, drawList);
		}

		if (!WorldToScreen(entityPosition, screenPos, matrix) || !WorldToScreen(HeadPosition, headScreenPos, matrix))
			continue;

		float boxHeight = std::fabs(headScreenPos.y - screenPos.y);
		float boxWidth = boxHeight * 0.6f;

		Vec2 topLeft(screenPos.x - boxWidth / 2, headScreenPos.y);
		Vec2 topRight(screenPos.x + boxWidth / 2 + 12.0f, headScreenPos.y);
		Vec2 bottomRight(screenPos.x + boxWidth / 2, screenPos.y);

		//if (config::visual::esp::team::b_FillBox) {
		//	drawList->AddRectFilled(
		//		{ topLeft.x, topLeft.y }, { bottomRight.x, bottomRight.y }, ImColor(0.12f, 0.12f, 0.12f, 0.35f)
		//	);
		//}

		float* boxColor = isEntityFriendly ? config::visual::esp::team::f_BoxColor : config::visual::esp::enemy::f_BoxColor;

		drawList->AddRect(
			{ topLeft.x, topLeft.y }, { bottomRight.x, bottomRight.y }, ImColor(config::visual::esp::outline::f_Color[0], config::visual::esp::outline::f_Color[1], config::visual::esp::outline::f_Color[2], config::visual::esp::outline::f_Color[3]), 0, 0, (config::visual::esp::f_BoxThickness + config::visual::esp::outline::f_Thickness) - 1.f
		);

		drawList->AddRect(
			{ topLeft.x, topLeft.y }, { bottomRight.x, bottomRight.y }, ImColor(boxColor[0], boxColor[1], boxColor[2], boxColor[3]), 0, 0, config::visual::esp::f_BoxThickness
		);
	}
}

void ESP::Tick()
{

}

void ESP::DrawSkeletons(Entity entity, ViewMatrixT viewMatrix, ImDrawList* drawList)
{
	uintptr_t boneMatrix = this->GetBoneMatrix(entity.GetEntityPtr());

	auto mem = g_Instance->GetMemory();

	Vec3 head = mem->Read<Vec3>(boneMatrix + BoneIndex::HEAD * 32);

	Vec2 headScreenPos;
	Vec2 screenPos;

	for (int i = 0; i < sizeof(boneConnections) / sizeof(boneConnections[0]); i++)
	{
		int bone1 = boneConnections[i].bone1;
		int bone2 = boneConnections[i].bone2;

		Vec3 VectorBone1 = mem->Read<Vec3>(boneMatrix + bone1 * 32);
		Vec3 VectorBone2 = mem->Read<Vec3>(boneMatrix + bone2 * 32);

		if (WorldToScreen(entity.GetPosition(), screenPos, viewMatrix) && WorldToScreen(head, headScreenPos, viewMatrix)) {
			Vec2 b1, b2;

			if (WorldToScreen(VectorBone1, b1, viewMatrix) && WorldToScreen(VectorBone2, b2, viewMatrix)) {
				float headHeight = entity.GetHeadHeight(screenPos.y, headScreenPos.y);

				// DrawUtils::DrawLine(b1, b2, Config::Esp::BonesColor, Config::Esp::BonesThickness);

				if (config::visual::esp::skeleton::outline::b_Enabled) {
					drawList->AddLine(
						{ b1.x, b1.y }, { b2.x, b2.y }, ImColor(config::visual::esp::skeleton::outline::f_Color[0], config::visual::esp::skeleton::outline::f_Color[1], config::visual::esp::skeleton::outline::f_Color[2], config::visual::esp::skeleton::outline::f_Color[3]), (config::visual::esp::skeleton::f_Thickness + config::visual::esp::skeleton::outline::f_Thickness) - .5f
					);
				}

				drawList->AddLine(
					{ b1.x, b1.y }, { b2.x, b2.y }, ImColor(config::visual::esp::skeleton::f_Color[0], config::visual::esp::skeleton::f_Color[1], config::visual::esp::skeleton::f_Color[2], config::visual::esp::skeleton::f_Color[3]), config::visual::esp::skeleton::f_Thickness
				);

				if (config::visual::esp::skeleton::b_HeadCircle)
				{
					drawList->AddCircle(
						{ headScreenPos.x, headScreenPos.y }, headHeight, ImColor(config::visual::esp::skeleton::f_Color[0], config::visual::esp::skeleton::f_Color[1], config::visual::esp::skeleton::f_Color[2], config::visual::esp::skeleton::f_Color[3]), 0, config::visual::esp::skeleton::f_Thickness
					);
				}
			}
		}
	}
}
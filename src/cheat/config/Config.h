#pragma once

#include <imgui/imgui.h>

namespace config
{
	namespace aim
	{
		namespace aimbot
		{
			inline bool b_Enabled = true;
			inline float f_Smoothing = 5;

			inline bool b_FoV = true;
			inline float f_Radius = 60;

			inline bool b_LockTarget = true;
			inline bool b_DormantCheck = true;

			inline int i_TargetModeIndex = 0;
			inline const char* a_TargetModeItems[4] = { "Distance", "Health", "Armor", "Center" };

			inline int32_t i_TargetBoneIndex = 0;
			inline const char* a_TargetBoneModeItems[5] = { "Head", "Neck", "Chest", "Stomach", "Pelvis" };
			
			namespace render
			{
				inline bool b_DrawCircle = true;
				inline float f_CircleColor[4] = { 1.f, 1.f, 1.f, 1.f };
				inline float f_CircleThickness = 1.f;
			}
		}
	}

	namespace visual
	{
		namespace esp
		{
			inline int i_BoxModeIndex = 0;
			inline const char* a_BoxModeItems[2] = { "Box", "Corner" };

			inline float f_BoxThickness = 1.f;

			namespace skeleton
			{
				inline bool b_Enabled = true;
				inline bool b_HeadCircle = false;
				inline float f_Thickness = 1.f;

				inline float f_Color[4] = { 1.f, 1.f, 1.f, 1.f };

				namespace outline
				{
					inline bool b_Enabled = false;
					inline float f_Thickness = 1.5f;
					inline float f_Color[4] = { 0.f, 0.f, 0.f, 1.f };
				}
			}

			namespace enemy
			{
				inline bool b_Enabled = true; // Enable ESP for Enemys
				inline float f_BoxColor[4] = { 1.f, 1.f, 1.f, 1.f };
			}

			namespace team
			{
				inline bool b_Enabled = false; // Enable ESP for TeamMates
				inline float f_BoxColor[4] = { 1.f, 1.f, 1.f, 1.f };
			}

			namespace outline // Same for Team and Enemy
			{
				inline bool b_Enabled = false;
				inline float f_Thickness = 1.5f;
				inline float f_Color[4] = { 0.f, 0.f, 0.f, 1.f };
			}
		}
	}
}
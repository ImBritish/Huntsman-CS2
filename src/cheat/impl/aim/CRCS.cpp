#include "CRCS.h"

#include "../../sdk/CS2Game.h"

void RCS::Tick()
{
	float aimPunchX = g_Instance->GetMemory()->Read<float>(g_Instance->GetLocalPlayer() + offset::m_aimPunchAngle);
	float aimPunchY = g_Instance->GetMemory()->Read<float>(g_Instance->GetLocalPlayer() + aimPunchX + 0x4);

	uint32_t isFiring = g_Instance->GetMemory()->Read<uint32_t>(g_Instance->GetLocalPlayer() + offset::m_iShotsFired);

	if (!(isFiring > 1)) {
		this->m_oldPunchX = aimPunchX;
		this->m_oldPunchY = aimPunchY;
		return;
	}

	float deltaX = (aimPunchX - this->m_oldPunchX) * -1.0f;
	float deltaY = (aimPunchY - this->m_oldPunchY) * -1.0f;

	uintptr_t dwSense = g_Instance->GetMemory()->Read<uintptr_t>(g_Instance->GetClient() + offset::dwSensitivity);
	float playerSens = g_Instance->GetMemory()->Read<float>(dwSense + offset::dwSensitivity_sensitivity);

	int mouseX = (deltaY * 2.0 / playerSens) / -0.022;
	int mouseY = (deltaX * 2.0 / playerSens) / 0.022;

	// Mouse Move
	INPUT input = { 0 };

	input.type = INPUT_MOUSE;
	input.mi.dwFlags = MOUSEEVENTF_MOVE;
	input.mi.dx = mouseX;
	input.mi.dy = mouseY;
	input.mi.dwExtraInfo = 0;

	SendInput(1, &input, sizeof(INPUT));

	this->m_oldPunchX = aimPunchX;
	this->m_oldPunchY = aimPunchY;
}
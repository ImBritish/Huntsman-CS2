#pragma once

#include <imgui/imgui.h>

#include <atomic>
#include <string>

#include "sdk/offsets/Offsets.h"
#include "config/Config.h"

#define SCREEN_WIDTH GetSystemMetrics(SM_CXSCREEN)
#define SCREEN_HEIGHT GetSystemMetrics(SM_CYSCREEN)

enum Category : short {
	Aim, Visual
};

enum BoneIndex : int32_t
{
	HEAD = 6,
	NECK = 5,
	CHEST = 4,
	STOMACH = 2,
	COCK = 0,
	LSHOULDER = 8,
	LARM = 9,
	LHAND = 10,
	RSHOULDER = 13,
	RARM = 14,
	RHAND = 15,
	LTHIGH = 22,
	LCALF = 23,
	LFOOT = 24,
	RTHIGH = 25,
	RCALF = 26,
	RFOOT = 27,
};

class CCheat
{
public:
	CCheat(
		std::string name,
		std::string description,
		Category category,
		bool enabled = false
	);

	virtual ~CCheat() = default;

	virtual void Render(ImDrawList* drawList);
	virtual void Tick();
	virtual void HandleKey();

	uintptr_t GetBoneMatrix(uintptr_t entityPawn);

	void SetEnabled(bool v) { m_Enabled.store(v, std::memory_order_release); }
	bool IsEnabled() const { return m_Enabled.load(std::memory_order_acquire); }

	void Toggle() {
		bool expected = m_Enabled.load(std::memory_order_relaxed);
		while (!m_Enabled.compare_exchange_weak(expected, !expected,
			std::memory_order_acq_rel, std::memory_order_relaxed)) {
		}
	}

	const std::string& GetName() const { return m_Name; }
	const std::string& GetDescription() const { return m_Description; }
	Category GetCategory() const { return m_Category; }
private:
	std::string m_Name;
	std::string m_Description;
	Category m_Category;
	std::atomic<bool> m_Enabled = false;
};


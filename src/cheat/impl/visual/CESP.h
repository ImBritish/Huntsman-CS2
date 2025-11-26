#pragma once

#include "../../CCheat.h"
#include "../../sdk/entity/Entity.h"
#include "../../sdk/math/Math.h"

class ESP : public CCheat
{
public:
	ESP() : CCheat("ESP", "Render a box around other players", Category::Visual, true) {}

	void Render(ImDrawList* drawList) override;
	void Tick() override;

	void DrawSkeletons(Entity entity, ViewMatrixT viewMatrix, ImDrawList* drawList);
private:
};
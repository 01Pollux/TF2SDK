#pragma once

#include "Engine/ClientDll.hpp"
#include "Utils/Draw.hpp"

TF2_NAMESPACE_BEGIN(::Utils);

bool GetVectorInHudSpace(const Vector3D_F& origin, const Vector2D_F& screen_size, Vector2D_F& cord)
{
	const Matrix4x4_F& worldToScreen = Interfaces::EngineClient->WorldToScreenMatrix();
	const float w = worldToScreen[3][0] * origin[0] + worldToScreen[3][1] * origin[1] + worldToScreen[3][2] * origin[2] + worldToScreen[3][3];

	if (w > 0.001f)
	{
		float screen_x = (worldToScreen[0][0] * origin[0] + worldToScreen[0][1] * origin[1] + worldToScreen[0][2] * origin[2] + worldToScreen[0][3]) / w;
		float screen_y = (worldToScreen[1][0] * origin[0] + worldToScreen[1][1] * origin[1] + worldToScreen[1][2] * origin[2] + worldToScreen[1][3]) / w;

		cord = {
			0.5f * (1.0f + screen_x) * screen_size[0],
			0.5f * (1.0f - screen_y) * screen_size[1]
		};

		return true;
	}
	else return false;
}

bool IsVectorInHudSpace(const Vector3D_F& pos)
{
	const Matrix4x4_F& worldToScreen = Interfaces::EngineClient->WorldToScreenMatrix();
	const float w = worldToScreen[3][0] * pos[0] + worldToScreen[3][1] * pos[1] + worldToScreen[3][2] * pos[2] + worldToScreen[3][3];
	return w > 0.001f;
}

TF2_NAMESPACE_END();

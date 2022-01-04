#pragma once

#include <tf2/math/Vector.hpp>

TF2_NAMESPACE_BEGIN(::utils);

/// <summary>
/// Get hud's position from vector position
/// </summary>
/// <param name="cord">output coordinate</param>
/// <param name="offset">extra offset to add to position</param>
PX_SDK_TF2 bool GetVectorInHudSpace(const Vector3D_F& origin, const Vector2D_F& screen_position, Vector2D_F& cord);

/// <summary>
/// Tests if the current origin is in hud's space
/// </summary>
PX_SDK_TF2 bool IsVectorInHudSpace(const Vector3D_F& origin);

TF2_NAMESPACE_END();

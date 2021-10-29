#pragma once

#include "TF2Config.hpp"
#include "Math/Vector.hpp"

TF2_NAMESPACE_BEGIN();
class IBaseEntityInternal;
class ITFPlayerInternal;
TF2_NAMESPACE_END();

TF2_NAMESPACE_BEGIN(::Utils);

/// <summary>
/// Process simple latency prediction
/// </summary>
SG_SDK_TF2 Vector3D_F Prediction_SimpleLatency(const Vector3D_F& position, const Vector3D_F& velocity);

struct PredictionProjectileRet
{
	Vector3D_F Results;
	Vector3D_F InitialResults;
};
/// <summary>
/// Process a prediction, engine side
/// </summary>
SG_SDK_TF2 PredictionProjectileRet Prediction_Projectile(IBaseEntityInternal* player, float time);


TF2_NAMESPACE_END();

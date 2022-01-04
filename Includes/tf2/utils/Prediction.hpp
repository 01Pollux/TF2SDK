#pragma once

#include <tf2/math/Vector.hpp>

TF2_NAMESPACE_BEGIN();

class IBaseEntityInternal;
class ITFPlayerInternal;

TF2_NAMESPACE_END();

TF2_NAMESPACE_BEGIN(::utils);

/// <summary>
/// Process simple latency prediction
/// </summary>
PX_SDK_TF2 Vector3D_F Prediction_SimpleLatency(
	const Vector3D_F& position, 
	const Vector3D_F& velocity
);

/// <summary>
/// Predict's projectile next position (velocity position)
/// </summary>
PX_SDK_TF2 Vector3D_F Prediction_Projectile(
	IBaseEntityInternal* pEnt,
	const Vector3D_F& position,
	float speed,
	float projectile_gravity,
	float gravity
);

///// <summary>
///// Predict's entity next position, engine side
///// </summary>
//PX_SDK_TF2 Vector3D_F Prediction_EngineSide(
//	IBaseEntityInternal* pEnt,
//	float frame_time,
//	const Vector3D_F* predict_velocity
//);


TF2_NAMESPACE_END();

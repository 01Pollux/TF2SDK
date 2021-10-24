#pragma once

#include "TF2Config.hpp"
#include "Math/Matrix.hpp"
#include <numbers>

TF2_NAMESPACE_BEGIN(::Utils);

template<typename _FTy, typename = std::enable_if_t<std::is_floating_point_v<_FTy>>>
constexpr _FTy DegToRad(_FTy n)
{
	return n * static_cast<_FTy>(std::numbers::pi_v<_FTy> / 180.0);
}

template<typename _FTy, typename = std::enable_if_t<std::is_floating_point_v<_FTy>>>
constexpr _FTy RadToDeg(const _FTy& n)
{
	return n * static_cast<_FTy>(180.0 / std::numbers::pi_v<_FTy>);
}

constexpr Vector3D_F CrossProduct(const Vector3D_F& v1, const Vector3D_F& v2)
{
	return {
		v1[1] * v2[2] - v1[2] * v2[1],
		v1[2] * v2[0] - v1[0] * v2[2],
		v1[0] * v2[1] - v1[1] * v2[0]
	};
}

SG_SDK_TF2 void 
VectorVectors(const Vector3D_F& fwd, Vector3D_F& origin, Vector3D_F& up);

SG_SDK_TF2 void
VectorAngles(const Vector3D_F& forward, Angle_F& angles);

SG_SDK_TF2 void
AngleVectors(const Angle_F& angles, Vector3D_F* forward = nullptr, Vector3D_F* right = nullptr, Vector3D_F* up = nullptr);

SG_SDK_TF2 void
AngleMatrix(const Angle_F& angles, Matrix3x4_F& matrix);

SG_SDK_TF2 float
VecToYaw(const Vector3D_F& vec);

SG_SDK_TF2 float
VecToPitch(const Vector3D_F& vec);

SG_SDK_TF2 float
VecToYaw(const Matrix3x4_F& matrix, const Vector3D_F& vec);

SG_SDK_TF2 float
VecToPitch(const Matrix3x4_F& matrix, const Vector3D_F& vec);

SG_SDK_TF2 Vector3D_F
YawToVector(float yaw);

SG_SDK_TF2 float
ApproachAngle(float target, float value, float speed);

SG_SDK_TF2 float
AngleDiff(float destAngle, float srcAngle);

SG_SDK_TF2 float
AngleDistance(float next, float cur);

SG_SDK_TF2 float
GetFOV(const Vector3D_F& start, const Vector3D_F& end, const Angle_F& angle);

inline float AngMod(float a)
{
	return (360.f / 65536) * ((int)(a * (65536.f / 360.0f)) & 65535);
}

inline float AngleNormalize(float angle)
{
	angle = fmodf(angle, 360.0f);
	if (angle > 180)
		angle -= 360;
	else if (angle < -180)
		angle += 360;
	return angle;
}

inline void ClampAngle(Angle_F& ang)
{
	ang[0] = AngleNormalize(ang[0]);
	ang[1] = AngleNormalize(ang[1]);
	ang[2] = 0.f;
}

inline void SmoothAngle(const Angle_F& input, Angle_F& output, const float rate)
{
	const Angle_F delta = (input - output) * rate;

	output[0] = ApproachAngle(output[0], input[0], delta[0]);
	output[1] = ApproachAngle(output[1], input[1], delta[1]);

	ClampAngle(output);
}

TF2_NAMESPACE_END();

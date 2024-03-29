#pragma once

#include <numbers>
#include <tf2/config.hpp>
#include "tf2/math/Matrix.hpp"

TF2_NAMESPACE_BEGIN(::utils);

template<typename _FTy, typename = std::enable_if_t<std::is_floating_point_v<_FTy>>>
[[nodiscard]] constexpr _FTy DegToRad(_FTy n) noexcept
{
	return n * static_cast<_FTy>(std::numbers::pi_v<_FTy> / 180.0);
}

template<typename _FTy, typename = std::enable_if_t<std::is_floating_point_v<_FTy>>>
[[nodiscard]] constexpr _FTy RadToDeg(const _FTy& n) noexcept
{
	return n * static_cast<_FTy>(180.0 / std::numbers::pi_v<_FTy>);
}

[[nodiscard]] constexpr Vector3D_F CrossProduct(const Vector3D_F& v1, const Vector3D_F& v2) noexcept
{
	return {
		v1[1] * v2[2] - v1[2] * v2[1],
		v1[2] * v2[0] - v1[0] * v2[2],
		v1[0] * v2[1] - v1[1] * v2[0]
	};
}

PX_SDK_TF2 void 
VectorVectors(const Vector3D_F& fwd, Vector3D_F& origin, Vector3D_F& up);

PX_SDK_TF2 void
VectorAngles(const Vector3D_F& forward, Angle_F& angles);

PX_SDK_TF2 void
AngleVectors(const Angle_F& angles, Vector3D_F* forward = nullptr, Vector3D_F* right = nullptr, Vector3D_F* up = nullptr);

PX_SDK_TF2 void
AngleMatrix(const Angle_F& angles, Matrix3x4_F& matrix);

[[nodiscard]] PX_SDK_TF2 float
VecToYaw(const Vector3D_F& vec);

[[nodiscard]] PX_SDK_TF2 float
VecToPitch(const Vector3D_F& vec);

[[nodiscard]] PX_SDK_TF2 float
VecToYaw(const Matrix3x4_F& matrix, const Vector3D_F& vec);

[[nodiscard]] PX_SDK_TF2 float
VecToPitch(const Matrix3x4_F& matrix, const Vector3D_F& vec);

[[nodiscard]] PX_SDK_TF2 Vector3D_F
YawToVector(float yaw);

[[nodiscard]] PX_SDK_TF2 float
ApproachAngle(float target, float value, float speed);

[[nodiscard]] PX_SDK_TF2 float
AngleDiff(float destAngle, float srcAngle);

[[nodiscard]] PX_SDK_TF2 float
AngleDistance(float next, float cur);

[[nodiscard]] PX_SDK_TF2 float
GetFOV(const Vector3D_F& start, const Vector3D_F& end, const Angle_F& angle);

[[nodiscard]] constexpr float AngMod(float a) noexcept
{
	return (360.f / 65536) * (static_cast<int>((a * (65536.f / 360.0f))) & 65535);
}

[[nodiscard]] inline float AngleNormalize(float angle)
{
	angle = fmodf(angle, 360.f);
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

inline void SmoothAngle(const Angle_F& desired, Angle_F& cur_next_angle, const float rate)
{
	const Angle_F delta = (desired - cur_next_angle) * rate;

	cur_next_angle[0] = ApproachAngle(cur_next_angle[0], desired[0], delta[0]);
	cur_next_angle[1] = ApproachAngle(cur_next_angle[1], desired[1], delta[1]);

	ClampAngle(cur_next_angle);
}

TF2_NAMESPACE_END();

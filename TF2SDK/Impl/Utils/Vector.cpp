#include "Utils/Vector.hpp"
#include <numbers>


TF2_NAMESPACE_BEGIN(::Utils)

void VectorVectors(const Vector3D_F& forward, Vector3D_F& right, Vector3D_F& up)
{
	if (!forward[0] && !forward[1])
	{
		// pitch 90 degrees up/down from identity
		right[0] = 0.f;
		right[1] = -1.f;
		right[2] = 0.f;
		up[0] = -forward[2];
		up[1] = 0.f;
		up[2] = 0.f;
	}
	else
	{
		right = CrossProduct(forward, { 0.f, 0.f, 1.f });
		right.normalize();
		up = CrossProduct(right, forward);
		up.normalize();
	}
}

void VectorAngles(const Vector3D_F& forward, Angle_F& angles)
{
	float tmp, yaw, pitch;

	if (!forward[0] && !forward[1])
	{
		yaw = 0.f;
		if (forward[2] > 0.f)
			pitch = 270.f;
		else
			pitch = 90.f;
	}
	else
	{
		
		yaw = (atan2(forward[1], forward[0]) * 180.f / std::numbers::pi_v<float>);
		if (yaw < 0.f)
			yaw += 360.f;

		tmp = std::sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
		pitch = (atan2(-forward[2], tmp) * 180.f / std::numbers::pi_v<float>);
		if (pitch < 0.f)
			pitch += 360.f;
	}

	angles = { pitch, yaw, 0.f };
}

void AngleVectors(const Angle_F& angles, Vector3D_F* forward, Vector3D_F* right, Vector3D_F* up)
{
	float sr, sp, sy, cr, cp, cy;

	auto sin_cos = [](const float f, float& sval, float& cval)
	{
		sval = sin(f);
		cval = cos(f);
	};

	sin_cos(DegToRad(angles[0]), sp, cp);
	sin_cos(DegToRad(angles[1]), sy, cy);
	sin_cos(DegToRad(angles[2]), sr, cr);

	if (forward)
	{
		(*forward)[0] = cp * cy;
		(*forward)[1] = cp * sy;
		(*forward)[2] = -sp;
	}

	if (right)
	{
		(*right)[0] = (-1.f * sr * sp * cy + -1.f * cr * -sy);
		(*right)[1] = (-1.f * sr * sp * sy + -1.f * cr * cy);
		(*right)[2] = -1 * sr * cp;
	}

	if (up)
	{
		(*up)[0] = (cr * sp * cy + -sr * -sy);
		(*up)[1] = (cr * sp * sy + -sr * cy);
		(*up)[2] = cr * cp;
	}
}

float VecToYaw(const Vector3D_F& vec)
{
	if (!vec[0] && !vec[1])
		return 0.f;

	float yaw = RadToDeg(atan2(vec[1], vec[0]));

	if (yaw < 0.f)
		yaw += 360.f;

	return yaw;
}

float VecToPitch(const Vector3D_F& vec)
{
	if (!vec[0] && !vec[1])
	{
		if (vec[1]< 0.f)
			return 180.f;
		else
			return -180.f;
	}

	float dist = static_cast<float>(Vector2D_F({ vec[0], vec[1] }).length());
	float pitch = atan2f(-vec[1], dist);

	pitch = RadToDeg(pitch);

	return pitch;
}

float VecToYaw(const Matrix3x4_F& matrix, const Vector3D_F& vec)
{
	Vector3D_F tmp = vec;
	tmp.normalize();

	float x = matrix[0][0] * tmp[0] + matrix[1][0] * tmp[1] + matrix[2][0] * tmp[2];
	float y = matrix[0][1] * tmp[0] + matrix[1][1] * tmp[1] + matrix[2][1] * tmp[2];

	if (!x || !y)
		return .0f;

	float yaw = RadToDeg(atan2(-y, x));

	if (yaw < 0.f)
		yaw += 360.f;

	return yaw;
}

float VecToPitch(const Matrix3x4_F& matrix, const Vector3D_F& vec)
{
	Vector3D_F tmp = vec;
	tmp.normalize();

	float x = matrix[0][0] * tmp[0] + matrix[1][0] * tmp[1] + matrix[2][0] * tmp[2];
	float z = matrix[0][2] * tmp[0] + matrix[1][2] * tmp[1] + matrix[2][2] * tmp[2];

	if (!x || !z)
		return 0.f;

	float pitch = RadToDeg(atan2(z, x));

	if (pitch < 0.f)
		pitch += 360.f;

	return pitch;
}

Vector3D_F YawToVector(float yaw)
{
	float angle = RadToDeg(yaw);
	return { cos(angle), sin(angle), 0.f };
}

float ApproachAngle(float target, float value, float speed)
{
	target = AngMod(target);
	value = AngMod(value);

	float delta = target - value;

	// Speed is assumed to be positive
	if (speed < 0.f)
		speed = -speed;

	if (delta < -180.f)
		delta += 360.f;
	else if (delta > 180.f)
		delta -= 360.f;

	if (delta > speed)
		value += speed;
	else if (delta < -speed)
		value -= speed;
	else
		value = target;

	return value;
}

float AngleDiff(float destAngle, float srcAngle)
{
	float delta;

	delta = fmodf(destAngle - srcAngle, 360.f);
	if (destAngle > srcAngle)
	{
		if (delta >= 180.f)
			delta -= 360.f;
	}
	else
	{
		if (delta <= -180.f)
			delta += 360.f;
	}
	return delta;
}


float AngleDistance(float next, float cur)
{
	float delta = next - cur;

	if (delta < -180.f)
		delta += 360.f;
	else if (delta > 180.f)
		delta -= 360.f;

	return delta;
}

float GetFOV(const Vector3D_F& start, const Vector3D_F& end, const Vector3D_F& angle)
{
	Vector3D_F vecfwd;
	AngleVectors(angle, &vecfwd);
	vecfwd.normalize();

	Vector3D_F delta = end - start;
	delta.normalize();

	const float dot = static_cast<float>(vecfwd.dot(delta));
	const float pow_len = powf(static_cast<float>(std::abs(vecfwd.length())), 2.f);

	return RadToDeg(acosf(dot / (pow_len)));
}


TF2_NAMESPACE_END();

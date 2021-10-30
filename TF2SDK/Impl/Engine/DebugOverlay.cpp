
#include <numeric>
#include <algorithm>

#include "Engine/DebugOverlay.hpp"
#include "Entity/BasePlayer.hpp"
#include "Entity/BaseEntity.hpp"

#include "Utils/Trace.hpp"
#include "Utils/Vector.hpp"


TF2_NAMESPACE_BEGIN();

#define	MAX_OVERLAY_DIST_SQR	90000000

//-----------------------------------------------------------------------------
// Purpose: Draw a box with no orientation
//-----------------------------------------------------------------------------
void NDebugOverlay::Box(const Vector3D_F& origin, const Vector3D_F& mins, const Vector3D_F& maxs, int r, int g, int b, int a, float flDuration)
{
	BoxAngles(origin, mins, maxs, { }, r, g, b, a, flDuration);
}

//-----------------------------------------------------------------------------
// Purpose: Draw box oriented to a Vector3D_F direction
//-----------------------------------------------------------------------------
void NDebugOverlay::BoxDirection(const Vector3D_F& origin, const Vector3D_F& mins, const Vector3D_F& maxs, const Vector3D_F& orientation, int r, int g, int b, int a, float duration)
{
	// convert forward Vector3D_F to angles
	Angle_F f_angles{ 0.f, 0.f, Utils::VecToYaw(orientation) };

	BoxAngles(origin, mins, maxs, f_angles, r, g, b, a, duration);
}

//-----------------------------------------------------------------------------
// Purpose: Draw box oriented to a Angle_F direction
//-----------------------------------------------------------------------------
void NDebugOverlay::BoxAngles(const Vector3D_F& origin, const Vector3D_F& mins, const Vector3D_F& maxs, const Angle_F& angles, int r, int g, int b, int a, float duration)
{
	Interfaces::DebugOverlay->AddBoxOverlay(origin, mins, maxs, angles, r, g, b, a, duration);
}

//-----------------------------------------------------------------------------
// Purpose: Draws a swept box
//-----------------------------------------------------------------------------
void NDebugOverlay::SweptBox(const Vector3D_F& start, const Vector3D_F& end, const Vector3D_F& mins, const Vector3D_F& maxs, const Angle_F& angles, int r, int g, int b, int a, float flDuration)
{
	Interfaces::DebugOverlay->AddSweptBoxOverlay(start, end, mins, maxs, angles, r, g, b, a, flDuration);
}

//-----------------------------------------------------------------------------
// Purpose: Draws a box around an entity
//-----------------------------------------------------------------------------
void NDebugOverlay::EntityBounds(IBaseEntityInternal* pEntity, int r, int g, int b, int a, float flDuration)
{
	const ICollideable* pCollide = pEntity->CollisionProp.data();
	BoxAngles(pCollide->GetCollisionOrigin(), pCollide->OBBMins(), pCollide->OBBMaxs(), pCollide->GetCollisionAngles(), r, g, b, a, flDuration);
}

//-----------------------------------------------------------------------------
// Purpose: Draws a line from one position to another
//-----------------------------------------------------------------------------
void NDebugOverlay::Line(const Vector3D_F& origin, const Vector3D_F& target, int r, int g, int b, bool noDepthTest, float duration)
{
	// --------------------------------------------------------------
	// Clip the line before sending so we 
	// don't overflow the client message buffer
	// --------------------------------------------------------------
	ILocalPlayer player;
	if (!player)
		return;

	const Vector3D_F& my_origin = player->GetAbsOrigin();

	// Clip line that is far away
	if (((my_origin - origin).length_sqr() > MAX_OVERLAY_DIST_SQR) &&
		((my_origin - target).length_sqr() > MAX_OVERLAY_DIST_SQR))
		return;

	// Clip line that is behind the client 
	Vector3D_F clientForward;
	player->EyeVectors(&clientForward);

	Vector3D_F toOrigin = origin - my_origin;
	Vector3D_F toTarget = target - my_origin;

	if (clientForward.dot(toOrigin) < .0 && clientForward.dot(toTarget) < .0)
		return;

	Interfaces::DebugOverlay->AddLineOverlay(origin, target, r, g, b, noDepthTest, duration);
}


//-----------------------------------------------------------------------------
// Purpose: Draw triangle
//-----------------------------------------------------------------------------
void NDebugOverlay::Triangle(const Vector3D_F& p1, const Vector3D_F& p2, const Vector3D_F& p3, int r, int g, int b, int a, bool noDepthTest, float duration)
{
	ILocalPlayer player;
	if (!player)
		return;

	// Clip triangles that are far away
	Vector3D_F to1 = p1 - player->GetAbsOrigin();
	Vector3D_F to2 = p2 - player->GetAbsOrigin();
	Vector3D_F to3 = p3 - player->GetAbsOrigin();

	if ((to1.length_sqr() > MAX_OVERLAY_DIST_SQR) &&
		(to2.length_sqr() > MAX_OVERLAY_DIST_SQR) &&
		(to3.length_sqr() > MAX_OVERLAY_DIST_SQR))
	{
		return;
	}

	// Clip triangles that are behind the client 
	Vector3D_F clientForward;
	player->EyeVectors(&clientForward);

	if (clientForward.dot(to1) < .0 &&
		clientForward.dot(to2) < .0 &&
		clientForward.dot(to3) < .0)
		return;

	Interfaces::DebugOverlay->AddTriangleOverlay(p1, p2, p3, r, g, b, a, noDepthTest, duration);
}

//-----------------------------------------------------------------------------
// Purpose: Draw entity text overlay
//-----------------------------------------------------------------------------
void NDebugOverlay::EntityText(int entityID, int text_offset, const char* text, float duration, int r, int g, int b, int a)
{
	Interfaces::DebugOverlay->AddEntityTextOverlay(
		entityID,
		text_offset,
		duration,
		static_cast<int>(std::clamp(r * 255.f, 0.f, 255.f)),
		static_cast<int>(std::clamp(g * 255.f, 0.f, 255.f)),
		static_cast<int>(std::clamp(b * 255.f, 0.f, 255.f)),
		static_cast<int>(std::clamp(a * 255.f, 0.f, 255.f)),
		text
	);
}

//-----------------------------------------------------------------------------
// Purpose: Draw entity text overlay at a specific position
//-----------------------------------------------------------------------------
void NDebugOverlay::EntityTextAtPosition(const Vector3D_F& origin, int text_offset, const char* text, float duration, int r, int g, int b, int a)
{
	Interfaces::DebugOverlay->AddTextOverlayRGB(origin, text_offset, duration, r, g, b, a, "%s", text);
}

//-----------------------------------------------------------------------------
// Purpose: Add grid overlay 
//-----------------------------------------------------------------------------
void NDebugOverlay::Grid(const Vector3D_F& vPosition)
{
	Interfaces::DebugOverlay->AddGridOverlay(vPosition);
}

//-----------------------------------------------------------------------------
// Purpose: Draw debug text at a position
//-----------------------------------------------------------------------------
void NDebugOverlay::Text(const Vector3D_F& origin, const char* text, float duration)
{
	ILocalPlayer player;
	if (!player)
		return;

	// Clip text that is far away
	if ((player->GetAbsOrigin() - origin).length_sqr() > MAX_OVERLAY_DIST_SQR)
		return;

	// Clip text that is behind the client 
	Vector3D_F clientForward;
	player->EyeVectors(&clientForward);

	Vector3D_F toText = origin - player->GetAbsOrigin();

	if (clientForward.dot(toText) < .0)
		return;

	Interfaces::DebugOverlay->AddTextOverlay(origin, duration, "%s", text);
}

//-----------------------------------------------------------------------------
// Purpose: Add debug overlay text with screen position
//-----------------------------------------------------------------------------
void NDebugOverlay::ScreenText(float flXpos, float flYpos, const char* text, int r, int g, int b, int a, float duration)
{
	Interfaces::DebugOverlay->AddScreenTextOverlay(flXpos, flYpos, duration, r, g, b, a, text);
}

//-----------------------------------------------------------------------------
// Purpose: Draw a colored 3D cross of the given hull size at the given position
//-----------------------------------------------------------------------------
void NDebugOverlay::Cross3D(const Vector3D_F& position, const Vector3D_F& mins, const Vector3D_F& maxs, int r, int g, int b, bool noDepthTest, float fDuration)
{
	Vector3D_F start = mins + position;
	Vector3D_F end = maxs + position;
	Line(start, end, r, g, b, noDepthTest, fDuration);

	start[0] += (maxs[0] - mins[0]);
	end[0] -= (maxs[0] - mins[0]);
	Line(start, end, r, g, b, noDepthTest, fDuration);

	start[1] += (maxs[1] - mins[1]);
	end[1] -= (maxs[1] - mins[1]);
	Line(start, end, r, g, b, noDepthTest, fDuration);

	start[0] -= (maxs[0] - mins[0]);
	end[0] += (maxs[0] - mins[0]);
	Line(start, end, r, g, b, noDepthTest, fDuration);
}

//-----------------------------------------------------------------------------
// Purpose: Draw a colored 3D cross of the given size at the given position
//-----------------------------------------------------------------------------
void NDebugOverlay::Cross3D(const Vector3D_F& position, float size, int r, int g, int b, bool noDepthTest, float flDuration)
{
	Line(position + VectorXD{ size, 0.f, 0.f }, position - VectorXD{ size, 0.f, 0.f }, r, g, b, noDepthTest, flDuration);
	Line(position + VectorXD{ 0.f, size, 0.f }, position - VectorXD{ 0.f, size, 0.f }, r, g, b, noDepthTest, flDuration);
	Line(position + VectorXD{ 0.f, 0.f, size }, position - VectorXD{ 0.f, 0.f, size }, r, g, b, noDepthTest, flDuration);
}

//-----------------------------------------------------------------------------
// Purpose: Draw an oriented, colored 3D cross of the given size at the given position (via a Vector3D_F)
//-----------------------------------------------------------------------------
void NDebugOverlay::Cross3DOriented(const Vector3D_F& position, const Angle_F& angles, float size, int r, int g, int b, bool noDepthTest, float flDuration)
{
	Vector3D_F forward, right, up;
	Utils::AngleVectors(angles, &forward, &right, &up);

	forward *= size;
	right *= size;
	up *= size;

	Line(position + right, position - right, r, g, b, noDepthTest, flDuration);
	Line(position + forward, position - forward, r, g, b, noDepthTest, flDuration);
	Line(position + up, position - up, r, g, b, noDepthTest, flDuration);
}

//--------------------------------------------------------------------------------
// Purpose : Draw tick marks between start and end position of the given distance
//			 with text every tickTextDist steps apart. 
//--------------------------------------------------------------------------------
void NDebugOverlay::DrawTickMarkedLine(const Vector3D_F& startPos, const Vector3D_F& endPos, float tickDist, int tickTextDist, int r, int g, int b, bool noDepthTest, float duration)
{
	ILocalPlayer player;
	if (!player)
		return;

	Vector3D_F lineDir = endPos - startPos;
	double	lineDist = lineDir.normalize();
	int		numTicks = static_cast<int>(lineDist / tickDist);
	Vector3D_F	vBodyDir;

	Utils::AngleVectors(player->EyeAngles, &vBodyDir);

	Vector3D_F  upVec = vBodyDir * 4;
	Vector3D_F	tickPos = startPos;
	int		tickTextCnt = 0;

	Vector3D_F	sideDir
	{
		lineDir[1] * upVec[2] - lineDir[2] * upVec[1],
		lineDir[2] * upVec[0] - lineDir[0] * upVec[2],
		lineDir[0] * upVec[1] - lineDir[1] * upVec[2]
	};

	// First draw the line
	Line(startPos, endPos, r, g, b, noDepthTest, duration);

	// Now draw the ticks
	for (int i = 0; i < numTicks + 1; i++)
	{
		// Draw tick mark
		Vector3D_F tickLeft = tickPos - sideDir;
		Vector3D_F tickRight = tickPos + sideDir;

		// Draw tick mark text
		if (tickTextCnt == tickTextDist)
		{
			char text[25];
			snprintf(text, sizeof(text), "%i", i);
			Vector3D_F textPos = tickLeft + VectorXD{ 0.f, 0.f, 8.f };
			Line(tickLeft, tickRight, 255, 255, 255, noDepthTest, duration);
			Text(textPos, text, 0.f);
			tickTextCnt = 0;
		}
		else
		{
			Line(tickLeft, tickRight, r, g, b, noDepthTest, duration);
		}

		tickTextCnt++;

		tickPos = tickPos + (lineDir * tickDist);
	}
}

//------------------------------------------------------------------------------
// Purpose : Draw crosshair on ground where player is looking
//------------------------------------------------------------------------------
void NDebugOverlay::DrawGroundCrossHairOverlay(void)
{
	ILocalPlayer player;
	if (!player)
		return;

	// Trace a line to where player is looking
	Vector3D_F vForward;
	Vector3D_F vSource = player->EyePosition();
	player->EyeVectors(&vForward);

	GameTrace tr;
	Utils::FilterSimple filter(player.get());

	Utils::TraceLine(vSource, vSource + vForward * 2048, Const::TraceMask::Solid, &tr, &filter);
	float dotPr = static_cast<float>(tr.Plane.Normal.dot({ 0, 0, 1 }));

	if (tr.Fraction != 1.0 && dotPr > 0.5)
	{
		tr.EndPos[2] += 1;
		float	scale = 6.f;
		Vector3D_F	startPos = tr.EndPos + Vector3D_F{ -scale, 0, 0 };
		Vector3D_F	endPos = tr.EndPos + Vector3D_F{ scale, 0, 0 };
		Line(startPos, endPos, 255, 0, 0, false, 0);

		startPos = tr.EndPos + Vector3D_F{ 0, -scale, 0 };
		endPos = tr.EndPos + Vector3D_F{ 0, scale, 0 };
		Line(startPos, endPos, 255, 0, 0, false, 0);
	}
}

//--------------------------------------------------------------------------------
// Purpose : Draw a horizontal arrow pointing in the specified direction
//--------------------------------------------------------------------------------
void NDebugOverlay::HorzArrow(const Vector3D_F& startPos, const Vector3D_F& endPos, float width, int r, int g, int b, int a, bool noDepthTest, float flDuration)
{
	Vector3D_F lineDir = (endPos - startPos);
	lineDir.normalize();
	Vector3D_F upVec{ 0.f, 0.f, 1.f };
	float radius = width / 2.0f;

	VectorXD sideDir = Utils::CrossProduct(lineDir, upVec);

	Vector3D_F p1 = startPos - sideDir * radius;
	Vector3D_F p2 = endPos - lineDir * width - sideDir * radius;
	Vector3D_F p3 = endPos - lineDir * width - sideDir * width;
	Vector3D_F p4 = endPos;
	Vector3D_F p5 = endPos - lineDir * width + sideDir * width;
	Vector3D_F p6 = endPos - lineDir * width + sideDir * radius;
	Vector3D_F p7 = startPos + sideDir * radius;

	// Outline the arrow
	Line(p1, p2, r, g, b, noDepthTest, flDuration);
	Line(p2, p3, r, g, b, noDepthTest, flDuration);
	Line(p3, p4, r, g, b, noDepthTest, flDuration);
	Line(p4, p5, r, g, b, noDepthTest, flDuration);
	Line(p5, p6, r, g, b, noDepthTest, flDuration);
	Line(p6, p7, r, g, b, noDepthTest, flDuration);

	if (a > 0)
	{
		// Fill us in with triangles
		Triangle(p5, p4, p3, r, g, b, a, noDepthTest, flDuration); // Tip
		Triangle(p1, p7, p6, r, g, b, a, noDepthTest, flDuration); // Shaft
		Triangle(p6, p2, p1, r, g, b, a, noDepthTest, flDuration);

		// And backfaces
		Triangle(p3, p4, p5, r, g, b, a, noDepthTest, flDuration); // Tip
		Triangle(p6, p7, p1, r, g, b, a, noDepthTest, flDuration); // Shaft
		Triangle(p1, p2, p6, r, g, b, a, noDepthTest, flDuration);
	}
}

//-----------------------------------------------------------------------------
// Purpose : Draw a horizontal arrow pointing in the specified direction by yaw value
//-----------------------------------------------------------------------------
void NDebugOverlay::YawArrow(const Vector3D_F& startPos, float yaw, float length, float width, int r, int g, int b, int a, bool noDepthTest, float flDuration)
{
	Vector3D_F forward = Utils::YawToVector(yaw);

	HorzArrow(startPos, startPos + forward * length, width, r, g, b, a, noDepthTest, flDuration);
}

//--------------------------------------------------------------------------------
// Purpose : Draw a vertical arrow at a position
//--------------------------------------------------------------------------------
void NDebugOverlay::VertArrow(const Vector3D_F& startPos, const Vector3D_F& endPos, float width, int r, int g, int b, int a, bool noDepthTest, float flDuration)
{
	Vector3D_F	lineDir = (endPos - startPos);
	lineDir.normalize();
	Vector3D_F  upVec;
	Vector3D_F	sideDir;
	float   radius = width / 2.0f;

	Utils::VectorVectors(lineDir, sideDir, upVec);

	Vector3D_F p1 = startPos - upVec * radius;
	Vector3D_F p2 = endPos - lineDir * width - upVec * radius;
	Vector3D_F p3 = endPos - lineDir * width - upVec * width;
	Vector3D_F p4 = endPos;
	Vector3D_F p5 = endPos - lineDir * width + upVec * width;
	Vector3D_F p6 = endPos - lineDir * width + upVec * radius;
	Vector3D_F p7 = startPos + upVec * radius;

	// Outline the arrow
	Line(p1, p2, r, g, b, noDepthTest, flDuration);
	Line(p2, p3, r, g, b, noDepthTest, flDuration);
	Line(p3, p4, r, g, b, noDepthTest, flDuration);
	Line(p4, p5, r, g, b, noDepthTest, flDuration);
	Line(p5, p6, r, g, b, noDepthTest, flDuration);
	Line(p6, p7, r, g, b, noDepthTest, flDuration);

	if (a > 0)
	{
		// Fill us in with triangles
		Triangle(p5, p4, p3, r, g, b, a, noDepthTest, flDuration); // Tip
		Triangle(p1, p7, p6, r, g, b, a, noDepthTest, flDuration); // Shaft
		Triangle(p6, p2, p1, r, g, b, a, noDepthTest, flDuration);

		// And backfaces
		Triangle(p3, p4, p5, r, g, b, a, noDepthTest, flDuration); // Tip
		Triangle(p6, p7, p1, r, g, b, a, noDepthTest, flDuration); // Shaft
		Triangle(p1, p2, p6, r, g, b, a, noDepthTest, flDuration);
	}
}

//-----------------------------------------------------------------------------
// Purpose: Draw an axis
//-----------------------------------------------------------------------------
void NDebugOverlay::Axis(const Vector3D_F& position, const Angle_F& angles, float size, bool noDepthTest, float flDuration)
{
	Vector3D_F xvec, yvec, zvec;
	Utils::AngleVectors(angles, &xvec, &yvec, &zvec);

	xvec = position + (xvec * size);
	yvec = position - (yvec * size); // Left is positive
	zvec = position + (zvec * size);

	Line(position, xvec, 255, 0, 0, noDepthTest, flDuration);
	Line(position, yvec, 0, 255, 0, noDepthTest, flDuration);
	Line(position, zvec, 0, 0, 255, noDepthTest, flDuration);
}

//-----------------------------------------------------------------------------
// Purpose: Draw circles to suggest a sphere
//-----------------------------------------------------------------------------
void NDebugOverlay::Sphere(const Vector3D_F& center, float radius, int r, int g, int b, bool noDepthTest, float flDuration)
{
	Vector3D_F edge;

	float axisSize = radius;
	Line(center + VectorXD{ 0.f, 0.f, -axisSize }, center + VectorXD{ 0.f, 0.f, axisSize }, r, g, b, noDepthTest, flDuration);
	Line(center + VectorXD{ 0.f, -axisSize, 0.f }, center + VectorXD{ 0.f, axisSize, 0.f }, r, g, b, noDepthTest, flDuration);
	Line(center + VectorXD{ -axisSize, 0.f, 0.f }, center + VectorXD{ axisSize, 0.f, 0.f }, r, g, b, noDepthTest, flDuration);

	VectorXD lastEdge = VectorXD{ radius + center[0], center[1], center[2] };
	for (float angle = 0.0f; angle <= 360.0f; angle += 22.5f)
	{
		edge[0] = radius * cosf(angle / 180.0f * std::numbers::pi_v<float>) + center[0];
		edge[1] = center[1];
		edge[2] = radius * sinf(angle / 180.0f * std::numbers::pi_v<float>) + center[2];

		Line(edge, lastEdge, r, g, b, noDepthTest, flDuration);

		lastEdge = edge;
	}

	lastEdge = VectorXD{ center[0], radius + center[1], center[2] };
	for (float angle = 0.0f; angle <= 360.0f; angle += 22.5f)
	{
		edge[0] = center[0];
		edge[1] = radius * cosf(angle / 180.0f * std::numbers::pi_v<float>) + center[1];
		edge[2] = radius * sinf(angle / 180.0f * std::numbers::pi_v<float>) + center[2];

		Line(edge, lastEdge, r, g, b, noDepthTest, flDuration);

		lastEdge = edge;
	}

	lastEdge = VectorXD{ center[0], radius + center[1], center[2] };
	for (float angle = 0.0f; angle <= 360.0f; angle += 22.5f)
	{
		edge[0] = radius * cosf(angle / 180.0f * std::numbers::pi_v<float>) + center[0];
		edge[1] = radius * sinf(angle / 180.0f * std::numbers::pi_v<float>) + center[1];
		edge[2] = center[2];

		Line(edge, lastEdge, r, g, b, noDepthTest, flDuration);

		lastEdge = edge;
	}
}

//-----------------------------------------------------------------------------
// Purpose: Draw a circle whose center is at a position, facing the camera
//-----------------------------------------------------------------------------
void NDebugOverlay::Circle(const Vector3D_F& position, float radius, int r, int g, int b, int a, bool bNoDepthTest, float flDuration)
{
	ILocalPlayer player;
	if (!player)
		return;

	Vector3D_F clientForward;
	player->EyeVectors(&clientForward);

	Angle_F vecAngles;
	Utils::VectorAngles(clientForward, vecAngles);

	Circle(position, vecAngles, radius, r, g, b, a, bNoDepthTest, flDuration);
}


//-----------------------------------------------------------------------------
// Purpose: Draw a circle whose center is at a position and is facing a specified direction
//-----------------------------------------------------------------------------
void NDebugOverlay::Circle(const Vector3D_F& position, const Angle_F& angles, float radius, int r, int g, int b, int a, bool bNoDepthTest, float flDuration)
{
	// Setup our transform matrix
	Matrix3x4_F xform;
	Utils::AngleMatrix(angles, xform);
	xform.set_column(3, position);
	// default draws circle in the y/z plane
	VectorXD xAxis = xform.get_column(2);
	VectorXD yAxis = xform.get_column(1);
	Circle(position, xAxis, yAxis, radius, r, g, b, a, bNoDepthTest, flDuration);
}

void NDebugOverlay::Circle(const Vector3D_F& position, const Vector3D_F& xAxis, const Vector3D_F& yAxis, float radius, int r, int g, int b, int a, bool bNoDepthTest, float flDuration)
{
	constexpr unsigned int nSegments = 16;
	const float flRadStep = (std::numbers::pi_v<float> * 2.0f) / nSegments;

	// Find our first position
	// Retained for triangle fanning
	Vector3D_F vecStart = position + xAxis * radius;
	Vector3D_F vecLastPosition = vecStart;
	Vector3D_F vecPosition = position + (xAxis * radius * 0.923879f) + (yAxis * radius * 0.382683f);

	// Draw out each segment (fanning triangles if we have an alpha amount)
	for (int i = 2; i <= nSegments; i++)
	{
		// Store off our last position
		vecLastPosition = vecPosition;

		// Calculate the new one
		float sin = std::sinf(flRadStep * i);
		float cos = std::cosf(flRadStep * i);
		vecPosition = position + (xAxis * cos * radius) + (yAxis * sin * radius);

		// Draw the line
		Line(vecLastPosition, vecPosition, r, g, b, bNoDepthTest, flDuration);

		Interfaces::DebugOverlay->AddTriangleOverlay(vecStart, vecLastPosition, vecPosition, r, g, b, a, bNoDepthTest, flDuration);
	}
}

void NDebugOverlay::Sphere(const Vector3D_F& position, const Angle_F& angles, float radius, int r, int g, int b, int a, bool bNoDepthTest, float flDuration)
{
	// Setup our transform matrix
	Matrix3x4_F xform;
	Utils::AngleMatrix(angles, xform);
	xform.set_column(3, position);
	// default draws circle in the y/z plane
	VectorXD xAxis = xform.get_column(0);
	VectorXD yAxis = xform.get_column(1);
	VectorXD zAxis = xform.get_column(2);
	Circle(position, xAxis, yAxis, radius, r, g, b, a, bNoDepthTest, flDuration);	// xy plane
	Circle(position, yAxis, zAxis, radius, r, g, b, a, bNoDepthTest, flDuration);	// yz plane
	Circle(position, xAxis, zAxis, radius, r, g, b, a, bNoDepthTest, flDuration);	// xz plane
}

TF2_NAMESPACE_END();

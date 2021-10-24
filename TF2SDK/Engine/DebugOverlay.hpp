#pragma once

#include "Math/Vector.hpp"

TF2_NAMESPACE_BEGIN();

class IVDebugOverlay;
class IBaseEntityInternal;

namespace Interfaces
{
	TF2_EXPORT_INTERFACE(IVDebugOverlay, DebugOverlay, "VDebugOverlay003");
}

struct OverlayLine
{
	Vector3D_F		Origin;
	Vector3D_F		Dest;
	int				R;
	int				G;
	int				B;
	bool			NoDepthTest;
	bool			Draw;
};

class IVDebugOverlay
{
public:
	virtual void AddEntityTextOverlay(int ent_index, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) abstract;
	virtual void AddBoxOverlay(const Vector3D_F& origin, const Vector3D_F& mins, const Vector3D_F& max, const Angle_F& orientation, int r, int g, int b, int a, float duration) abstract;
	virtual void AddTriangleOverlay(const Vector3D_F& p1, const Vector3D_F& p2, const Vector3D_F& p3, int r, int g, int b, int a, bool noDepthTest, float duration) abstract;
	virtual void AddLineOverlay(const Vector3D_F& origin, const Vector3D_F& dest, int r, int g, int b, bool noDepthTest, float duration) abstract;
	virtual void AddTextOverlay(const Vector3D_F& origin, float duration, const char* format, ...) abstract;
	virtual void AddTextOverlay(const Vector3D_F& origin, int line_offset, float duration, const char* format, ...) abstract;
	virtual void AddScreenTextOverlay(float flXPos, float flYPos, float flDuration, int r, int g, int b, int a, const char* text) abstract;
	virtual void AddSweptBoxOverlay(const Vector3D_F& start, const Vector3D_F& end, const Vector3D_F& mins, const Vector3D_F& max, const Angle_F& angles, int r, int g, int b, int a, float flDuration) abstract;
	virtual void AddGridOverlay(const Vector3D_F& origin) abstract;
	virtual int ScreenPosition(const Vector3D_F& point, Vector3D_F& screen) abstract;
	virtual int ScreenPosition(float flXPos, float flYPos, Vector3D_F& screen) abstract;

	virtual OverlayLine* GetFirst() abstract;
	virtual OverlayLine* GetNext(OverlayLine* current) abstract;
	virtual void ClearDeadOverlays() abstract;
	virtual void ClearAllOverlays() abstract;

	virtual void AddTextOverlayRGB(const Vector3D_F& origin, int line_offset, float duration, float r, float g, float b, float alpha, const char* format, ...) abstract;
	virtual void AddTextOverlayRGB(const Vector3D_F& origin, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) abstract;

	virtual void AddLineOverlayAlpha(const Vector3D_F& origin, const Vector3D_F& dest, int r, int g, int b, int a, bool noDepthTest, float duration) abstract;
	virtual void AddBoxOverlay2(const Vector3D_F& origin, const Vector3D_F& mins, const Vector3D_F& max, const Angle_F& orientation, const Color3_I8& faceColor, const Color3_I8& edgeColor, float duration) abstract;

private:
	inline void AddTextOverlay(const Vector3D_F& origin, int line_offset, float duration, int r, int g, int b, int a, const char* format, ...) {}
};

namespace NDebugOverlay
{
	SG_SDK_TF2 void Box(const Vector3D_F& origin, const Vector3D_F& mins, const Vector3D_F& maxs, int r, int g, int b, int a, float flDuration);
	SG_SDK_TF2 void BoxDirection(const Vector3D_F& origin, const Vector3D_F& mins, const Vector3D_F& maxs, const Vector3D_F& forward, int r, int g, int b, int a, float flDuration);
	SG_SDK_TF2 void BoxAngles(const Vector3D_F& origin, const Vector3D_F& mins, const Vector3D_F& maxs, const Angle_F& angles, int r, int g, int b, int a, float flDuration);
	SG_SDK_TF2 void SweptBox(const Vector3D_F& start, const Vector3D_F& end, const Vector3D_F& mins, const Vector3D_F& maxs, const Angle_F& angles, int r, int g, int b, int a, float flDuration);
	SG_SDK_TF2 void EntityBounds(IBaseEntityInternal* pEntity, int r, int g, int b, int a, float flDuration);
	SG_SDK_TF2 void Line(const Vector3D_F& origin, const Vector3D_F& target, int r, int g, int b, bool noDepthTest, float flDuration);
	SG_SDK_TF2 void Triangle(const Vector3D_F& p1, const Vector3D_F& p2, const Vector3D_F& p3, int r, int g, int b, int a, bool noDepthTest, float duration);
	SG_SDK_TF2 void EntityText(int entityID, int text_offset, const char* text, float flDuration, int r = 255, int g = 255, int b = 255, int a = 255);
	SG_SDK_TF2 void EntityTextAtPosition(const Vector3D_F& origin, int text_offset, const char* text, float flDuration, int r = 255, int g = 255, int b = 255, int a = 255);
	SG_SDK_TF2 void Grid(const Vector3D_F& vPosition);
	SG_SDK_TF2 void Text(const Vector3D_F& origin, const char* text, float flDuration);
	SG_SDK_TF2 void ScreenText(float fXpos, float fYpos, const char* text, int r, int g, int b, int a, float flDuration);
	SG_SDK_TF2 void Cross3D(const Vector3D_F& position, const Vector3D_F& mins, const Vector3D_F& maxs, int r, int g, int b, bool noDepthTest, float flDuration);
	SG_SDK_TF2 void Cross3D(const Vector3D_F& position, float size, int r, int g, int b, bool noDepthTest, float flDuration);
	SG_SDK_TF2 void Cross3DOriented(const Vector3D_F& position, const Angle_F& angles, float size, int r, int g, int b, bool noDepthTest, float flDuration);
	SG_SDK_TF2 void DrawTickMarkedLine(const Vector3D_F& startPos, const Vector3D_F& endPos, float tickDist, int tickTextDist, int r, int g, int b, bool noDepthTest, float flDuration);
	SG_SDK_TF2 void DrawGroundCrossHairOverlay();
	SG_SDK_TF2 void HorzArrow(const Vector3D_F& startPos, const Vector3D_F& endPos, float width, int r, int g, int b, int a, bool noDepthTest, float flDuration);
	SG_SDK_TF2 void YawArrow(const Vector3D_F& startPos, float yaw, float length, float width, int r, int g, int b, int a, bool noDepthTest, float flDuration);
	SG_SDK_TF2 void VertArrow(const Vector3D_F& startPos, const Vector3D_F& endPos, float width, int r, int g, int b, int a, bool noDepthTest, float flDuration);
	SG_SDK_TF2 void Axis(const Vector3D_F& position, const Angle_F& angles, float size, bool noDepthTest, float flDuration);
	SG_SDK_TF2 void Sphere(const Vector3D_F& center, float radius, int r, int g, int b, bool noDepthTest, float flDuration);
	SG_SDK_TF2 void Circle(const Vector3D_F& position, float radius, int r, int g, int b, int a, bool bNoDepthTest, float flDuration);
	SG_SDK_TF2 void Circle(const Vector3D_F& position, const Angle_F& angles, float radius, int r, int g, int b, int a, bool bNoDepthTest, float flDuration);
	SG_SDK_TF2 void Circle(const Vector3D_F& position, const Vector3D_F& xAxis, const Vector3D_F& yAxis, float radius, int r, int g, int b, int a, bool bNoDepthTest, float flDuration);
	SG_SDK_TF2 void Sphere(const Vector3D_F& position, const Angle_F& angles, float radius, int r, int g, int b, int a, bool bNoDepthTest, float flDuration);
};
TF2_NAMESPACE_END();

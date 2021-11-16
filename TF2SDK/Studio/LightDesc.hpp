#pragma once

#include "Math/Vector.hpp"

TF2_NAMESPACE_BEGIN();

//-----------------------------------------------------------------------------
// Light structure
//-----------------------------------------------------------------------------

namespace Const
{
	enum class LightType
	{
		Disable,
		Point,
		Directional,
		Spot
	};

	namespace LightType_OptimizationFlags
	{
		static constexpr uint32_t Attenuation0 = 1;
		static constexpr uint32_t Attenuation1 = 2;
		static constexpr uint32_t Attenuation2 = 4;
		static constexpr uint32_t Derived_Values_Calculed = 8;
	};
}

struct LightDesc_t
{
	Const::LightType	Type;											//< MATERIAL_LIGHT_xxx
	Vector3D_F			Color;											//< color+intensity 
	Vector3D_F			Position;										//< light source center position
	Vector3D_F			Direction;										//< for SPOT, direction it is pointing
	float				Range;											//< distance range for light.0=infinite
	float				Falloff;										//< angular falloff exponent for spot lights
	float				Attenuation0;									//< constant distance falloff term
	float				Attenuation1;									//< linear term of falloff
	float				Attenuation2;									//< quadatic term of falloff
	float				Theta;											//< inner cone angle. no angular falloff 
																		//< within this cone
	float				Phi;											//< outer cone angle

	// the values below are derived from the above settings for optimizations
	// These aren't used by DX8. . used for software lighting.
	float				ThetaDot;
	float				PhiDot;
	uint32_t			Flags;

	float				OneOver_ThetaDot_Minus_PhiDot;
	float				RangeSquared;
};

TF2_NAMESPACE_END();

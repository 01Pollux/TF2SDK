#pragma once

#include "TF2Config.hpp"
#include "Entity/BaseEntity.hpp"
#include "Math/Vector.hpp"

TF2_NAMESPACE_BEGIN();

class IBaseEntityInternal;

enum class ParticleAttachment
{
	// Create at absorigin, but don't follow
	AbsOrigin = 0,			
	// Create at absorigin, and update to follow the entity
	AbsOriginFollow,		
	// Create at a custom origin, but don't follow
	CustomOrigin,
	// Create on attachment point, but don't follow
	Point,
	// Create on attachment point, and update to follow the entity
	PointFollow,

	// Used for control points that don't attach to an entity
	WorldOrigin,

	// Create at the root bone of the entity, and update to follow
	RootBoneFollow,

	Count,
};

namespace Const
{
	namespace DispatchType
	{
		static constexpr uint32_t FromEntity = 1 << 0;
		static constexpr uint32_t ResetParticles = 1 << 1;
	}
}

struct TFParticleEffectColor
{
	Vector3D_F Color1;
	Vector3D_F Color2;
};

struct TFParticleEffectPoint
{
	ParticleAttachment AttachmentType;
	Vector3D_F Offset;
};

class TFEffectData
{
public:
	Vector3D_F	Origin{ };
	Vector3D_F	Start{ };
	Vector3D_F	Normal{ };
	Angle_F		Angles{ };
	int			Flags{ };
	IBaseHandle Entity;
	float		Scale{ 1.f };
	float		Magnitude{ };
	float		Radius{ };
	int			Attachement_Index{ };
	short		SurfaceProp{ };

	int			Matriel{ };
	int			DamageType{ };
	int			Hitbox{ };

	uint8_t		Color{ };

	bool						HasCustomColors{ };
	TFParticleEffectColor		CustomColors{ };

	bool						HasCustomPoint{ };
	TFParticleEffectPoint		CustomPoint{ };

	int			EffectName;
};



class ITFParticle
{
public:
	SG_SDK_TF2 void SetControlPoint(int point, const Vector3D_F& val);
	SG_SDK_TF2 void SetControlPoint(int point, const IBaseEntity pEnt);
};

class ITFParticleFactory
{
public:
	SG_SDK_TF2 static void DispatchParticleEffect(const IBaseEntity entity, const char* particle, const Vector3D_F& origins, const Angle_F& angles);

	SG_SDK_TF2 ITFParticle* Create(const char* particle_name, ParticleAttachment attach_type, int attach_point = 0, const Vector3D_F& attach_position = { }) noexcept;
	SG_SDK_TF2 void StopEmission(ITFParticle* pEffect = nullptr, bool bWakeOnStop = false, bool bDestroyAsleepSystems = false) noexcept;
};


class ITFParticleData
{
public:
	ITFParticleData(IBaseEntity pEnt, const char* particle_name, ParticleAttachment attach_type, int attach_pt = 0, const Vector3D_F& attach_offset = { }) noexcept :
		m_Owner(pEnt)
	{
		if (pEnt)
			m_Particle = pEnt->ParticleProp->Create(particle_name, attach_type, attach_pt, attach_offset);
	}

	~ITFParticleData() noexcept
	{
		if (m_Particle)
		{
			m_Owner->ParticleProp->StopEmission(m_Particle);
		}
	}

	IBaseEntity owner() const noexcept
	{
		return m_Owner;
	}

public:
	ITFParticleData(const ITFParticleData&) = delete;
	ITFParticleData& operator=(const ITFParticleData&) = delete;
	ITFParticleData(ITFParticleData&&) = default;
	ITFParticleData& operator=(ITFParticleData&&) = default;

private:
	ITFParticle* m_Particle{ };
	IBaseEntity m_Owner;
};


TF2_NAMESPACE_END();

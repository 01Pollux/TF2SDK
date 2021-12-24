#include <shadowgarden/interfaces/GameData.hpp>

#include <tf2/client/Effects.hpp>

#include <tf2/utils/Thunks.hpp>

TF2_NAMESPACE_BEGIN();

void ITFParticle::SetControlPoint(int point, const Vector3D_F& val)
{
	static Utils::IMemberFuncThunk<void, int, const Vector3D_F&> set_ctrpt{
		   Interfaces::SDKManager::Get()->ReadSignature({ "ITFParticle" }, "SetControlPoint").get()
	};
	assert(set_ctrpt);
	set_ctrpt(this, point, val);
}

void ITFParticle::SetControlPoint(int point, const IBaseEntity pEnt)
{
	static Utils::IMemberFuncThunk<void, int, const IBaseEntityInternal*> set_ctrpt{
		   Interfaces::SDKManager::Get()->ReadSignature({ "ITFParticle" }, "SetControlPointEntity").get()
	};
	assert(set_ctrpt);
	set_ctrpt(this, point, pEnt.get());
}


void ITFParticleFactory::DispatchParticleEffect(const IBaseEntity pEnt, const char* particle, const Vector3D_F& origins, const Angle_F& angles)
{
	static Utils::IFuncThunk<void, const IBaseEntityInternal*, const char*, Vector3D_F, Angle_F> dispatch_effect{
		   Interfaces::SDKManager::Get()->ReadSignature({ "ITFParticle" }, "DispatchParticleEffect").get()
	};
	assert(dispatch_effect);
	dispatch_effect(pEnt.get(), particle, origins, angles);
}

ITFParticle* ITFParticleFactory::Create(const char* particle_name, ParticleAttachment attach_type, int attach_point, const Vector3D_F& attach_position) noexcept
{
	static Utils::IMemberFuncThunk<ITFParticle*, const char*, ParticleAttachment, int, Vector3D_F> create_particle{
		   Interfaces::SDKManager::Get()->ReadSignature({ "ITFParticle" }, "FactoryCreate").get()
	};
	assert(create_particle);
	return create_particle(this, particle_name, attach_type, attach_point, attach_position);
}

void ITFParticleFactory::StopEmission(ITFParticle* pEffect, bool bWakeOnStop, bool bDestroyAsleepSystems) noexcept
{
	static Utils::IMemberFuncThunk<void, ITFParticle*, bool, bool> stop_particle{
		   Interfaces::SDKManager::Get()->ReadSignature({ "ITFParticle" }, "StopEmission").get()
	};
	assert(stop_particle);
	return stop_particle(this, pEffect, bWakeOnStop, bDestroyAsleepSystems);
}

TF2_NAMESPACE_END();

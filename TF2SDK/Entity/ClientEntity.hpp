#pragma once

#include "ClientEntity_Internal.hpp"

TF2_NAMESPACE_BEGIN();

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	// Delete yourself.
	virtual void			Release() = 0;

	// Network origin + angles
	virtual const Vector3D_F&	GetAbsOrigin() const = 0;
	virtual const Angle_F&	GetAbsAngles() const = 0;

	virtual CMouthInfo*		GetMouth() = 0;

	// Retrieve sound spatialization info for the specified sound on this entity
	// Return false to indicate sound is not audible
	virtual bool			GetSoundSpatialization(SpatializationInfo& info) = 0;
};

TF2_NAMESPACE_END();

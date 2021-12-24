#pragma once

#include "ClientEntity_Internal.hpp"

TF2_NAMESPACE_BEGIN();

class IClientEntity : public IClientUnknown, public IClientRenderable, public IClientNetworkable, public IClientThinkable
{
public:
	// Delete yourself.
	virtual void			Release() abstract;

	// Network origin + angles
	virtual const Vector3D_F&	GetAbsOrigin() const abstract;
	virtual const Angle_F&	GetAbsAngles() const abstract;

	virtual CMouthInfo*		GetMouth() abstract;

	// Retrieve sound spatialization info for the specified sound on this entity
	// Return false to indicate sound is not audible
	virtual bool			GetSoundSpatialization(SpatializationInfo& info) abstract;
};

TF2_NAMESPACE_END();

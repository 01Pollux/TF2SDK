#pragma once

#include "Math/Vector.hpp"

TF2_NAMESPACE_BEGIN();

class IPhysicsObject;

// NOTE: This is an iterator for the contact points on an object
// NOTE: This should only be used temporarily.  Holding one of these
// NOTE: across collision callbacks or calls into simulation will cause errors!
// NOTE: VPHYSICS may choose to make the data contained within this object invalid 
// NOTE: any time simulation is run.
class IPhysicsFrictionSnapshot
{
public:
	virtual ~IPhysicsFrictionSnapshot() = default;

	virtual bool IsValid() abstract;

	// Object 0 is this object, Object 1 is the other object
	virtual IPhysicsObject* GetObject(int index) abstract;
	virtual int GetMaterial(int index) abstract;

	virtual void GetContactPoint(Vector3D_F& out) abstract;

	// points away from source object
	virtual void GetSurfaceNormal(Vector3D_F& out) abstract;
	virtual float GetNormalForce() abstract;
	virtual float GetEnergyAbsorbed() abstract;

	// recompute friction (useful if dynamically altering materials/mass)
	virtual void RecomputeFriction() abstract;
	// clear all friction force at this contact point
	virtual void ClearFrictionForce() abstract;

	virtual void MarkContactForDelete() abstract;
	virtual void DeleteAllMarkedContacts(bool wakeObjects) abstract;

	// Move to the next friction data for this object
	virtual void NextFrictionData() abstract;
	virtual float GetFrictionCoefficient() abstract;
};

TF2_NAMESPACE_END();
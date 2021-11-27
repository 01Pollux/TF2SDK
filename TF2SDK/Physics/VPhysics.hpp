#pragma once

#include "Engine/AppSystem.hpp"
#include "Math/Matrix.hpp"
#include "Consts.hpp"

TF2_NAMESPACE_BEGIN();

class IPhysicsSurfaceProps;
class IPhysicsCollision;
class IVPhysics;

class IPhysicsEnvironment;
class IPhysicsObjectPairHash;
class IPhysicsCollisionSet;
class CPhysConvex;
class CPolyhedron;
class CPhysPolysoup;
class CPhysCollide;
class GameTrace;
class IConvexInfo;
class IVPhysicsKeyParser;
class ICollisionQuery;
class IPhysicsObject;
class IPhysicsFluidController;
class IPhysicsFrictionSnapshot;
class IPhysicsShadowController;

struct GameRay;
struct virtualmeshparams_t;
struct vcollide_t;


namespace Interfaces
{
	TF2_EXPORT_INTERFACE(IVPhysics, VPhysics, "VPhysics031");
	TF2_EXPORT_INTERFACE(IPhysicsCollision, PhysicsCollision, "VPhysicsCollision007");
	TF2_EXPORT_INTERFACE(IPhysicsSurfaceProps, PhysicsSurfProp, "VPhysicsSurfaceProps001");
}


class VIPhysics : public IAppSystem
{
public:
	virtual	IPhysicsEnvironment* CreateEnvironment() abstract;
	virtual void				 DestroyEnvironment(IPhysicsEnvironment*) abstract;
	virtual IPhysicsEnvironment* GetActiveEnvironmentByIndex(int index) abstract;

	// Creates a fast hash of pairs of objects
	// Useful for maintaining a table of object relationships like pairs that do not collide.
	virtual IPhysicsObjectPairHash* CreateObjectPairHash() abstract;
	virtual void					DestroyObjectPairHash(IPhysicsObjectPairHash* pHash) abstract;

	// holds a cache of these by id.  So you can get by id to search for the previously created set
	// UNDONE: Sets are currently limited to 32 elements.  More elements will return NULL in create.
	// NOTE: id is not allowed to be zero.
	virtual IPhysicsCollisionSet* FindOrCreateCollisionSet(unsigned int id, int maxElementCount) abstract;
	virtual IPhysicsCollisionSet* FindCollisionSet(unsigned int id) abstract;
	virtual void				  DestroyAllCollisionSets() abstract;
};

class IPhysicsCollisionData
{
public:
	virtual void GetSurfaceNormal(Vector3D_F& out) abstract;		// normal points toward second object (object index 1)
	virtual void GetContactPoint(Vector3D_F& out) abstract;		// contact point of collision (in world space)
	virtual void GetContactSpeed(Vector3D_F& out) abstract;		// speed of surface 1 relative to surface 0 (in world space)
};


class IPhysicsCollision
{
public:
	virtual ~IPhysicsCollision() = default;

	// produce a convex element from verts (convex hull around verts)
	virtual CPhysConvex* ConvexFromVerts(Vector3D_F** pVerts, int vertCount) abstract;
	// produce a convex element from planes (csg of planes)
	virtual CPhysConvex* ConvexFromPlanes(float* pPlanes, int planeCount, float mergeDistance) abstract;
	// calculate volume of a convex element
	virtual float			ConvexVolume(CPhysConvex* pConvex) abstract;

	virtual float			ConvexSurfaceArea(CPhysConvex* pConvex) abstract;
	// store game-specific data in a convex solid
	virtual void			SetConvexGameData(CPhysConvex* pConvex, unsigned int gameData) abstract;
	// If not converted, free the convex elements with this call
	virtual void			ConvexFree(CPhysConvex* pConvex) abstract;
	virtual CPhysConvex* BBoxToConvex(const Vector3D_F& mins, const Vector3D_F& maxs) abstract;
	// produce a convex element from a convex polyhedron
	virtual CPhysConvex* ConvexFromConvexPolyhedron(const CPolyhedron& ConvexPolyhedron) abstract;
	// produce a set of convex triangles from a convex polygon, normal is assumed to be on the side with forward point ordering, which should be clockwise, output will need to be able to hold exactly (iPointCount-2) convexes
	virtual void			ConvexesFromConvexPolygon(const Vector3D_F& vPolyNormal, const Vector3D_F* pPoints, int iPointCount, CPhysConvex** pOutput) abstract;

	// concave objects
	// create a triangle soup
	virtual CPhysPolysoup* PolysoupCreate() abstract;
	// destroy the container and memory
	virtual void			PolysoupDestroy(CPhysPolysoup* pSoup) abstract;
	// add a triangle to the soup
	virtual void			PolysoupAddTriangle(CPhysPolysoup* pSoup, const Vector3D_F& a, const Vector3D_F& b, const Vector3D_F& c, int materialIndex7bits) abstract;
	// convert the convex into a compiled collision model
	virtual CPhysCollide* ConvertPolysoupToCollide(CPhysPolysoup* pSoup, bool useMOPP) abstract;

	// Convert an array of convex elements to a compiled collision model (this deletes the convex elements)
	virtual CPhysCollide* ConvertConvexToCollide(CPhysConvex** pConvex, int convexCount) abstract;
	virtual CPhysCollide* ConvertConvexToCollideParams(CPhysConvex** pConvex, int convexCount, const convertconvexparams_t& convertParams) abstract;
	// Free a collide that was created with ConvertConvexToCollide()
	virtual void			DestroyCollide(CPhysCollide* pCollide) abstract;

	// Get the memory size in bytes of the collision model for serialization
	virtual int				CollideSize(CPhysCollide* pCollide) abstract;
	// serialize the collide to a block of memory
	virtual int				CollideWrite(char* pDest, CPhysCollide* pCollide, bool bSwap = false) abstract;
	// unserialize the collide from a block of memory
	virtual CPhysCollide* UnserializeCollide(char* pBuffer, int size, int index) abstract;

	// compute the volume of a collide
	virtual float			CollideVolume(CPhysCollide* pCollide) abstract;
	// compute surface area for tools
	virtual float			CollideSurfaceArea(CPhysCollide* pCollide) abstract;

	// Get the support map for a collide in the given direction
	virtual Vector3D_F			CollideGetExtent(const CPhysCollide* pCollide, const Vector3D_F& collideOrigin, const Angle_F& collideAngles, const Vector3D_F& direction) abstract;

	// Get an AABB for an oriented collision model
	virtual void			CollideGetAABB(Vector3D_F* pMins, Vector3D_F* pMaxs, const CPhysCollide* pCollide, const Vector3D_F& collideOrigin, const Angle_F& collideAngles) abstract;

	virtual void			CollideGetMassCenter(CPhysCollide* pCollide, Vector3D_F* pOutMassCenter) abstract;
	virtual void			CollideSetMassCenter(CPhysCollide* pCollide, const Vector3D_F& massCenter) abstract;
	// get the approximate cross-sectional area projected orthographically on the bbox of the collide
	// NOTE: These are fractional areas - unitless.  Basically this is the fraction of the OBB on each axis that
	// would be visible if the object were rendered orthographically.
	// NOTE: This has been precomputed when the collide was built or this function will return 1,1,1
	virtual Vector3D_F			CollideGetOrthographicAreas(const CPhysCollide* pCollide) abstract;
	virtual void			CollideSetOrthographicAreas(CPhysCollide* pCollide, const Vector3D_F& areas) abstract;

	// query the vcollide index in the physics model for the instance
	virtual int				CollideIndex(const CPhysCollide* pCollide) abstract;

	// Convert a bbox to a collide
	virtual CPhysCollide* BBoxToCollide(const Vector3D_F& mins, const Vector3D_F& maxs) abstract;
	virtual int				GetConvexesUsedInCollideable(const CPhysCollide* pCollideable, CPhysConvex** pOutputArray, int iOutputArrayLimit) abstract;


	// Trace an AABB against a collide
	virtual void TraceBox(const Vector3D_F& start,
		const Vector3D_F& end,
		const Vector3D_F& mins,
		const Vector3D_F& maxs,
		const CPhysCollide* pCollide,
		const Vector3D_F& collideOrigin,
		const Angle_F& collideAngles, GameTrace* ptr
	) abstract;

	virtual void TraceBox(const GameRay& ray,
		const CPhysCollide* pCollide,
		const Vector3D_F& collideOrigin,
		const Angle_F& collideAngles,
		GameTrace* ptr
	) abstract;

	virtual void TraceBox(const GameRay& ray,
		unsigned int contentsMask,
		IConvexInfo* pConvexInfo,
		const CPhysCollide* pCollide,
		const Vector3D_F& collideOrigin,
		const Angle_F& collideAngles,
		GameTrace* ptr
	) abstract;

	// Trace one collide against another
	virtual void TraceCollide(
		const Vector3D_F& start, 
		const Vector3D_F& end, 
		const CPhysCollide* pSweepCollide,
		const Angle_F& sweepAngles, 
		const CPhysCollide* pCollide, 
		const Vector3D_F& collideOrigin,
		const Angle_F& collideAngles, 
		GameTrace* ptr
	) abstract;

	// relatively slow test for box vs. truncated cone
	virtual bool			IsBoxIntersectingCone(const Vector3D_F& boxAbsMins, const Vector3D_F& boxAbsMaxs, const truncatedcone_t& cone) abstract;

	// loads a set of solids into a vcollide_t
	virtual void			VCollideLoad(vcollide_t* pOutput, int solidCount, const char* pBuffer, int size, bool swap = false) abstract;
	// destroyts the set of solids created by VCollideLoad
	virtual void			VCollideUnload(vcollide_t* pVCollide) abstract;

	// begins parsing a vcollide.  NOTE: This keeps pointers to the text
	// If you free the text and call members of IVPhysicsKeyParser, it will crash
	virtual IVPhysicsKeyParser* VPhysicsKeyParserCreate(const char* pKeyData) abstract;
	// Free the parser created by VPhysicsKeyParserCreate
	virtual void			VPhysicsKeyParserDestroy(IVPhysicsKeyParser* pParser) abstract;

	// creates a list of verts from a collision mesh
	virtual int				CreateDebugMesh(CPhysCollide const* pCollisionModel, Vector3D_F** outVerts) abstract;
	// destroy the list of verts created by CreateDebugMesh
	virtual void			DestroyDebugMesh(int vertCount, Vector3D_F* outVerts) abstract;

	// create a queryable version of the collision model
	virtual ICollisionQuery* CreateQueryModel(CPhysCollide* pCollide) abstract;
	// destroy the queryable version
	virtual void			DestroyQueryModel(ICollisionQuery* pQuery) abstract;

	virtual IPhysicsCollision* ThreadContextCreate() abstract;
	virtual void			ThreadContextDestroy(IPhysicsCollision* pThreadContex) abstract;

	virtual CPhysCollide* CreateVirtualMesh(const virtualmeshparams_t& params) abstract;
	virtual bool			SupportsVirtualMesh() abstract;


	virtual bool			GetBBoxCacheSize(int* pCachedSize, int* pCachedCount) abstract;


	// extracts a polyhedron that defines a CPhysConvex's shape
	virtual CPolyhedron* PolyhedronFromConvex(CPhysConvex* const pConvex, bool bUseTempPolyhedron) abstract;

	// dumps info about the collide to Msg()
	virtual void			OutputDebugInfo(const CPhysCollide* pCollide) abstract;
	virtual unsigned int	ReadStat(int statID) abstract;
};


class IPhysicsCollisionEvent
{
public:
	// returns the two objects that collided, time between last collision of these objects
	// and an opaque data block of collision information
	// NOTE: PreCollision/PostCollision ALWAYS come in matched pairs!!!
	virtual void PreCollision(vcollisionevent_t* pEvent) abstract;
	virtual void PostCollision(vcollisionevent_t* pEvent) abstract;

	// This is a scrape event.  The object has scraped across another object consuming the indicated energy
	virtual void Friction(IPhysicsObject* pObject, float energy, int surfaceProps, int surfacePropsHit, IPhysicsCollisionData* pData) abstract;

	virtual void StartTouch(IPhysicsObject* pObject1, IPhysicsObject* pObject2, IPhysicsCollisionData* pTouchData) abstract;
	virtual void EndTouch(IPhysicsObject* pObject1, IPhysicsObject* pObject2, IPhysicsCollisionData* pTouchData) abstract;

	virtual void FluidStartTouch(IPhysicsObject* pObject, IPhysicsFluidController* pFluid) abstract;
	virtual void FluidEndTouch(IPhysicsObject* pObject, IPhysicsFluidController* pFluid) abstract;

	virtual void PostSimulationFrame() abstract;

	virtual void ObjectEnterTrigger(IPhysicsObject* pTrigger, IPhysicsObject* pObject) {}
	virtual void ObjectLeaveTrigger(IPhysicsObject* pTrigger, IPhysicsObject* pObject) {}
};


class IPhysicsSurfaceProps
{
public:
	virtual ~IPhysicsSurfaceProps() {}

	// parses a text file containing surface prop keys
	virtual int		ParseSurfaceData(const char* pFilename, const char* pTextfile) abstract;
	// current number of entries in the database
	virtual int		SurfacePropCount() const abstract;

	virtual int		GetSurfaceIndex(const char* pSurfacePropName) const abstract;
	virtual void	GetPhysicsProperties(int surfaceDataIndex, float* density, float* thickness, float* friction, float* elasticity) const abstract;

	virtual surfacedata_t* GetSurfaceData(int surfaceDataIndex) abstract;
	virtual const char* GetString(unsigned short stringTableIndex) const abstract;


	virtual const char* GetPropName(int surfaceDataIndex) const abstract;

	// sets the global index table for world materials
	// UNDONE: Make this per-CPhysCollide
	virtual void	SetWorldMaterialIndexTable(int* pMapArray, int mapSize) abstract;

	// NOTE: Same as GetPhysicsProperties, but maybe more convenient
	virtual void	GetPhysicsParameters(int surfaceDataIndex, surfacephysicsparams_t* pParamsOut) const abstract;
};


class IPhysicsObject
{
public:
	virtual ~IPhysicsObject() {}

	// returns true if this object is static/unmoveable
	// NOTE: returns false for objects that are not created static, but set EnableMotion(false);
	// Call IsMoveable() to find if the object is static OR has motion disabled
	virtual bool			IsStatic() const abstract;
	virtual bool			IsAsleep() const abstract;
	virtual bool			IsTrigger() const abstract;
	virtual bool			IsFluid() const abstract;		// fluids are special triggers with fluid controllers attached, they return true to IsTrigger() as well!
	virtual bool			IsHinged() const abstract;
	virtual bool			IsCollisionEnabled() const abstract;
	virtual bool			IsGravityEnabled() const abstract;
	virtual bool			IsDragEnabled() const abstract;
	virtual bool			IsMotionEnabled() const abstract;
	virtual bool			IsMoveable() const abstract;	 // legacy: IsMotionEnabled() && !IsStatic()
	virtual bool			IsAttachedToConstraint(bool bExternalOnly) const abstract;

	// Enable / disable collisions for this object
	virtual void			EnableCollisions(bool enable) abstract;
	// Enable / disable gravity for this object
	virtual void			EnableGravity(bool enable) abstract;
	// Enable / disable air friction / drag for this object
	virtual void			EnableDrag(bool enable) abstract;
	// Enable / disable motion (pin / unpin the object)
	virtual void			EnableMotion(bool enable) abstract;

	// Game can store data in each object (link back to game object)
	virtual void			SetGameData(void* pGameData) abstract;
	virtual void* GetGameData() const abstract;
	// This flags word can be defined by the game as well
	virtual void			SetGameFlags(unsigned short userFlags) abstract;
	virtual unsigned short	GetGameFlags() const abstract;
	virtual void			SetGameIndex(unsigned short gameIndex) abstract;
	virtual unsigned short	GetGameIndex() const abstract;

	// setup various callbacks for this object
	virtual void			SetCallbackFlags(unsigned short callbackflags) abstract;
	// get the current callback state for this object
	virtual unsigned short	GetCallbackFlags() const abstract;

	// "wakes up" an object
	// NOTE: ALL OBJECTS ARE "Asleep" WHEN CREATED
	virtual void			Wake() abstract;
	virtual void			Sleep() abstract;
	// call this when the collision filter conditions change due to this 
	// object's state (e.g. changing solid type or collision group)
	virtual void			RecheckCollisionFilter() abstract;
	// NOTE: Contact points aren't updated when collision rules change, call this to force an update
	// UNDONE: Force this in RecheckCollisionFilter() ?
	virtual void			RecheckContactPoints() abstract;

	// mass accessors
	virtual void			SetMass(float mass) abstract;
	virtual float			GetMass() const abstract;
	// get 1/mass (it's cached)
	virtual float			GetInvMass() const abstract;
	virtual Vector3D_F		GetInertia() const abstract;
	virtual Vector3D_F		GetInvInertia() const abstract;
	virtual void			SetInertia(const Vector3D_F& inertia) abstract;

	virtual void			SetDamping(const float* speed, const float* rot) abstract;
	virtual void			GetDamping(float* speed, float* rot) const abstract;

	// coefficients are optional, pass either
	virtual void			SetDragCoefficient(float* pDrag, float* pAngularDrag) abstract;
	virtual void			SetBuoyancyRatio(float ratio) abstract;			// Override bouyancy

	// material index
	virtual int				GetMaterialIndex() const abstract;
	virtual void			SetMaterialIndex(int materialIndex) abstract;

	// contents bits
	virtual unsigned int	GetContents() const abstract;
	virtual void			SetContents(unsigned int contents) abstract;

	// Get the radius if this is a sphere object (zero if this is a polygonal mesh)
	virtual float			GetSphereRadius() const abstract;
	virtual float			GetEnergy() const abstract;
	virtual Vector3D_F		GetMassCenterLocalSpace() const abstract;

	// NOTE: This will teleport the object
	virtual void			SetPosition(const Vector3D_F& worldPosition, const Angle_F& angles, bool isTeleport) abstract;
	virtual void			SetPositionMatrix(const Matrix3x4_F& matrix, bool isTeleport) abstract;

	virtual void			GetPosition(Vector3D_F* worldPosition, Angle_F* angles) const abstract;
	virtual void			GetPositionMatrix(Matrix3x4_F* positionMatrix) const abstract;
	// force the velocity to a new value
	// NOTE: velocity is in worldspace, angularVelocity is relative to the object's 
	// local axes (just like pev->velocity, pev->avelocity)
	virtual void			SetVelocity(const Vector3D_F* velocity, const Vector3D_F* angularVelocity) abstract;

	// like the above, but force the change into the simulator immediately
	virtual void			SetVelocityInstantaneous(const Vector3D_F* velocity, const Vector3D_F* angularVelocity) abstract;

	// NOTE: velocity is in worldspace, angularVelocity is relative to the object's 
	// local axes (just like pev->velocity, pev->avelocity)
	virtual void			GetVelocity(Vector3D_F* velocity, Vector3D_F* angularVelocity) const abstract;

	// NOTE: These are velocities, not forces.  i.e. They will have the same effect regardless of
	// the object's mass or inertia
	virtual void			AddVelocity(const Vector3D_F* velocity, const Vector3D_F* angularVelocity) abstract;
	// gets a velocity in the object's local frame of reference at a specific point
	virtual void			GetVelocityAtPoint(const Vector3D_F& worldPosition, Vector3D_F* pVelocity) const abstract;
	// gets the velocity actually moved by the object in the last simulation update
	virtual void			GetImplicitVelocity(Vector3D_F* velocity, Vector3D_F* angularVelocity) const abstract;
	// NOTE:	These are here for convenience, but you can do them yourself by using the matrix
	//			returned from GetPositionMatrix()
	// convenient coordinate system transformations (params - dest, src)
	virtual void			LocalToWorld(Vector3D_F* worldPosition, const Vector3D_F& localPosition) const abstract;
	virtual void			WorldToLocal(Vector3D_F* localPosition, const Vector3D_F& worldPosition) const abstract;

	// transforms a vector (no translation) from object-local to world space
	virtual void			LocalToWorldVector(Vector3D_F* worldVector, const Vector3D_F& localVector) const abstract;
	// transforms a vector (no translation) from world to object-local space
	virtual void			WorldToLocalVector(Vector3D_F* localVector, const Vector3D_F& worldVector) const abstract;

	// push on an object
	// force vector is direction & magnitude of impulse kg in / s
	virtual void			ApplyForceCenter(const Vector3D_F& forceVector) abstract;
	virtual void			ApplyForceOffset(const Vector3D_F& forceVector, const Vector3D_F& worldPosition) abstract;
	// apply torque impulse.  This will change the angular velocity on the object.
	// HL Axes, kg degrees / s
	virtual void			ApplyTorqueCenter(const Vector3D_F& torque) abstract;

	// Calculates the force/torque on the center of mass for an offset force impulse (pass output to ApplyForceCenter / ApplyTorqueCenter)
	virtual void			CalculateForceOffset(const Vector3D_F& forceVector, const Vector3D_F& worldPosition, Vector3D_F* centerForce, Vector3D_F* centerTorque) const abstract;
	// Calculates the linear/angular velocities on the center of mass for an offset force impulse (pass output to AddVelocity)
	virtual void			CalculateVelocityOffset(const Vector3D_F& forceVector, const Vector3D_F& worldPosition, Vector3D_F* centerVelocity, Vector3D_F* centerAngularVelocity) const abstract;
	// calculate drag scale
	virtual float			CalculateLinearDrag(const Vector3D_F& unitDirection) const abstract;
	virtual float			CalculateAngularDrag(const Vector3D_F& objectSpaceRotationAxis) const abstract;

	// returns true if the object is in contact with another object
	// if true, puts a point on the contact surface in contactPoint, and
	// a pointer to the object in contactObject
	// NOTE: You can pass NULL for either to avoid computations
	// BUGBUG: Use CreateFrictionSnapshot instead of this - this is a simple hack
	virtual bool			GetContactPoint(Vector3D_F* contactPoint, IPhysicsObject** contactObject) const abstract;

	// refactor this a bit - move some of this to IPhysicsShadowController
	virtual void			SetShadow(float maxSpeed, float maxAngularSpeed, bool allowPhysicsMovement, bool allowPhysicsRotation) abstract;
	virtual void			UpdateShadow(const Vector3D_F& targetPosition, const Angle_F& targetAngles, bool tempDisableGravity, float timeOffset) abstract;

	// returns number of ticks since last Update() call
	virtual int				GetShadowPosition(Vector3D_F* position, Angle_F* angles) const abstract;
	virtual IPhysicsShadowController* GetShadowController() const abstract;
	virtual void			RemoveShadowController() abstract;
	// applies the math of the shadow controller to this object.
	// for use in your own controllers
	// returns the new value of secondsToArrival with dt time elapsed
	virtual float			ComputeShadowControl(const hlshadowcontrol_params_t& params, float secondsToArrival, float dt) abstract;


	virtual const CPhysCollide* GetCollide() const abstract;
	virtual const char*			GetName() const abstract;

	virtual void			BecomeTrigger() abstract;
	virtual void			RemoveTrigger() abstract;

	// sets the object to be hinged.  Fixed it place, but able to rotate around one axis.
	virtual void			BecomeHinged(int localAxis) abstract;
	// resets the object to original state
	virtual void			RemoveHinged() abstract;

	// used to iterate the contact points of an object
	virtual IPhysicsFrictionSnapshot* CreateFrictionSnapshot() abstract;
	virtual void DestroyFrictionSnapshot(IPhysicsFrictionSnapshot* pSnapshot) abstract;

	// dumps info about the object to Msg()
	virtual void			OutputDebugInfo() const abstract;

};

class IPhysicsShadowController
{
public:
	virtual ~IPhysicsShadowController() {}

	virtual void Update(const Vector3D_F& position, const Angle_F& angles, float timeOffset) abstract;
	virtual void MaxSpeed(float maxSpeed, float maxAngularSpeed) abstract;
	virtual void StepUp(float height) abstract;

	// If the teleport distance is non-zero, the object will be teleported to 
	// the target location when the error exceeds this quantity.
	virtual void SetTeleportDistance(float teleportDistance) abstract;
	virtual bool AllowsTranslation() abstract;
	virtual bool AllowsRotation() abstract;

	// There are two classes of shadow objects:
	// 1) Game physics controlled, shadow follows game physics (this is the default)
	// 2) Physically controlled - shadow position is a target, but the game hasn't guaranteed that the space can be occupied by this object
	virtual void SetPhysicallyControlled(bool isPhysicallyControlled) abstract;
	virtual bool IsPhysicallyControlled() abstract;
	virtual void GetLastImpulse(Vector3D_F* pOut) abstract;
	virtual void UseShadowMaterial(bool bUseShadowMaterial) abstract;
	virtual void ObjectMaterialChanged(int materialIndex) abstract;


	//Basically get the last inputs to IPhysicsShadowController::Update(), returns last input to timeOffset in Update()
	virtual float GetTargetPosition(Vector3D_F* pPositionOut, Vector3D_F* pAnglesOut) abstract;

	virtual float GetTeleportDistance() abstract;
	virtual void GetMaxSpeed(float* pMaxSpeedOut, float* pMaxAngularSpeedOut) abstract;
};



TF2_NAMESPACE_END();
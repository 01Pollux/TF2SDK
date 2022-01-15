#pragma once

#include <tf2/math/Vector.hpp>
#include <tf2/math/Matrix.hpp>
#include <tf2/consts.hpp>

TF2_NAMESPACE_BEGIN();

class IBaseHandle;
class ICollideable;
class IClientNetworkable;
class IClientRenderable;
class IClientEntity;
class IBaseEntityInternal;
class IClientThinkable;
class IPVSNotify;
class ClientClass;
class bf_read;
class CMouthInfo;
struct SpatializationInfo;
struct ModelInfo;
struct GameRay;
class GameTrace;


class IHandleEntity
{
public:
	virtual ~IHandleEntity() = default;
	virtual void SetRefEHandle(const IBaseHandle& handle) abstract;
	[[nodiscard]] virtual const IBaseHandle& GetRefEHandle() const abstract;
};

class IClientUnknown : public IHandleEntity
{
public:
	[[nodiscard]] virtual ICollideable* GetCollideable() abstract;
	[[nodiscard]] virtual IClientNetworkable* GetClientNetworkable() abstract;
	[[nodiscard]] virtual IClientRenderable* GetClientRenderable() const abstract;
	[[nodiscard]] virtual IClientEntity* GetIClientEntity() abstract;
	[[nodiscard]] virtual IBaseEntityInternal* GetBaseEntity() abstract;
	[[nodiscard]] virtual IClientThinkable* GetClientThinkable() const abstract;
};

class ICollideable
{
public:
	[[nodiscard]] virtual IHandleEntity* GetEntityHandle() abstract;

	// These methods return the bounds of an OBB measured in "collision" space
	// which can be retreived through the CollisionToWorldTransform or
	// GetCollisionOrigin/GetCollisionAngles methods
	[[nodiscard]] virtual const Vector3D_F& OBBMinsPreScaled() const abstract;
	[[nodiscard]] virtual const Vector3D_F& OBBMaxsPreScaled() const abstract;
	[[nodiscard]] virtual const Vector3D_F& OBBMins() const abstract;
	[[nodiscard]] virtual const Vector3D_F& OBBMaxs() const abstract;

	// Returns the bounds of a world-space box used when the collideable is being traced
	// against as a trigger. It's only valid to call these methods if the solid flags
	// have the FSOLID_USE_TRIGGER_BOUNDS flag set.
	virtual void			WorldSpaceTriggerBounds(Vector3D_F* pVecWorldMins, Vector3D_F* pVecWorldMaxs) const abstract;

	// custom collision test
	[[nodiscard]] virtual bool TestCollision(const GameRay& ray, unsigned int fContentsMask, GameTrace& tr) abstract;

	// Perform hitbox test, returns true *if hitboxes were tested at all*!!
	[[nodiscard]] virtual bool TestHitboxes(const GameRay& ray, unsigned int fContentsMask, GameTrace& tr) abstract;

	// Returns the BRUSH model index if this is a brush model. Otherwise, returns -1.
	[[nodiscard]] virtual int GetCollisionModelIndex() abstract;

	// Return the model, if it's a studio model.
	[[nodiscard]] virtual const ModelInfo* GetCollisionModel() abstract;

	// Get angles and origin.
	[[nodiscard]] virtual const Vector3D_F& GetCollisionOrigin() const abstract;
	[[nodiscard]] virtual const Vector3D_F& GetCollisionAngles() const abstract;
	[[nodiscard]] virtual const Matrix3x4_F& CollisionToWorldTransform() const abstract;

	// Return a SOLID_ define.
	[[nodiscard]] virtual Const::EntSolidType GetSolid() const abstract;
	[[nodiscard]] virtual uint32_t GetSolidFlags() const abstract;

	// Gets at the containing class...
	[[nodiscard]] virtual IClientUnknown* GetIClientUnknown() abstract;

	// We can filter out collisions based on collision group
	[[nodiscard]] virtual Const::EntCollisionGroup GetCollisionGroup() const abstract;

	// Returns a world-aligned box guaranteed to surround *everything* in the collision representation
	// Note that this will surround hitboxes, trigger bounds, physics.
	// It may or may not be a tight-fitting box and its volume may suddenly change
	virtual void WorldSpaceSurroundingBounds(Vector3D_F* pVecMins, Vector3D_F* pVecMaxs) abstract;

	[[nodiscard]] virtual bool ShouldTouchTrigger(int triggerSolidFlags) const abstract;

	// returns NULL unless this collideable has specified FSOLID_ROOT_PARENT_ALIGNED
	[[nodiscard]] virtual const Matrix3x4_F* GetRootParentToWorldTransform() const abstract;
};

class IClientRenderable
{
public:
	using ClientShadowHandle = unsigned short;
	using ClientRenderHandle = unsigned short;
	using ModelInstanceHandle = unsigned short;

	[[nodiscard]] static constexpr ClientRenderHandle BadHandle()
	{
		return 0xFFFF;
	}

	// Gets at the containing class...
	[[nodiscard]] virtual IClientUnknown* GetIClientUnknown() abstract;

	// Data accessors
	[[nodiscard]] virtual const Vector3D_F&	GetRenderOrigin() abstract;
	[[nodiscard]] virtual const Angle_F&		GetRenderAngles() abstract;
	[[nodiscard]] virtual bool ShouldDraw() abstract;
	[[nodiscard]] virtual bool IsTransparent() abstract;
	[[nodiscard]] virtual bool UsesPowerOfTwoFrameBufferTexture() abstract;
	[[nodiscard]] virtual bool UsesFullFrameBufferTexture() abstract;
	[[nodiscard]] virtual ClientShadowHandle GetShadowHandle() const abstract;

	// Used by the leaf system to store its render handle.
	[[nodiscard]] virtual ClientRenderHandle& RenderHandle() abstract;

	// Render baby!
	[[nodiscard]] virtual const ModelInfo* GetModel() const abstract;
	[[nodiscard]] virtual int DrawModel(int studio_draw_flags) abstract;

	// Get the body parameter
	[[nodiscard]] virtual int GetBody() abstract;

	// Determine alpha and blend amount for transparent objects based on render state info
	[[nodiscard]] virtual void ComputeFxBlend() abstract;
	[[nodiscard]] virtual int GetFxBlend() abstract;

	// Determine the color modulation amount
	[[nodiscard]] virtual void GetColorModulation(float* color) abstract;

	// Returns false if the entity shouldn't be drawn due to LOD. 
	// (NOTE: This is no longer used/supported, but kept in the vtable for backwards compat)
	[[nodiscard]] virtual bool LODTest() abstract;

	// Call this to get the current bone transforms for the model.
	// currentTime parameter will affect interpolation
	// nMaxBones specifies how many matrices pBoneToWorldOut can hold. (Should be greater than or
	// equal to studiohdr_t::numbones. Use MAXSTUDIOBONES to be safe.)
	[[nodiscard]] virtual bool SetupBones(Matrix3x4_F* pBoneToWorldOut, int nMaxBones, int boneMask, float currentTime) abstract;

	virtual void	SetupWeights(const Matrix3x4_F* pBoneToWorld, int nFlexWeightCount, float* pFlexWeights, float* pFlexDelayedWeights) abstract;
	virtual void	DoAnimationEvents() abstract;

	// Return this if you want PVS notifications. See IPVSNotify for more info.	
	// Note: you must always return the same value from this function. If you don't,
	// undefined things will occur, and they won't be good.
	[[nodiscard]] virtual IPVSNotify* GetPVSNotifyInterface() abstract;

	// Returns the bounds relative to the origin (render bounds)
	virtual void	GetRenderBounds(Vector3D_F& mins, Vector3D_F& maxs) abstract;

	// returns the bounds as an AABB in worldspace
	virtual void	GetRenderBoundsWorldspace(Vector3D_F& mins, Vector3D_F& maxs) abstract;

	// These normally call through to GetRenderAngles/GetRenderBounds, but some entities custom implement them.
	virtual void	GetShadowRenderBounds(Vector3D_F& mins, Vector3D_F& maxs, int shadowType) abstract;

	// Should this object be able to have shadows cast onto it?
	[[nodiscard]] virtual bool ShouldReceiveProjectedTextures(int flags) abstract;

	// These methods return true if we want a per-renderable shadow cast direction + distance
	[[nodiscard]] virtual bool GetShadowCastDistance(float* pDist, int shadowType) const abstract;
	[[nodiscard]] virtual bool GetShadowCastDirection(Vector3D_F* pDirection, int shadowType) const abstract;

	// Other methods related to shadow rendering
	[[nodiscard]] virtual bool	IsShadowDirty() abstract;
	virtual void	MarkShadowDirty(bool bDirty) abstract;

	// Iteration over shadow hierarchy
	[[nodiscard]] virtual IClientRenderable* GetShadowParent() abstract;
	[[nodiscard]] virtual IClientRenderable* FirstShadowChild() abstract;
	[[nodiscard]] virtual IClientRenderable* NextShadowPeer() abstract;

	// Returns the shadow cast type
	[[nodiscard]] virtual int ShadowCastType() abstract;

	// Create/get/destroy model instance
	virtual void CreateModelInstance() abstract;
	[[nodiscard]] virtual ModelInstanceHandle GetModelInstance() abstract;

	// Returns the transform from RenderOrigin/RenderAngles to world
	[[nodiscard]] virtual const Matrix3x4_F& RenderableToWorldTransform() abstract;

	// Attachments
	[[nodiscard]] virtual int LookupAttachment(const char* pAttachmentName) abstract;
	[[nodiscard]] virtual bool GetAttachment(int number, Vector3D_F& origin, Angle_F& angles) abstract;
	[[nodiscard]] virtual bool GetAttachment(int number, Matrix3x4_F& matrix) abstract;

	// Rendering clip plane, should be 4 floats, return value of NULL indicates a disabled render clip plane
	[[nodiscard]] virtual float* GetRenderClipPlane() abstract;

	// Get the skin parameter
	[[nodiscard]] virtual int GetSkin() abstract;

	// Is this a two-pass renderable?
	[[nodiscard]] virtual bool IsTwoPass() abstract;

	virtual void OnThreadedDrawSetup() abstract;

	[[nodiscard]] virtual bool UsesFlexDelayedWeights() abstract;

	virtual void RecordToolMessage() abstract;

	[[nodiscard]] virtual bool IgnoresZBuffer() const abstract;
};



class IClientNetworkable
{
public:
	enum class ShouldTransmitState
	{
		Start,	// The entity is starting to be transmitted (maybe it entered the PVS).
		End		// Called when the entity isn't being transmitted by the server.
				// This signals a good time to hide the entity until next time
				// the server wants to transmit its state.
	};

	enum class DataUpdateType
	{
		Created = 0,	// indicates it was created +and+ entered the pvs
		DTChange,
	};

	// Gets at the containing class...
	[[nodiscard]] virtual IClientUnknown* GetIClientUnknown() abstract;

	// Called by the engine when the server deletes the entity.
	virtual void			Release() abstract;

	// Supplied automatically by the IMPLEMENT_CLIENTCLASS macros.
	[[nodiscard]] virtual ClientClass* GetClientClass() abstract;
	[[nodiscard]] const ClientClass* GetClientClass() const
	{
		return const_cast<IClientNetworkable*>(this)->GetClientClass();
	}

	// This tells the entity what the server says for ShouldTransmit on this entity.
	// Note: This used to be EntityEnteredPVS/EntityRemainedInPVS/EntityLeftPVS.
	virtual void			NotifyShouldTransmit(ShouldTransmitState state) abstract;


	//
	// NOTE FOR ENTITY WRITERS: 
	//
	// In 90% of the cases, you should hook OnPreDataChanged/OnDataChanged instead of 
	// PreDataUpdate/PostDataUpdate.
	//
	// The DataChanged events are only called once per frame whereas Pre/PostDataUpdate
	// are called once per packet (and sometimes multiple times per frame).
	//
	// OnDataChanged is called during simulation where entity origins are correct and 
	// attachments can be used. whereas PostDataUpdate is called while parsing packets
	// so attachments and other entity origins may not be valid yet.
	//

	virtual void			OnPreDataChanged(DataUpdateType updateType) abstract;
	virtual void			OnDataChanged(DataUpdateType updateType) abstract;

	// Called when data is being updated across the network.
	// Only low-level entities should need to know about these.
	virtual void			PreDataUpdate(DataUpdateType updateType) abstract;
	virtual void			PostDataUpdate(DataUpdateType updateType) abstract;


	// Objects become dormant on the client if they leave the PVS on the server.
	[[nodiscard]] virtual bool IsDormant() abstract;

	// Ent Index is the server handle used to reference this entity.
	// If the index is < 0, that indicates the entity is not known to the server
	[[nodiscard]] virtual int entindex() const abstract;

	// Server to client entity message received
	virtual void			ReceiveMessage(int classID, bf_read& msg) abstract;

	// Get the base pointer to the networked data that GetClientClass->m_pRecvTable starts at.
	// (This is usually just the "this" pointer).
	[[nodiscard]] virtual void* GetDataTableBasePtr() abstract;

	// Tells the entity that it's about to be destroyed due to the client receiving
	// an uncompressed update that's caused it to destroy all entities & recreate them.
	virtual void			SetDestroyedOnRecreateEntities(void) abstract;

	virtual void			OnDataUnchangedInPVS() abstract;
};

class IClientThinkable
{
public:
	class IClientThinkHandle;

	// Gets at the containing class...
	[[nodiscard]] virtual IClientUnknown* GetIClientUnknown() abstract;

	virtual void				ClientThink() abstract;

	// Called when you're added to the think list.
	// GetThinkHandle's return value must be initialized to INVALID_THINK_HANDLE.
	[[nodiscard]] virtual IClientThinkHandle* GetThinkHandle() abstract;
	virtual void				SetThinkHandle(IClientThinkHandle* hThink) abstract;

	// Called by the client when it deletes the entity.
	virtual void				Release() abstract;
};

TF2_NAMESPACE_END();

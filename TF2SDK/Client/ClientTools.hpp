#pragma once

#include "Math/Vector.hpp"
#include "Entity/ClientEntity.hpp"
#include "Engine/AppSystem.hpp"

TF2_NAMESPACE_BEGIN();

class IBaseEntityInternal;
class IClientRenderable;
class IClientTools;
class KeyValues;

struct FlashlightState_t;

namespace Interfaces
{
	TF2_EXPORT_INTERFACE(IClientTools, ClientTools, "VCLIENTTOOLS001");
}

namespace Const
{
	namespace ClientShadowFlags
	{
		constexpr uint32_t UserRender_ToTexture = 2 << 1;
		constexpr uint32_t Animating_Source = 2 << 2;
		constexpr uint32_t UseDepth_Texture = 2 << 3;
	}
}

class IClientTools : public IBaseInterface
{
public:
	using HTOOLHANDLE = uint32_t;
	using EntitySearchResult = void*;

	// Allocates or returns the handle to an entity previously found using the Find* APIs below
	virtual HTOOLHANDLE		AttachToEntity(EntitySearchResult entityToAttach) abstract;
	virtual void			DetachFromEntity(EntitySearchResult entityToDetach) abstract;

	// Checks whether a handle is still valid.
	virtual bool			IsValidHandle(HTOOLHANDLE handle) abstract;

	// Iterates the list of entities which have been associated with tools
	virtual int				GetNumRecordables() abstract;
	virtual HTOOLHANDLE		GetRecordable(int index) abstract;

	// Iterates through ALL entities (separate list for client vs. server)
	virtual IBaseEntityInternal*NextEntity(IBaseEntityInternal* currentEnt) abstract;
	IBaseEntityInternal*		FirstEntity() { return NextEntity(nullptr); }

	// Use this to turn on/off the presence of an underlying game entity
	virtual void			SetEnabled(HTOOLHANDLE handle, bool enabled) abstract;
	// Use this to tell an entity to post "state" to all listening tools
	virtual void			SetRecording(HTOOLHANDLE handle, bool recording) abstract;
	// Some entities are marked with ShouldRecordInTools false, such as ui entities, etc.
	virtual bool			ShouldRecord(HTOOLHANDLE handle) abstract;

	virtual HTOOLHANDLE		GetToolHandleForEntityByIndex(int entindex) abstract;

	virtual int				GetModelIndex(HTOOLHANDLE handle) abstract;
	virtual const char* GetModelName(HTOOLHANDLE handle) abstract;
	virtual const char* GetClassname(HTOOLHANDLE handle) abstract;

	virtual void			AddClientRenderable(IClientRenderable* pRenderable, int renderGroup) abstract;
	virtual void			RemoveClientRenderable(IClientRenderable* pRenderable) abstract;
	virtual void			SetRenderGroup(IClientRenderable* pRenderable, int renderGroup) abstract;
	virtual void			MarkClientRenderableDirty(IClientRenderable* pRenderable) abstract;
	virtual void			UpdateProjectedTexture(uint32_t client_shadowflags, bool bForce) abstract;

	virtual bool			DrawSprite(IClientRenderable* pRenderable, float scale, float frame, int rendermode, int renderfx, const Color4_I8& color, float flProxyRadius, int* pVisHandle) abstract;

	virtual EntitySearchResult	GetLocalPlayer() abstract;
	virtual bool			GetLocalPlayerEyePosition(Vector3D_F& org, Angle_F& ang, float& fov) abstract;

	// See ClientShadowFlags_t above
	virtual IClientRenderable::ClientShadowHandle CreateShadow(IBaseHandle handle, int nFlags) abstract;
	virtual void			DestroyShadow(IClientRenderable::ClientShadowHandle h) abstract;

	virtual IClientRenderable::ClientShadowHandle CreateFlashlight(const FlashlightState_t& lightState) abstract;
	virtual void			DestroyFlashlight(IClientRenderable::ClientShadowHandle h) abstract;
	virtual void			UpdateFlashlightState(IClientRenderable::ClientShadowHandle h, const FlashlightState_t& lightState) abstract;

	virtual void			AddToDirtyShadowList(IClientRenderable::ClientShadowHandle h, bool force = false) abstract;
	virtual void			MarkRenderToTextureShadowDirty(IClientRenderable::ClientShadowHandle h) abstract;

	// Global toggle for recording
	virtual void			EnableRecordingMode(bool bEnable) abstract;
	virtual bool			IsInRecordingMode() const abstract;

	// Trigger a temp entity
	virtual void			TriggerTempEntity(KeyValues* pKeyValues) abstract;

	// get owning weapon (for viewmodels)
	virtual int				GetOwningWeaponEntIndex(int entindex) abstract;
	virtual int				GetEntIndex(EntitySearchResult entityToAttach) abstract;

	virtual int				FindGlobalFlexcontroller(char const* name) abstract;
	virtual char const* GetGlobalFlexControllerName(int idx) abstract;

	// helper for traversing ownership hierarchy
	virtual EntitySearchResult	GetOwnerEntity(EntitySearchResult currentEnt) abstract;

	// common and useful types to query for hierarchically
	virtual bool			IsPlayer(EntitySearchResult currentEnt) abstract;
	virtual bool			IsBaseCombatCharacter(EntitySearchResult currentEnt) abstract;
	virtual bool			IsNPC(EntitySearchResult currentEnt) abstract;

	virtual Vector3D_F		GetAbsOrigin(HTOOLHANDLE handle) abstract;
	virtual Angle_F			GetAbsAngles(HTOOLHANDLE handle) abstract;

	// This reloads a portion or all of a particle definition file.
	// It's up to the client to decide if it cares about this file
	// Use a UtlBuffer to crack the data
	virtual void			ReloadParticleDefintions(const char* pFileName, const void* pBufData, int nLen) abstract;

	// Sends a mesage from the tool to the client
	virtual void			PostToolMessage(KeyValues* pKeyValues) abstract;

	// Indicates whether the client should render particle systems
	virtual void			EnableParticleSystems(bool bEnable) abstract;

	// Is the game rendering in 3rd person mode?
	virtual bool			IsRenderingThirdPerson() const abstract;
};

TF2_NAMESPACE_END();

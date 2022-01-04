#pragma once

#include <tf2/math/Vector.hpp>
#include <tf2/math/Matrix.hpp>
#include <tf2/consts.hpp>

TF2_NAMESPACE_BEGIN();


class IVModelInfo;


namespace interfaces
{
	TF2_EXPORT_INTERFACE(IVModelInfo, ModelInfo, "VModelInfoClient006");
}


struct ModelInfo;
struct studiohdr_t;
struct vcollide_t;
struct virtualmdl_t;
struct cplane_t;
struct mleaf_t;
struct mleafwaterdata_t;
using mvertex_t = Vector3D_F;

class IMaterial;
class UtlBuffer;
class IClientRenderable;
class IPhysCollide;
class EngTrace;
class IModelLoadCallback;

namespace Const
{
	enum class ModelType
	{
		Bad,
		Brush,
		Sprite,
		Studio
	};
}

class IVModelInfo_Internal
{
public:
	virtual							~IVModelInfo_Internal() = default;

	// Returns ModelInfo* pointer for a model given a precached or dynamic model index.
	virtual const ModelInfo* GetModel(int modelindex) abstract;

	// Returns index of model by name for precached or known dynamic models.
	// Does not adjust reference count for dynamic models.
	virtual int						GetModelIndex(const char* name) abstract;

	// Returns name of model
	virtual const char*				GetModelName(const ModelInfo* model) abstract;
	virtual vcollide_t*				GetVCollide(const ModelInfo* model) abstract;
	virtual vcollide_t*				GetVCollide(int modelindex) abstract;
	virtual void					GetModelBounds(const ModelInfo* model, Vector3D_F& mins, Vector3D_F& maxs) abstract;
	virtual	void					GetModelRenderBounds(const ModelInfo* model, Vector3D_F& mins, Vector3D_F& maxs) abstract;
	virtual int						GetModelFrameCount(const ModelInfo* model) abstract;
	virtual Const::ModelType		GetModelType(const ModelInfo* model) abstract;
	virtual void*					GetModelExtraData(const ModelInfo* model) abstract;
	virtual bool					ModelHasMaterialProxy(const ModelInfo* model) abstract;
	virtual bool					IsTranslucent(ModelInfo const* model) abstract;
	virtual bool					IsTranslucentTwoPass(const ModelInfo* model) abstract;
	virtual void					RecomputeTranslucency(const ModelInfo* model, int nSkin, int nBody, void /*IClientRenderable*/* pClientRenderable, float fInstanceAlphaModulate = 1.0f) abstract;
	virtual int						GetModelMaterialCount(const ModelInfo* model) abstract;
	virtual void					GetModelMaterials(const ModelInfo* model, int count, IMaterial** ppMaterial) abstract;
	virtual bool					IsModelVertexLit(const ModelInfo* model) abstract;
	virtual const char*				GetModelKeyValueText(const ModelInfo* model) abstract;
	virtual bool					GetModelKeyValue(const ModelInfo* model, UtlBuffer& buf) abstract; // supports keyvalue blocks in submodels
	virtual float					GetModelRadius(const ModelInfo* model) abstract;

	virtual const studiohdr_t* FindModel(const studiohdr_t* pStudioHdr, void** cache, const char* modelname) abstract;
	virtual const studiohdr_t* FindModel(void* cache) abstract;
	virtual	virtualmdl_t* GetVirtualModel(const studiohdr_t* pStudioHdr) abstract;
	virtual unsigned char* GetAnimBlock(const studiohdr_t* pStudioHdr, int iBlock) abstract;

	// Available on client only!!!
	virtual void					GetModelMaterialColorAndLighting(const ModelInfo* model, Vector3D_F const& origin,
		Angle_F const& angles, EngTrace* pTrace,
		Vector3D_F& lighting, Vector3D_F& matColor) abstract;
	virtual void					GetIlluminationPoint(const ModelInfo* model, IClientRenderable* pRenderable, Vector3D_F const& origin,
		Angle_F const& angles, Vector3D_F* pLightingCenter) abstract;

	virtual int						GetModelContents(int modelIndex) abstract;
	virtual studiohdr_t* GetStudiomodel(const ModelInfo* mod) abstract;
	virtual int						GetModelSpriteWidth(const ModelInfo* model) abstract;
	virtual int						GetModelSpriteHeight(const ModelInfo* model) abstract;

	// Sets/gets a map-specified fade range (client only)
	virtual void					SetLevelScreenFadeRange(float flMinSize, float flMaxSize) abstract;
	virtual void					GetLevelScreenFadeRange(float* pMinArea, float* pMaxArea) abstract;

	// Sets/gets a map-specified per-view fade range (client only)
	virtual void					SetViewScreenFadeRange(float flMinSize, float flMaxSize) abstract;

	// Computes fade alpha based on distance fade + screen fade (client only)
	virtual unsigned char			ComputeLevelScreenFade(const Vector3D_F& vecAbsOrigin, float flRadius, float flFadeScale) abstract;
	virtual unsigned char			ComputeViewScreenFade(const Vector3D_F& vecAbsOrigin, float flRadius, float flFadeScale) abstract;

	// both client and server
	virtual int						GetAutoplayList(const studiohdr_t* pStudioHdr, unsigned short** pAutoplayList) abstract;

	// Gets a virtual terrain collision model (creates if necessary)
	// NOTE: This may return NULL if the terrain model cannot be virtualized
	virtual IPhysCollide* GetCollideForVirtualTerrain(int index) abstract;

	virtual bool					IsUsingFBTexture(const ModelInfo* model, int nSkin, int nBody, void /*IClientRenderable*/* pClientRenderable) abstract;

	// Obsolete methods. These are left in to maintain binary compatibility with clients using the IVModelInfo old version.
	virtual const ModelInfo* FindOrLoadModel(const char* name) { return nullptr; }
	virtual void					InitDynamicModels() {  }
	virtual void					ShutdownDynamicModels() { ; }
	virtual void					AddDynamicModel(const char* name, int nModelIndex = -1) {  }
	virtual void					ReferenceModel(int modelindex) { }
	virtual void					UnreferenceModel(int modelindex) {  }
	virtual void					CleanupDynamicModels(bool bForce = false) {  }

	virtual unsigned short			GetCacheHandle(const ModelInfo* model) abstract;

	// Returns planes of non-nodraw brush model surfaces
	virtual int						GetBrushModelPlaneCount(const ModelInfo* model) abstract;
	virtual void					GetBrushModelPlane(const ModelInfo* model, int nIndex, cplane_t& plane, Vector3D_F* pOrigin) abstract;
	virtual int						GetSurfacepropsForVirtualTerrain(int index) abstract;

	// Poked by engine host system
	virtual void					OnLevelChange() abstract;

	virtual int						GetModelClientSideIndex(const char* name) abstract;

	// Returns index of model by name, dynamically registered if not already known.
	virtual int						RegisterDynamicModel(const char* name, bool bClientSide) abstract;

	virtual bool					IsDynamicModelLoading(int modelIndex) abstract;

	virtual void					AddRefDynamicModel(int modelIndex) abstract;
	virtual void					ReleaseDynamicModel(int modelIndex) abstract;

	// Registers callback for when dynamic model has finished loading.
	// Automatically adds reference, pair with ReleaseDynamicModel.
	virtual bool					RegisterModelLoadCallback(int modelindex, IModelLoadCallback* pCallback, bool bCallImmediatelyIfLoaded = true) abstract;
	virtual void					UnregisterModelLoadCallback(int modelindex, IModelLoadCallback* pCallback) abstract;
};

class IVModelInfo : public IVModelInfo_Internal
{
	virtual void OnDynamicModelsStringTableChange(int nStringIndex, const char* pString, const void* pData) abstract;

	// For tools only!
	virtual const ModelInfo* FindOrLoadModel(const char* name) abstract;
};

namespace Const
{
	namespace ModelFlags
	{
		// we've got a material proxy
		static constexpr uint32_t MaterialProxy			= 0x0'001;
		// we've got a translucent model
		static constexpr uint32_t Translucent			= 0x0'002;
		// we've got a vertex-lit model
		static constexpr uint32_t VertexLit				= 0x0'004;
		// render opaque part in opaque pass
		static constexpr uint32_t Transulcent_TwoPass	= 0x0'008;
		// we need the framebuffer as a texture
		static constexpr uint32_t FrameBuffer_Texture	= 0x0'010;
		// need to check dlights
		static constexpr uint32_t HasDlight				= 0x0'020;
		// persisted from studiohdr
		static constexpr uint32_t SHDR_UsesFBTexture	= 0x0'100;
		// persisted from studiohdr
		static constexpr uint32_t SHDR_UsesBumpMapping	= 0x0'200;
		// persisted from studiohdr
		static constexpr uint32_t SHDR_UsesEnvCubemap	= 0x0'400;
		// persisted from studiohdr
		static constexpr uint32_t SHDR_AmbientBoost		= 0x0'800;
		// persisted from studiohdr
		static constexpr uint32_t SHDR_DontCastShadows	= 0x1'000;
	}
}

/*struct ModelInfo
{
	void*				FileNameHndl;
	const char*			ModelName;

	int					LoadFlags;
	int					ServerCount;
	IMaterial**			Marterials;

	int					ModelType;
	uint32_t			ModelFlags;

	Vector3D_F			Mins, Maxs;
	float				Radius;

	union
	{
		uint8_t			_PadBrush[16];
		uint16_t		StudioHndl;
		uint8_t			_PadSprite[16];
	};
};*/


TF2_NAMESPACE_END();
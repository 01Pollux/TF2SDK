#pragma once

#include "Math/Matrix.hpp"
#include "Materials/Consts.hpp"

TF2_NAMESPACE_BEGIN();

class IVModelRender;

namespace Interfaces
{
	TF2_EXPORT_INTERFACE(IVModelRender, ModelRenderer, "VEngineModel016");
}

class IClientRenderable;
class IMaterial;
class CStudioHdr;
class IMesh;
class ITexture;
class IPooledVBAllocator;

struct studiohdr_t;
struct studiohwdata_t;
struct DrawModelState_t;
struct DrawModelInfo_t;
struct ModelRenderInfo_t;
struct StaticPropRenderInfo_t;
struct model_t;
struct GameRay;


namespace Const
{
	namespace DrawModelFlags
	{
		static constexpr uint32_t draw_entire_model		= 0;
		static constexpr uint32_t draw_opaque_only		= 0x01;
		static constexpr uint32_t draw_translucent_only	= 0x02;
		static constexpr uint32_t draw_group_mask		= 0x03;
		static constexpr uint32_t draw_no_flexes		= 0x04;
		static constexpr uint32_t draw_static_lighting	= 0x08;
		static constexpr uint32_t draw_accuratetime		= 0x10;		//UseAccurateTimingWhenDrawingTheModel.
		static constexpr uint32_t draw_no_shadows		= 0x20;
		static constexpr uint32_t draw_get_perf_stats	= 0x40;
		static constexpr uint32_t draw_wireframe		= 0x80;
		static constexpr uint32_t draw_item_blink		= 0x100;
		static constexpr uint32_t shadowdepthtexture	= 0x200;
		static constexpr uint32_t ssaodepthtexture		= 0x1000;
		static constexpr uint32_t generate_stats		= 0x8000;
	}

	enum class OverrideType
	{
		Normal,
		Build_Shadows,
		Depth_Write,
		SSAO_Depth_Write
	};

	enum class LightType;
}


class IVModelRender
{
public:
	virtual int	DrawModel(int flags,
		IClientRenderable* pRenderable,
		void* instance,
		int entity_index,
		const model_t* model,
		Vector3D_F const& origin,
		Angle_F const& angles,
		int skin,
		int body,
		int hitboxset,
		const Matrix3x4_F* modelToWorld = nullptr,
		const Matrix3x4_F* pLightingOffset = nullptr
	) abstract;

	// This causes a material to be used when rendering the model instead 
	// of the materials the model was compiled with
	virtual void	ForcedMaterialOverride(IMaterial* newMaterial, Const::OverrideType nOverrideType = Const::OverrideType::Normal) abstract;

	virtual void	SetViewTarget(const CStudioHdr* pStudioHdr, int nBodyIndex, const Vector3D_F& target) abstract;

	// Creates, destroys instance data to be associated with the model
	virtual void* CreateInstance(IClientRenderable* pRenderable, void* pLightCache = nullptr) abstract;
	virtual void DestroyInstance(void* handle) abstract;

	// Associates a particular lighting condition with a model instance handle.
	// FIXME: This feature currently only works for static props. To make it work for entities, etc.,
	// we must clean up the lightcache handles as the model instances are removed.
	// At the moment, since only the static prop manager uses this, it cleans up all LightCacheHandles 
	// at level shutdown.
	virtual void SetStaticLighting(void* handle, void** pHandle) abstract;
	virtual void* GetStaticLighting(void* handle) abstract;

	// moves an existing InstanceHandle to a nex Renderable to keep decals etc. Models must be the same
	virtual bool ChangeInstance(void* handle, IClientRenderable* pRenderable) abstract;

	// Creates a decal on a model instance by doing a planar projection
	// along the ray. The material is the decal material, the radius is the
	// radius of the decal to create.
	virtual void AddDecal(void* handle, const GameRay& ray,
		const Vector3D_F& decalUp, int decalIndex, int body, bool noPokeThru = false, int maxLODToDecal = -1) abstract;

	// Removes all the decals on a model instance
	virtual void RemoveAllDecals(void* handle) abstract;

	// Remove all decals from all models
	virtual void RemoveAllDecalsFromAllModels() abstract;

	// Shadow rendering, DrawModelShadowSetup returns the address of the bone-to-world array, NULL in case of error
	virtual Matrix3x4_F* DrawModelShadowSetup(IClientRenderable* pRenderable, int body, int skin, DrawModelInfo_t* pInfo, Matrix3x4_F* pCustomBoneToWorld = nullptr) abstract;
	virtual void DrawModelShadow(IClientRenderable* pRenderable, const DrawModelInfo_t& info, Matrix3x4_F* pCustomBoneToWorld = nullptr) abstract;

	// This gets called when overbright, etc gets changed to recompute static prop lighting.
	virtual bool RecomputeStaticLighting(void* handle) abstract;

	virtual void ReleaseAllStaticPropColorData(void) abstract;
	virtual void RestoreAllStaticPropColorData(void) abstract;

	// Extended version of drawmodel
	virtual int	DrawModelEx(ModelRenderInfo_t& pInfo) abstract;

	virtual int	DrawModelExStaticProp(ModelRenderInfo_t& pInfo) abstract;

	virtual bool DrawModelSetup(ModelRenderInfo_t& pInfo, DrawModelState_t* pState, Matrix3x4_F* pCustomBoneToWorld, Matrix3x4_F** ppBoneToWorldOut) abstract;
	virtual void DrawModelExecute(const DrawModelState_t& state, const ModelRenderInfo_t& pInfo, Matrix3x4_F* pCustomBoneToWorld = nullptr) abstract;

	// Sets up lighting context for a point in space
	virtual void SetupLighting(const Vector3D_F& vecCenter) abstract;

	// doesn't support any debug visualization modes or other model options, but draws static props in the
	// fastest way possible
	virtual int DrawStaticPropArrayFast(StaticPropRenderInfo_t* pProps, int count, bool bShadowDepth) abstract;

	// Allow client to override lighting state
	virtual void SuppressEngineLighting(bool bSuppress) abstract;

	virtual void SetupColorMeshes(int nTotalVerts) abstract;

	virtual void AddColoredDecal(void* handle, const GameRay& ray,
		const Vector3D_F& decalUp, int decalIndex, int body, Color4_8 cColor, bool noPokeThru = false, int maxLODToDecal = -1) abstract;

	virtual void GetMaterialOverride(IMaterial** ppOutForcedMaterial, Const::OverrideType* pOutOverrideType) abstract;
};


struct DrawModelState_t
{
	studiohdr_t*		StudioHdr;
	studiohwdata_t*		StudioHWData;
	IClientRenderable*	Renderable;
	const Matrix3x4_F*	ModelToWorld;
	void*				Decals;
	int					DrawFlags;
	int					LOD;
};


struct ModelRenderInfo_t
{
	Vector3D_F			Origin;
	Angle_F				Angles;
	IClientRenderable*	Renderable;
	const model_t*		Model;
	const Matrix3x4_F*	ModelToWorld{ };
	const Matrix3x4_F*	LightingOffset{ };
	const Vector3D_F*	LightingOrigin{ };
	int					flags;
	int					entity_index;
	int					skin;
	int					body;
	int					hitboxset;
	uint16_t			Instance;
};

struct StaticPropRenderInfo_t
{
	const Matrix3x4_F*	ModelToWorld;
	const model_t*		Model;
	IClientRenderable*	Renderable;
	Vector3D_F*			LightingOrigin;
	short				Skin;
	uint16_t			Instance;
};

struct ColorTexelsInfo_t
{
	int					Width;
	int					Height;
	int					MipmapCount;
	Const::ImageFormat	ImageFormat;
	int					ByteCount;
	uint8_t*			TexelData;
};

struct ColorMeshInfo_t
{
	// A given color mesh can own a unique Mesh, or it can use a shared Mesh
	// (in which case it uses a sub-range defined by m_nVertOffset and m_nNumVerts)
	IMesh* m_pMesh;
	IPooledVBAllocator* m_pPooledVBAllocator;
	int						m_nVertOffsetInBytes;
	int						m_nNumVerts;
	ITexture* m_pLightmap;
	ColorTexelsInfo_t* m_pLightmapData;
};

struct LightDesc_t
{
	Const::LightType Type;			//< MATERIAL_LIGHT_xxx
	Vector3D_F	Color;				//< color+intensity 
	Vector3D_F	Position;			//< light source center position
	Vector3D_F	Direction;			//< for SPOT, direction it is pointing
	float	Range;					//< distance range for light.0=infinite
	float	Falloff;				//< angular falloff exponent for spot lights
	float	Attenuation0;			//< constant distance falloff term
	float	Attenuation1;			//< linear term of falloff
	float	Attenuation2;			//< quadatic term of falloff
	float	Theta;					//< inner cone angle. no angular falloff 
									//< within this cone
	float Phi;						//< outer cone angle

	// the values below are derived from the above settings for optimizations
	// These aren't used by DX8. . used for software lighting.
	float ThetaDot;
	float PhiDot;
	unsigned int Flags;
protected:
	float OneOver_ThetaDot_Minus_PhiDot;
	float RangeSquared;
};

struct DrawModelInfo_t
{
	studiohdr_t*	StudioHdr;
	studiohwdata_t* HardwareData;
	void*			Decals;
	int				Skin;
	int				Body;
	int				HitboxSet;
	void*			ClientEntity;
	int				Lod;
	ColorMeshInfo_t* ColorMeshes;
	bool			StaticLighting;
	Vector3D_F		AmbientCube[6];		// ambient, and lights that aren't in locallight[]
	int				LocalLightCount;
	LightDesc_t		LocalLightDescs[4];
};

TF2_NAMESPACE_END();

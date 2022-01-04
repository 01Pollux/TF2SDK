#pragma once

#include <tf2/engine/AppSystem.hpp>
#include <tf2/studio/Studio.hpp>
#include <tf2/studio/LightDesc.hpp>
#include "Consts.hpp"

TF2_NAMESPACE_BEGIN();

class IStudioRender;
namespace interfaces
{
	TF2_EXPORT_INTERFACE(IStudioRender, StudioRender, "VStudioRender025");
}

namespace utils { class UtlBuffer; }

class IMaterialSystem;
class ITexture;

struct FlashlightState_t;
struct GameRay;


namespace Const
{
	struct StudioRenderConfig
	{
		float EyeShiftX;	// eye X position
		float EyeShiftY;	// eye Y position
		float EyeShiftZ;	// eye Z position
		float EyeSize;		// adjustment to iris textures
		float EyeGlintPixelWidthLODThreshold;

		int MaxDecalsPerModel;
		int DrawEntities;
		int Skin;
		int Fullbright;

		bool EyeMove : 1;		// look around
		bool SoftwareSkin : 1;
		bool NoHardware : 1;
		bool NoSoftware : 1;
		bool Teeth : 1;
		bool Eyes : 1;
		bool Flex : 1;
		bool Wireframe : 1;
		bool DrawNormals : 1;
		bool DrawTangentFrame : 1;
		bool DrawZBufferedWireframe : 1;
		bool SoftwareLighting : 1;
		bool ShowEnvCubemapOnly : 1;
		bool WireframeDecals : 1;

		// Reserved for future use
		int _Reserved[4];
	};

	namespace DrawModelFLags
	{
		static constexpr uint32_t EntireModel		= 0;
		static constexpr uint32_t OpaqueOnly		= 0x1;
		static constexpr uint32_t Translucent_Only	= 0x2;
		static constexpr uint32_t GroupMask			= 0x3;

		static constexpr uint32_t NoFlexes			= 0x4;
		static constexpr uint32_t StaticLighting	= 0x8;

		// Use accurate timing when drawing the model.
		static constexpr uint32_t AccurateTime		= 0x10;
		static constexpr uint32_t NoShadows			= 0x20;
		static constexpr uint32_t GetPerfStats		= 0x40;

		static constexpr uint32_t WireFrame			= 0x80;

		static constexpr uint32_t ItemBlink			= 0x100;

		static constexpr uint32_t Shadow_DepthTexture= 0x200;

		static constexpr uint32_t SSAO_DepthTexture = 0x1000;

		static constexpr uint32_t GenerateStats		= 0x8000;
	};


	//-----------------------------------------------------------------------------
	// Standard model vertex formats
	//-----------------------------------------------------------------------------
	// FIXME: remove these (materials/shaders should drive vertex format). Need to
	//        list required forcedmaterialoverrides in models/bsps (rather than
	//        all models supporting all possible overrides, as they do currently).
	namespace MatVertexFormatFlags
	{
		static constexpr uint32_t Skinned =
			VertexFormat::Position | VertexFormat::Color | VertexFormat::Normal |
			(2 << VertexFormat::TexCoordBit) | (2 << VertexFormat::BoneWeightBit) | VertexFormat::BoneIndex | (4 << VertexFormat::UserDataBit);
		static constexpr uint32_t Skinned_Dx7 =
			VertexFormat::Position | VertexFormat::Color | VertexFormat::Normal |
			(2 << VertexFormat::TexCoordBit) | (2 << VertexFormat::BoneWeightBit) | VertexFormat::BoneIndex;

		static constexpr uint32_t Model =
			VertexFormat::Position | VertexFormat::Color | VertexFormat::Normal |
			(2 << VertexFormat::TexCoordBit) | (4 << VertexFormat::UserDataBit);
		static constexpr uint32_t Model_Dx7 =
			VertexFormat::Position | VertexFormat::Color | VertexFormat::Normal |
			(2 << VertexFormat::TexCoordBit);

		static constexpr uint32_t Color = VertexFormat::Specular;
	}

	//-----------------------------------------------------------------------------
	// What kind of material override is it?
	//-----------------------------------------------------------------------------
	enum class MatOverrideType
	{
		Normal,
		BuildShadows,
		DepthWrite,
		SSAO_DepthWrite
	};


	//-----------------------------------------------------------------------------
	// DrawModel info
	//-----------------------------------------------------------------------------

	// Special flag for studio models that have a compiled in shadow lod version
	// It's negative 2 since positive numbers == use a regular slot and -1 means
	//  have studiorender compute a value instead
	static constexpr int StudioModel_UseShadowLod = -2;

	struct DrawModelResults
	{
		int ActualTriCount;
		int TextureMemoryBytes;
		int NumHardwareBones;
		int NumBatches;
		int NumMaterials;
		int LODUsed;
		int LODMetric;
		uint64_t RenderTime;
		utils::UtlVector<IMaterial*, utils::UtlMemoryFixed<IMaterial*, 8>> Materials;
	};

	struct ColorTexelsInfo
	{
		int			Width;
		int			Height;
		int			MipmapCount;
		ImageFormat	mageFormat;
		int			ByteCount;
		std::byte*	TexelData;
	};

	struct ColorMeshInfo
	{
		// A given color mesh can own a unique Mesh, or it can use a shared Mesh
		// (in which case it uses a sub-range defined by m_nVertOffset and m_nNumVerts)
		IMesh*	Mesh;
		void*	PooledVBAllocator;
		int		VertOffsetInBytes;
		int		NumVerts;
		ITexture* Lightmap;
		ColorTexelsInfo* LightmapData;
	};

	struct DrawModelInfo
	{
		studiohdr_t*	StudioHdr;
		studiohwdata_t* HardwareData;
		void*			Decals;
		int				Skin;
		int				Body;
		int				HitboxSet;
		void*			ClientEntity;
		int				Lod;
		ColorMeshInfo*	ColorMeshes;
		bool			StaticLighting;
		Vector3D_F		AmbientCube[6];		// ambient, and lights that aren't in locallight[]
		int				LocalLightCount;
		LightDesc_t		LocalLightDescs[4];
	};

	struct GetTriangles_Vertex
	{
		Vector3D_F	Position;
		Vector3D_F	Normal;
		Vector4D_F	TangentS;
		Vector2D_F	TexCoord;
		Vector4D_F	BoneWeight;
		int			BoneIndex[4];
		int			NumBones;
	};

	struct GetTriangles_MaterialBatch
	{
		IMaterial*								Material;
		utils::UtlVector<GetTriangles_Vertex>	Verts;
		utils::UtlVector<int>					TriListIndices;
	};

	struct GetTriangles_Output
	{
		utils::UtlVector<GetTriangles_MaterialBatch>	MaterialBatches;
		Matrix3x4_F										PoseToWorld[Const::MaxStudio_Bones];
	};
}

class IStudioRender : public IAppSystem
{
public:
	virtual void BeginFrame() abstract;
	virtual void EndFrame() abstract;

	// Used for the mat_stub console command.
	virtual void Mat_Stub(IMaterialSystem* pMatSys) abstract;

	// Updates the rendering configuration 
	virtual void UpdateConfig(const Const::StudioRenderConfig& config) abstract;
	virtual void GetCurrentConfig(Const::StudioRenderConfig& config) abstract;

	// Load, unload model data
	virtual bool LoadModel(studiohdr_t* pStudioHdr, void* pVtxData, studiohwdata_t* pHardwareData) abstract;
	virtual void UnloadModel(studiohwdata_t* pHardwareData) abstract;

	// Refresh the studiohdr since it was lost...
	virtual void RefreshStudioHdr(studiohdr_t* pStudioHdr, studiohwdata_t* pHardwareData) abstract;

	// This is needed to do eyeglint and calculate the correct texcoords for the eyes.
	virtual void SetEyeViewTarget(const studiohdr_t* pStudioHdr, int nBodyIndex, const Vector3D_F& worldPosition) abstract;

	// Methods related to lighting state
	// NOTE: SetAmbientLightColors assumes that the arraysize is the same as 
	// returned from GetNumAmbientLightSamples
	virtual int GetNumAmbientLightSamples() abstract;
	virtual const Vector3D_F* GetAmbientLightDirections() abstract;
	virtual void SetAmbientLightColors(const Vector4D_F* pAmbientOnlyColors) abstract;
	virtual void SetAmbientLightColors(const Vector3D_F* pAmbientOnlyColors) abstract;
	virtual void SetLocalLights(int numLights, const LightDesc_t* pLights) abstract;

	// Sets information about the camera location + orientation
	virtual void SetViewState(const Vector3D_F& viewOrigin, const Vector3D_F& viewRight,
		const Vector3D_F& viewUp, const Vector3D_F& viewPlaneNormal) abstract;

	// Allocates flex weights for use in rendering
	// NOTE: Pass in a non-null second parameter to lock delayed flex weights
	virtual void LockFlexWeights(int nWeightCount, float** ppFlexWeights, float** ppFlexDelayedWeights = nullptr) abstract;
	virtual void UnlockFlexWeights() abstract;

	// Used to allocate bone matrices to be used to pass into DrawModel
	virtual Matrix3x4_F* LockBoneMatrices(int nBoneCount) abstract;
	virtual void UnlockBoneMatrices() abstract;

	// LOD stuff
	virtual int GetNumLODs(const studiohwdata_t& hardwareData) const abstract;
	virtual float GetLODSwitchValue(const studiohwdata_t& hardwareData, int lod) const abstract;
	virtual void SetLODSwitchValue(studiohwdata_t& hardwareData, int lod, float switchValue) abstract;

	// Sets the color/alpha modulation
	virtual void SetColorModulation(float const* pColor) abstract;
	virtual void SetAlphaModulation(float flAlpha) abstract;

	// Draws the model
	virtual void DrawModel(
		Const::DrawModelResults* pResults,
		const Const::DrawModelInfo& info,
		Matrix3x4_F* pBoneToWorld, 
		float* pFlexWeights, 
		float* pFlexDelayedWeights,
		const Vector3D_F& modelOrigin, 
		uint32_t flags = Const::DrawModelFLags::EntireModel
	) abstract;

	// Methods related to static prop rendering
	virtual void DrawModelStaticProp(
		const Const::DrawModelInfo& drawInfo,
		const Matrix3x4_F& modelToWorld,
		uint32_t flags = Const::DrawModelFLags::EntireModel
	) abstract;
	virtual void DrawStaticPropDecals(
		const Const::DrawModelInfo& drawInfo, 
		const Matrix3x4_F& modelToWorld
	) abstract;
	virtual void DrawStaticPropShadows(
		const Const::DrawModelInfo& drawInfo, 
		const Matrix3x4_F& modelToWorld, 
		uint32_t flags
	) abstract;

	// Causes a material to be used instead of the materials the model was compiled with
	virtual void ForcedMaterialOverride(
		IMaterial* newMaterial,
		Const::MatOverrideType nOverrideType = Const::MatOverrideType::Normal
	) abstract;

	// Create, destroy list of decals for a particular model
	virtual void* CreateDecalList(studiohwdata_t* pHardwareData) abstract;
	virtual void DestroyDecalList(void* handle) abstract;

	// Add decals to a decal list by doing a planar projection along the ray
	// The BoneToWorld matrices must be set before this is called
	virtual void AddDecal(
		void* handle, 
		studiohdr_t* pStudioHdr,
		Matrix3x4_F* pBoneToWorld,
		const GameRay& ray, 
		const Vector3D_F& decalUp,
		IMaterial* pDecalMaterial, 
		float radius,
		int body, 
		bool noPokethru = false,
		int maxLODToDecal = -1
	) abstract;

	// Compute the lighting at a point and normal
	virtual void ComputeLighting(
		const Vector3D_F* pAmbient, 
		int lightCount,
		LightDesc_t* pLights, 
		const Vector3D_F& pt,
		const Vector3D_F& normal, 
		Vector3D_F& lighting
	) abstract;

	// Compute the lighting at a point, constant directional component is passed
	// as flDirectionalAmount
	virtual void ComputeLightingConstDirectional(
		const Vector3D_F* pAmbient,
		int lightCount,
		LightDesc_t* pLights, 
		const Vector3D_F& pt, 
		const Vector3D_F& normal, 
		Vector3D_F& lighting, 
		float flDirectionalAmount
	) abstract;

	// Shadow state (affects the models as they are rendered)
	virtual void AddShadow(
		IMaterial* pMaterial, 
		void* pProxyData, 
		FlashlightState_t* m_pFlashlightState = nullptr,
		Matrix4x4_F* pWorldToTexture = nullptr,
		ITexture* pFlashlightDepthTexture = nullptr
	) abstract;

	virtual void ClearAllShadows() abstract;

	// Gets the model LOD; pass in the screen size in pixels of a sphere 
	// of radius 1 that has the same origin as the model to get the LOD out...
	virtual int ComputeModelLod(studiohwdata_t* pHardwareData, float unitSphereSize, float* pMetric = nullptr) abstract;

	// Return a number that is usable for budgets, etc.
	// Things that we care about:
	// 1) effective triangle count (factors in batch sizes, state changes, etc)
	// 2) texture memory usage
	// Get Triangles returns the LOD used
	virtual void GetPerfStats(
		Const::DrawModelResults* pResults, 
		const Const::DrawModelInfo& info,
		utils::UtlBuffer* pSpewBuf = nullptr
	) const abstract;

	virtual void GetTriangles(
		const Const::DrawModelInfo& info,
		Matrix3x4_F* pBoneToWorld, 
		Const::GetTriangles_Output& out
	) abstract;

	// Returns materials used by a particular model
	virtual int GetMaterialList(
		studiohdr_t* pStudioHdr,
		int count,
		IMaterial** ppMaterials
	) abstract;

	virtual int GetMaterialListFromBodyAndSkin(
		uint16_t studio,
		int nSkin,
		int nBody,
		int nCountOutputMaterials,
		IMaterial** ppOutputMaterials
	) abstract;

	// draw an array of models with the same state
	virtual void DrawModelArray(
		const Const::DrawModelInfo& drawInfo,
		int arrayCount,
		Matrix3x4_F* pInstanceData,
		int instanceStride,
		int flags = Const::DrawModelFLags::EntireModel
	) abstract;

	virtual void GetMaterialOverride(
		IMaterial** ppOutForcedMaterial,
		Const::MatOverrideType* pOutOverrideType
	) abstract;
};


TF2_NAMESPACE_END();
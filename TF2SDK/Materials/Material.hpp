#pragma once

#include "Consts.hpp"
#include "Math/Vector.hpp"

TF2_NAMESPACE_BEGIN();

class IMaterialVar;
class KeyValues;


class IMaterial
{
public:
	// Get the name of the material.  This is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	virtual const char* GetName() const abstract;
	virtual const char* GetTextureGroupName() const abstract;

	// Get the preferred size/bitDepth of a preview image of a material.
	// This is the sort of image that you would use for a thumbnail view
	// of a material, or in WorldCraft until it uses materials to render.
	// separate this for the tools maybe
	virtual Const::PreviewImageRetVal GetPreviewImageProperties(
		int* width, int* height,
		Const::ImageFormat* imageFormat, bool* isTranslucent) const abstract;

	// Get a preview image at the specified width/height and bitDepth.
	// Will do resampling if necessary.(not yet!!! :) )
	// Will do color format conversion. (works now.)
	virtual Const::PreviewImageRetVal GetPreviewImage(
		unsigned char* data, int width, int height,
		Const::ImageFormat imageFormat) const abstract;
	// 
	virtual int				GetMappingWidth() abstract;
	virtual int				GetMappingHeight() abstract;

	virtual int				GetNumAnimationFrames() abstract;

	// For material subrects (material pages).  Offset(u,v) and scale(u,v) are normalized to texture.
	virtual bool			InMaterialPage() abstract;
	virtual	void			GetMaterialOffset(float* pOffset) abstract;
	virtual void			GetMaterialScale(float* pScale) abstract;
	virtual IMaterial*		GetMaterialPage() abstract;

	// find a vmt variable.
	// This is how game code affects how a material is rendered.
	// The game code must know about the params that are used by
	// the shader for the material that it is trying to affect.
	virtual IMaterialVar*	FindVar(const char* varName, bool* found, bool complain = true) abstract;

	// The user never allocates or deallocates materials.  Reference counting is
	// used instead.  Garbage collection is done upon a call to 
	// IMaterialSystem::UncacheUnusedMaterials.
	virtual void			IncrementReferenceCount() abstract;
	virtual void			DecrementReferenceCount() abstract;

	inline void				AddRef() { IncrementReferenceCount(); }
	inline void				Release() { DecrementReferenceCount(); }

	// Each material is assigned a number that groups it with like materials
	// for sorting in the application.
	virtual int 			GetEnumerationID() const abstract;

	virtual void			GetLowResColorSample(float s, float t, float* color) const abstract;

	// This computes the state snapshots for this material
	virtual void			RecomputeStateSnapshots() abstract;

	// Are we translucent?
	virtual bool			IsTranslucent() abstract;

	// Are we alphatested?
	virtual bool			IsAlphaTested() abstract;

	// Are we vertex lit?
	virtual bool			IsVertexLit() abstract;

	// Gets the vertex format
	virtual uint64_t		GetVertexFormat() const abstract;

	// returns true if this material uses a material proxy
	virtual bool			HasProxy() const abstract;

	virtual bool			UsesEnvCubemap() abstract;

	virtual bool			NeedsTangentSpace() abstract;

	virtual bool			NeedsPowerOfTwoFrameBufferTexture(bool bCheckSpecificToThisFrame = true) abstract;
	virtual bool			NeedsFullFrameBufferTexture(bool bCheckSpecificToThisFrame = true) abstract;

	// returns true if the shader doesn't do skinning itself and requires
	// the data that is sent to it to be preskinned.
	virtual bool			NeedsSoftwareSkinning() abstract;

	// Apply constant color or alpha modulation
	virtual void			AlphaModulate(float alpha) abstract;
	virtual void			ColorModulate(float r, float g, float b) abstract;

	// Material Var flags...
	virtual void			SetMaterialVarFlag(uint32_t flag, bool on) abstract;
	virtual bool			GetMaterialVarFlag(uint32_t flag) const abstract;

	// Gets material reflectivity
	virtual void			GetReflectivity(Vector3D_F& reflect) abstract;

	// Gets material property flags
	virtual bool			GetPropertyFlag(Const::MaterialPropertyTypes type) abstract;

	// Is the material visible from both sides?
	virtual bool			IsTwoSided() abstract;

	// Sets the shader associated with the material
	virtual void			SetShader(const char* pShaderName) abstract;

	// Can't be const because the material might have to precache itself.
	virtual int				GetNumPasses() abstract;

	// Can't be const because the material might have to precache itself.
	virtual int				GetTextureMemoryBytes() abstract;

	// Meant to be used with materials created using CreateMaterial
	// It updates the materials to reflect the current values stored in the material vars
	virtual void			Refresh() abstract;

	// GR - returns true is material uses lightmap alpha for blending
	virtual bool			NeedsLightmapBlendAlpha() abstract;

	// returns true if the shader doesn't do lighting itself and requires
	// the data that is sent to it to be prelighted
	virtual bool			NeedsSoftwareLighting() abstract;

	// Gets at the shader parameters
	virtual int				ShaderParamCount() const abstract;
	virtual IMaterialVar** GetShaderParams() abstract;

	// Returns true if this is the error material you get back from IMaterialSystem::FindMaterial if
	// the material can't be found.
	virtual bool			IsErrorMaterial() const abstract;

	virtual void			SetUseFixedFunctionBakedLighting(bool bEnable) abstract;

	// Gets the current alpha modulation
	virtual float			GetAlphaModulation() abstract;
	virtual void			GetColorModulation(float* r, float* g, float* b) abstract;

	// Gets the morph format
	virtual uint32_t		GetMorphFormat() const abstract;

	// fast find that stores the index of the found var in the string table in local cache
	virtual IMaterialVar*	FindVarFast(char const* pVarName, unsigned int* pToken) abstract;

	// Sets new VMT shader parameters for the material
	virtual void			SetShaderAndParams(KeyValues* pKeyValues) abstract;
	virtual const char*		GetShaderName() const abstract;

	virtual void			DeleteIfUnreferenced() abstract;

	virtual bool			IsSpriteCard() abstract;

	virtual void			CallBindProxy(void* proxyData) abstract;

	virtual IMaterial*		CheckProxyReplacement(void* proxyData) abstract;

	virtual void			RefreshPreservingMaterialVars() abstract;

	virtual bool			WasReloadedFromWhitelist() abstract;

	virtual bool			IsPrecached() const abstract;
};


TF2_NAMESPACE_END();

#pragma once

#include "Consts.hpp"

TF2_NAMESPACE_BEGIN();

class IVTFTexture;
struct Rect_t;

class ITextureRegenerator
{
public:
	// This will be called when the texture bits need to be regenerated.
	// Use the VTFTexture interface, which has been set up with the
	// appropriate texture size + format
	// The rect specifies which part of the texture needs to be updated
	// You can choose to update all of the bits if you prefer
	virtual void RegenerateTextureBits(ITexture* pTexture, IVTFTexture* pVTFTexture, Rect_t* pRect) abstract;

	// This will be called when the regenerator needs to be deleted
	// which will happen when the texture is destroyed
	virtual void Release() abstract;

	// (erics): This should have a virtual destructor, but would be ABI breaking (non-versioned interface implemented
	//          by the game)
//	virtual ~ITextureRegenerator(){}
};

class ITexture
{
public:
	// Various texture polling methods
	virtual const char* GetName() const abstract;
	virtual int GetMappingWidth() const abstract;
	virtual int GetMappingHeight() const abstract;
	virtual int GetActualWidth() const abstract;
	virtual int GetActualHeight() const abstract;
	virtual int GetNumAnimationFrames() const abstract;
	virtual bool IsTranslucent() const abstract;
	virtual bool IsMipmapped() const abstract;

	virtual void GetLowResColorSample(float s, float t, float* color) const abstract;

	// Gets texture resource data of the specified type.
	// Params:
	//		eDataType		type of resource to retrieve.
	//		pnumBytes		on return is the number of bytes available in the read-only data buffer or is undefined
	// Returns:
	//		pointer to the resource data, or NULL
	virtual void* GetResourceData(uint32_t eDataType, size_t* pNumBytes) const abstract;

	// Methods associated with reference count
	virtual void IncrementReferenceCount() abstract;
	virtual void DecrementReferenceCount() abstract;

	inline void AddRef() { IncrementReferenceCount(); }
	inline void Release() { DecrementReferenceCount(); }

	// Used to modify the texture bits (procedural textures only)
	virtual void SetTextureRegenerator(ITextureRegenerator* pTextureRegen) abstract;

	// Reconstruct the texture bits in HW memory

	// If rect is not specified, reconstruct all bits, otherwise just
	// reconstruct a subrect.
	virtual void Download(Rect_t* pRect = nullptr, int nAdditionalCreationFlags = 0) abstract;

	// Uses for stats. . .get the approximate size of the texture in it's current format.
	virtual int GetApproximateVidMemBytes() const abstract;

	// Returns true if the texture data couldn't be loaded.
	virtual bool IsError() const abstract;

	// NOTE: Stuff after this is added after shipping HL2.

	// For volume textures
	virtual bool IsVolumeTexture() const abstract;
	virtual int GetMappingDepth() const abstract;
	virtual int GetActualDepth() const abstract;

	virtual Const::ImageFormat GetImageFormat() const abstract;
	virtual uint32_t GetNormalDecodeMode() const abstract;

	// Various information about the texture
	virtual bool IsRenderTarget() const abstract;
	virtual bool IsCubeMap() const abstract;
	virtual bool IsNormalMap() const abstract;
	virtual bool IsProcedural() const abstract;

	virtual void DeleteIfUnreferenced() abstract;

	// swap everything except the name with another texture
	virtual void SwapContents(ITexture* pOther) abstract;

	// Retrieve the vtf flags mask
	virtual unsigned int GetFlags() const abstract;

	// Force LOD override (automatically downloads the texture)
	virtual void ForceLODOverride(int iNumLodsOverrideUpOrDown) abstract;

	// Save texture to a file.
	virtual bool SaveToFile(const char* fileName) abstract;

	// Copy this texture, which must be a render target or a renderable texture, to the destination texture, 
	// which must have been created with the STAGING bit.
	virtual void CopyToStagingTexture(ITexture* pDstTex) abstract;

	// Set that this texture should return true for the call "IsError"
	virtual void SetErrorTexture(bool bIsErrorTexture) abstract;
};

TF2_NAMESPACE_END();
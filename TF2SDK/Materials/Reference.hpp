#pragma once

#include "Consts.hpp"

TF2_NAMESPACE_BEGIN();

class KeyValues;
class IMaterial;
class ITexture;

//-----------------------------------------------------------------------------
// Little utility class to deal with material references
//-----------------------------------------------------------------------------
class IMaterialReference
{
public:
	// constructor, destructor
	SG_SDK_TF2 IMaterialReference(char const* pMaterialName = nullptr, const char* pTextureGroupName = nullptr, bool complain = true);
	SG_SDK_TF2 ~IMaterialReference();

	// Attach to a material
	SG_SDK_TF2 void init(const char* pMaterialName, const char* pTextureGroupName, bool bComplain = true);
	SG_SDK_TF2 void init(const char* pMaterialName, KeyValues* pVMTKeyValues);
	SG_SDK_TF2 void init(IMaterial* pMaterial);
	SG_SDK_TF2 void init(IMaterialReference& ref);
	SG_SDK_TF2 void init(const char* pMaterialName, const char* pTextureGroupName, KeyValues* pVMTKeyValues);

	// Detach from a material
	SG_SDK_TF2 void shutdown();
	bool valid() { return m_Material != nullptr; }

	// Automatic casts to IMaterial
	operator IMaterial*() noexcept { return m_Material; }
	operator IMaterial*() const noexcept { return m_Material; }
	operator const IMaterial*() const noexcept { return m_Material; }
	IMaterial* operator->() noexcept { return m_Material; }

private:
	IMaterial* m_Material{ };
};


//-----------------------------------------------------------------------------
// Little utility class to deal with texture references
//-----------------------------------------------------------------------------
class ITextureReference
{
public:
	// constructor, destructor
	ITextureReference() = default;
	SG_SDK_TF2 ITextureReference(const ITextureReference& ref);
	SG_SDK_TF2 ~ITextureReference();

	// Attach to a texture
	SG_SDK_TF2 void init(char const* pTexture, const char* pTextureGroupName, bool bComplain = true);
	SG_SDK_TF2 void initProceduralTexture(const char* pTextureName, const char* pTextureGroupName, int w, int h, Const::ImageFormat fmt, int nFlags);
	SG_SDK_TF2 void initRenderTarget(int w, int h, Const::RenderTargetSizeMode sizeMode, Const::ImageFormat fmt, Const::MaterialRenderTargetDepth depth, bool bHDR, char* pStrOptionalName = nullptr);
	SG_SDK_TF2 void init(ITexture* pTexture);

	// Detach from a texture
	SG_SDK_TF2 void shutdown(bool bDeleteIfUnReferenced = false);
	bool valid() const noexcept { return m_Texture != nullptr; }

	// Automatic casts to ITexture
	operator ITexture* () noexcept { return m_Texture; }
	operator ITexture const* () const noexcept { return m_Texture; }
	ITexture* operator->() noexcept { return m_Texture; }

	// Assignment operator
	SG_SDK_TF2 void operator=(ITextureReference& ref);

private:
	ITexture* m_Texture{ };
};

TF2_NAMESPACE_END();

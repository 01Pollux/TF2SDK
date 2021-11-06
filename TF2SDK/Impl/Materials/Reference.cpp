
#include "Materials/Reference.hpp"
#include "Materials/MaterialSys.hpp"
#include "Materials/Material.hpp"
#include "Materials/Texture.hpp"

TF2_NAMESPACE_BEGIN();


IMaterialReference::IMaterialReference(char const* pMaterialName, const char* pTextureGroupName, bool bComplain)
{
	if (pMaterialName)
	{
		init(pMaterialName, pTextureGroupName, bComplain);
	}
}

IMaterialReference::~IMaterialReference()
{
	shutdown();
}

//-----------------------------------------------------------------------------
// Attach to a material
//-----------------------------------------------------------------------------
void IMaterialReference::init(char const* pMaterialName, const char* pTextureGroupName, bool bComplain)
{
	IMaterial* pMaterial = Interfaces::MatSys->FindMaterial(pMaterialName, pTextureGroupName, bComplain);
	if (IMaterial::IsError(pMaterial))
		return;

	init(pMaterial);
}

void IMaterialReference::init(const char* pMaterialName, KeyValues* pVMTKeyValues)
{
	// CreateMaterial has a refcount of 1
	shutdown();
	m_Material = Interfaces::MatSys->CreateMaterial(pMaterialName, pVMTKeyValues);
}

void IMaterialReference::init(const char* pMaterialName, const char* pTextureGroupName, KeyValues* pVMTKeyValues)
{
	IMaterial* pMaterial = Interfaces::MatSys->FindProceduralMaterial(pMaterialName, pTextureGroupName, pVMTKeyValues);
	init(pMaterial);
}

void IMaterialReference::init(IMaterial* pMaterial)
{
	if (m_Material != pMaterial)
	{
		shutdown();
		m_Material = pMaterial;
		if (m_Material)
		{
			m_Material->IncrementReferenceCount();
		}
	}
}

void IMaterialReference::init(IMaterialReference& ref)
{
	if (m_Material != ref.m_Material)
	{
		shutdown();
		m_Material = ref.m_Material;
		if (m_Material)
		{
			m_Material->IncrementReferenceCount();
		}
	}
}

//-----------------------------------------------------------------------------
// Detach from a material
//-----------------------------------------------------------------------------
void IMaterialReference::shutdown()
{
	if (m_Material)
	{
		m_Material->DecrementReferenceCount();
		m_Material = nullptr;
	}
}


//-----------------------------------------------------------------------------
// Little utility class to deal with texture references
//-----------------------------------------------------------------------------
ITextureReference::ITextureReference(const ITextureReference& ref)
{
	m_Texture = ref.m_Texture;
	if (m_Texture)
	{
		m_Texture->IncrementReferenceCount();
	}
}

void ITextureReference::operator=(ITextureReference& ref)
{
	m_Texture = ref.m_Texture;
	if (m_Texture)
	{
		m_Texture->IncrementReferenceCount();
	}
}

ITextureReference::~ITextureReference()
{
	shutdown();
}

//-----------------------------------------------------------------------------
// Attach to a texture
//-----------------------------------------------------------------------------
void ITextureReference::init(char const* pTextureName, const char* pTextureGroupName, bool bComplain)
{
	shutdown();
	m_Texture = Interfaces::MatSys->FindTexture(pTextureName, pTextureGroupName, bComplain);
	if (m_Texture)
	{
		m_Texture->IncrementReferenceCount();
	}
}

void ITextureReference::init(ITexture* pTexture)
{
	shutdown();

	m_Texture = pTexture;
	if (m_Texture)
	{
		m_Texture->IncrementReferenceCount();
	}
}

void ITextureReference::initProceduralTexture(const char* pTextureName, const char* pTextureGroupName, int w, int h, Const::ImageFormat fmt, int nFlags)
{
	shutdown();

	m_Texture = Interfaces::MatSys->CreateProceduralTexture(pTextureName, pTextureGroupName, w, h, fmt, nFlags);

	// NOTE: The texture reference is already incremented internally above!
	/*
	if ( m_Texture )
	{
		m_Texture->IncrementReferenceCount();
	}
	*/
}

void ITextureReference::initRenderTarget(int w, int h, Const::RenderTargetSizeMode sizeMode, Const::ImageFormat fmt, Const::MaterialRenderTargetDepth depth, bool bHDR, char* pStrOptionalName)
{
	shutdown();

	int textureFlags = Const::CompiledVTF::textureflags_clamps | Const::CompiledVTF::textureflags_clampt;
	if (depth == Const::MaterialRenderTargetDepth::RT_Only)
		textureFlags |= Const::CompiledVTF::textureflags_pointsample;

	int renderTargetFlags = bHDR ? 0x1 : 0;

	// NOTE: Refcount returned by CreateRenderTargetTexture is 1
	m_Texture = Interfaces::MatSys->CreateNamedRenderTargetTextureEx(pStrOptionalName, w, h, sizeMode, fmt,
		depth, textureFlags, renderTargetFlags);
}

//-----------------------------------------------------------------------------
// Detach from a texture
//-----------------------------------------------------------------------------
void ITextureReference::shutdown(bool bDeleteIfUnReferenced)
{
	if (m_Texture)
	{
		m_Texture->DecrementReferenceCount();
		if (bDeleteIfUnReferenced)
		{
			m_Texture->DeleteIfUnreferenced();
		}
		m_Texture = nullptr;
	}
}

TF2_NAMESPACE_END();

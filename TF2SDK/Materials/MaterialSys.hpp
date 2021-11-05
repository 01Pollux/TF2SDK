#pragma once

#include "Engine/AppSystem.hpp"

#include "MaterialCfg.hpp"
#include "VTF.hpp"

#include "Math/Matrix.hpp"
#include "Utils/RefCount.hpp"

TF2_NAMESPACE_BEGIN();

class IMaterialSystem;
namespace Interfaces
{
	TF2_EXPORT_INTERFACE(IMaterialSystem, MatSys, "VMaterialSystem081");
}

class IMaterialProxyFactory;
class KeyValues;
class IShader;
class IMaterial;
class ITexture; 
class IMorph;
class IMatRenderContext;
class ICallQueue;
class IFileList;
class ITextureCompositor;
class IAsyncTextureOperationReceiver;
class IMesh;
class IVertexBuffer;
class IIndexBuffer;
class DeformationBase_t;

struct MaterialSystem_Config_t;
struct MaterialAdapterInfo_t;
struct MaterialVideoMode_t;
struct MaterialSystem_SortInfo_t;
struct MaterialSystemHardwareIdentifier_t;
struct LightDesc_t;
struct FlashlightState_t;
struct MorphWeight_t;
struct Rect_t;


namespace Const
{
	namespace TextureGroupName
	{
		static constexpr const char* Lightmap				= "Lightmaps";
		static constexpr const char* World					= "World textures";
		static constexpr const char* Model					= "Model textures";
		static constexpr const char* VGui					= "VGUI textures";
		static constexpr const char* Particle				= "Particle textures";
		static constexpr const char* Decal					= "Decal textures";
		static constexpr const char* Skybox					= "SkyBox textures";
		static constexpr const char* ClientEffects			= "ClientEffect textures";
		static constexpr const char* Other					= "Other textures";
		static constexpr const char* Precached				= "Precached";
		static constexpr const char* CubeMap				= "CubeMap textures";
		static constexpr const char* RenderTarget			= "RenderTargets";
		static constexpr const char* RuntimeComposite		= "Runtime Composite";
		// Textures that weren't assigned a texture group.
		static constexpr const char* Unaccounted			= "Unaccounted textures";
		static constexpr const char* IndexBuffer			= "Static Indices";
		static constexpr const char* IndexBuffer_Disp		= "Displacement Verts";
		static constexpr const char* IndexBuffer_Color		= "Lighting Verts";
		static constexpr const char* IndexBuffer_World		= "World Verts";
		static constexpr const char* IndexBuffer_Models		= "Model Verts";
		static constexpr const char* IndexBuffer_Other		= "Other Verts";
		static constexpr const char* Dynamic_IndexBuffer	= "Dynamic Indices";
		static constexpr const char* Dynamic_VertedBuffer	= "Dynamic Verts";
		static constexpr const char* DepthBuffer			= "DepthBuffer";
		static constexpr const char* ViewModel				= "ViewModel";
		static constexpr const char* PixelShaders			= "Pixel Shaders";
		static constexpr const char* VertedSharders			= "Vertex Shaders";
		static constexpr const char* RenderTarget_Surface	= "RenderTarget Surfaces";
		static constexpr const char* Morth_Targets			= "Morph Targets";
	}
}


class IMaterialSystem : public IAppSystem
{
public:
	using MatHandle = uint16_t;

	// Placeholder for API revision
	virtual bool Connect(IAppSystem::CreateIFace factory) abstract;
	virtual void Disconnect() abstract;
	virtual void* QueryInterface(const char* pInterfaceName) abstract;
	virtual InitRetVal Init() abstract;
	virtual void Shutdown() abstract;

	//---------------------------------------------------------
	// Initialization and shutdown
	//---------------------------------------------------------

	// Call this to initialize the material system
	// returns a method to create interfaces in the shader dll
	virtual IAppSystem::CreateIFace	Init(char const* pShaderAPIDLL,
		IMaterialProxyFactory* pMaterialProxyFactory,
		IAppSystem::CreateIFace fileSystemFactory,
		IAppSystem::CreateIFace cvarFactory = nullptr) abstract;

	// Call this to set an explicit shader version to use 
	// Must be called before Init().
	virtual void				SetShaderAPI(char const* pShaderAPIDLL) abstract;

	// Must be called before Init(), if you're going to call it at all...
	virtual void				SetAdapter(int nAdapter, int nFlags) abstract;

	// Call this when the mod has been set up, which may occur after init
	// At this point, the game + gamebin paths have been set up
	virtual void				ModInit() abstract;
	virtual void				ModShutdown() abstract;

	//---------------------------------------------------------
	//
	//---------------------------------------------------------
	virtual void					SetThreadMode(Const::MaterialThreadMode mode, int nServiceThread = -1) abstract;
	virtual Const::MaterialThreadMode	GetThreadMode() abstract;
	virtual bool					IsRenderThreadSafe() abstract;
	virtual void					ExecuteQueued() abstract;

	//---------------------------------------------------------
	// Config management
	//---------------------------------------------------------

	virtual IMaterialSystemHardwareConfig* GetHardwareConfig(const char* pVersion, int* returnCode) abstract;


	// Call this before rendering each frame with the current config
	// for the material system.
	// Will do whatever is necessary to get the material system into the correct state
	// upon configuration change. .doesn't much else otherwise.
	virtual bool				UpdateConfig(bool bForceUpdate) abstract;

	// Force this to be the config; update all material system convars to match the state
	// return true if lightmaps need to be redownloaded
	virtual bool				OverrideConfig(const MaterialSystem_Config_t& config, bool bForceUpdate) abstract;

	// Get the current config for this video card (as last set by UpdateConfig)
	virtual const MaterialSystem_Config_t& GetCurrentConfigForVideoCard() const abstract;

	// Gets *recommended* configuration information associated with the display card, 
	// given a particular dx level to run under. 
	// Use dxlevel 0 to use the recommended dx level.
	// The function returns false if an invalid dxlevel was specified

	// UNDONE: To find out all convars affected by configuration, we'll need to change
	// the dxsupport.pl program to output all column headers into a single keyvalue block
	// and then we would read that in, and send it back to the client
	virtual bool				GetRecommendedConfigurationInfo(int nDXLevel, KeyValues* pKeyValues) abstract;


	// -----------------------------------------------------------
	// Device methods
	// -----------------------------------------------------------

	// Gets the number of adapters...
	virtual int					GetDisplayAdapterCount() const abstract;

	// Returns the current adapter in use
	virtual int					GetCurrentAdapter() const abstract;

	// Returns info about each adapter
	virtual void				GetDisplayAdapterInfo(int adapter, MaterialAdapterInfo_t& info) const abstract;

	// Returns the number of modes
	virtual int					GetModeCount(int adapter) const abstract;

	// Returns mode information..
	virtual void				GetModeInfo(int adapter, int mode, MaterialVideoMode_t& info) const abstract;

	virtual void				AddModeChangeCallBack(void (*func)()) abstract;

	// Returns the mode info for the current display device
	virtual void				GetDisplayMode(MaterialVideoMode_t& mode) const abstract;

	// Sets the mode...
	virtual bool				SetMode(void* hwnd, const MaterialSystem_Config_t& config) abstract;

	virtual bool				SupportsMSAAMode(int nMSAAMode) abstract;

	// FIXME: REMOVE! Get video card identitier
	virtual const MaterialSystemHardwareIdentifier_t& GetVideoCardIdentifier() const abstract;

	// Use this to spew information about the 3D layer 
	virtual void				SpewDriverInfo() const abstract;

	virtual void				GetDXLevelDefaults(uint32_t& max_dxlevel, uint32_t& recommended_dxlevel) abstract;

	// Get the image format of the back buffer. . useful when creating render targets, etc.
	virtual void				GetBackBufferDimensions(int& width, int& height) const abstract;
	virtual Const::ImageFormat	GetBackBufferFormat() const abstract;

	virtual bool				SupportsHDRMode(Const::HDRType nHDRModede) abstract;


	// -----------------------------------------------------------
	// Window methods
	// -----------------------------------------------------------

	// Creates/ destroys a child window
	virtual bool				AddView(void* hwnd) abstract;
	virtual void				RemoveView(void* hwnd) abstract;

	// Sets the view
	virtual void				SetView(void* hwnd) abstract;


	// -----------------------------------------------------------
	// Control flow
	// -----------------------------------------------------------

	virtual void				BeginFrame(float frameTime) abstract;
	virtual void				EndFrame() abstract;
	virtual void				Flush(bool flushHardware = false) abstract;

	/// FIXME: This stuff needs to be cleaned up and abstracted.
	// Stuff that gets exported to the launcher through the engine
	virtual void				SwapBuffers() abstract;

	// Flushes managed textures from the texture cacher
	virtual void				EvictManagedResources() abstract;

	virtual void				ReleaseResources() abstract;
	virtual void				ReacquireResources() abstract;


	// -----------------------------------------------------------
	// Device loss/restore
	// -----------------------------------------------------------

	// Installs a function to be called when we need to release vertex buffers + textures
	virtual void				AddReleaseFunc(void(*func)()) abstract;
	virtual void				RemoveReleaseFunc(void(*func)()) abstract;

	// Installs a function to be called when we need to restore vertex buffers
	virtual void				AddRestoreFunc(void(*func)(int flags)) abstract;
	virtual void				RemoveRestoreFunc(void(*func)(int flags)) abstract;

	// Release temporary HW memory...
	virtual void				ResetTempHWMemory(bool bExitingLevel = false) abstract;

	// For dealing with device lost in cases where SwapBuffers isn't called all the time (Hammer)
	virtual void				HandleDeviceLost() abstract;


	// -----------------------------------------------------------
	// Shaders
	// -----------------------------------------------------------

	// Used to iterate over all shaders for editing purposes
	// GetShaders returns the number of shaders it actually found
	virtual int					ShaderCount() const abstract;
	virtual int					GetShaders(int nFirstShader, int nMaxCount, IShader** ppShaderList) const abstract;

	// FIXME: Is there a better way of doing this?
	// Returns shader flag names for editors to be able to edit them
	virtual int					ShaderFlagCount() const abstract;
	virtual const char* ShaderFlagName(int nIndex) const abstract;

	// Gets the actual shader fallback for a particular shader
	virtual void				GetShaderFallback(const char* pShaderName, char* pFallbackShader, int nFallbackLength) abstract;


	// -----------------------------------------------------------
	// Material proxies
	// -----------------------------------------------------------

	virtual IMaterialProxyFactory* GetMaterialProxyFactory() abstract;

	// Sets the material proxy factory. Calling this causes all materials to be uncached.
	virtual void				SetMaterialProxyFactory(IMaterialProxyFactory* pFactory) abstract;


	// -----------------------------------------------------------
	// Editor mode
	// -----------------------------------------------------------

	// Used to enable editor materials. Must be called before Init.
	virtual void				EnableEditorMaterials() abstract;


	// -----------------------------------------------------------
	// Stub mode mode
	// -----------------------------------------------------------

	// Force it to ignore Draw calls.
	virtual void				SetInStubMode(bool bInStubMode) abstract;


	//---------------------------------------------------------
	// Debug support
	//---------------------------------------------------------

	virtual void				DebugPrintUsedMaterials(const char* pSearchSubString, bool bVerbose) abstract;
	virtual void				DebugPrintUsedTextures() abstract;

	virtual void				ToggleSuppressMaterial(char const* pMaterialName) abstract;
	virtual void				ToggleDebugMaterial(char const* pMaterialName) abstract;


	//---------------------------------------------------------
	// Misc features
	//---------------------------------------------------------
	//returns whether fast clipping is being used or not - needed to be exposed for better per-object clip behavior
	virtual bool				UsingFastClipping() abstract;

	virtual int					StencilBufferBits() abstract; //number of bits per pixel in the stencil buffer


	//---------------------------------------------------------
	// Material and texture management
	//---------------------------------------------------------

	// Stop attempting to stream in textures in response to usage.  Useful for phases such as loading or other explicit
	// operations that shouldn't take usage of textures as a signal to stream them in at full rez.
	virtual void				SuspendTextureStreaming() abstract;
	virtual void				ResumeTextureStreaming() abstract;

	// uncache all materials. .  good for forcing reload of materials.
	virtual void				UncacheAllMaterials() abstract;

	// Remove any materials from memory that aren't in use as determined
	// by the IMaterial's reference count.
	virtual void				UncacheUnusedMaterials(bool bRecomputeStateSnapshots = false) abstract;

	// Load any materials into memory that are to be used as determined
	// by the IMaterial's reference count.
	virtual void				CacheUsedMaterials() abstract;

	// Force all textures to be reloaded from disk.
	virtual void				ReloadTextures() abstract;

	// Reloads materials
	virtual void				ReloadMaterials(const char* pSubString = nullptr) abstract;

	// Create a procedural material. The keyvalues looks like a VMT file
	virtual IMaterial* CreateMaterial(const char* pMaterialName, KeyValues* pVMTKeyValues) abstract;

	// Find a material by name.
	// The name of a material is a full path to 
	// the vmt file starting from "hl2/materials" (or equivalent) without
	// a file extension.
	// eg. "dev/dev_bumptest" refers to somethign similar to:
	// "d:/hl2/hl2/materials/dev/dev_bumptest.vmt"
	//
	// Most of the texture groups for pTextureGroupName are listed in texture_group_names.h.
	// 
	// Note: if the material can't be found, this returns a checkerboard material. You can 
	// find out if you have that material by calling IMaterial::IsErrorMaterial().
	// (Or use the global IsErrorMaterial function, which checks if it's null too).
	virtual IMaterial* FindMaterial(char const* pMaterialName, const char* pTextureGroupName, bool complain = true, const char* pComplainPrefix = nullptr) abstract;

	// Query whether a material is loaded (eg, whether FindMaterial will be nonblocking)
	virtual bool				IsMaterialLoaded(char const* pMaterialName) abstract;

	//---------------------------------
	// This is the interface for knowing what materials are available
	// is to use the following functions to get a list of materials.  The
	// material names will have the full path to the material, and that is the 
	// only way that the directory structure of the materials will be seen through this
	// interface.
	// NOTE:  This is mostly for worldcraft to get a list of materials to put
	// in the "texture" browser.in Worldcraft
	virtual MatHandle	FirstMaterial() const abstract;

	// returns InvalidMaterial if there isn't another material.
	// WARNING: you must call GetNextMaterial until it returns NULL, 
	// otherwise there will be a memory leak.
	virtual MatHandle	NextMaterial(MatHandle h) const abstract;

	// This is the invalid material
	virtual MatHandle	InvalidMaterial() const abstract;

	// Returns a particular material
	virtual IMaterial* GetMaterial(MatHandle h) const abstract;

	// Get the total number of materials in the system.  These aren't just the used
	// materials, but the complete collection.
	virtual int					GetNumMaterials() const abstract;

	//---------------------------------

	virtual void				SetAsyncTextureLoadCache(void* hFileCache) abstract;

	virtual ITexture* FindTexture(char const* pTextureName, const char* pTextureGroupName, bool complain = true, int nAdditionalCreationFlags = 0) abstract;

	// Checks to see if a particular texture is loaded
	virtual bool				IsTextureLoaded(char const* pTextureName) const abstract;

	// Creates a procedural texture
	virtual ITexture* CreateProceduralTexture(const char* pTextureName,
		const char* pTextureGroupName,
		int w,
		int h,
		Const::ImageFormat fmt,
		int nFlags
	) abstract;

	//
	// Render targets
	//
	virtual void				BeginRenderTargetAllocation() abstract;
	virtual void				EndRenderTargetAllocation() abstract; // Simulate an Alt-Tab in here, which causes a release/restore of all resources

	// Creates a render target
	// If depth == true, a depth buffer is also allocated. If not, then
	// the screen's depth buffer is used.
	// Creates a texture for use as a render target
	virtual ITexture* CreateRenderTargetTexture(int w,
		int h,
		Const::RenderTargetSizeMode sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		Const::ImageFormat	format,
		Const::MaterialRenderTargetDepth depth = Const::MaterialRenderTargetDepth::Shared
	) abstract;

	virtual ITexture* CreateNamedRenderTargetTextureEx(const char* pRTName,				// Pass in NULL here for an unnamed render target.
		int w,
		int h,
		Const::RenderTargetSizeMode sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		Const::ImageFormat format,
		Const::MaterialRenderTargetDepth depth = Const::MaterialRenderTargetDepth::Shared,
		uint32_t textureFlags = Const::CompiledVTF::textureflags_clamps | Const::CompiledVTF::textureflags_clampt,
		uint32_t renderTargetFlags = 0
	) abstract;

	virtual ITexture* CreateNamedRenderTargetTexture(const char* pRTName,
		int w,
		int h,
		Const::RenderTargetSizeMode sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		Const::ImageFormat format,
		Const::MaterialRenderTargetDepth depth = Const::MaterialRenderTargetDepth::Shared,
		bool bClampTexCoords = true,
		bool bAutoMipMap = false
	) abstract;

	// Must be called between the above Begin-End calls!
	virtual ITexture* CreateNamedRenderTargetTextureEx2(const char* pRTName,				// Pass in NULL here for an unnamed render target.
		int w,
		int h,
		Const::RenderTargetSizeMode sizeMode,	// Controls how size is generated (and regenerated on video mode change).
		Const::ImageFormat format,
		Const::MaterialRenderTargetDepth depth = Const::MaterialRenderTargetDepth::Shared,
		uint32_t textureFlags = Const::CompiledVTF::textureflags_clamps | Const::CompiledVTF::textureflags_clampt,
		uint32_t renderTargetFlags = 0
	) abstract;

	// -----------------------------------------------------------
	// Lightmaps
	// -----------------------------------------------------------

	// To allocate lightmaps, sort the whole world by material twice.
	// The first time through, call AllocateLightmap for every surface.
	// that has a lightmap.
	// The second time through, call AllocateWhiteLightmap for every 
	// surface that expects to use shaders that expect lightmaps.
	virtual void				BeginLightmapAllocation() abstract;
	virtual void				EndLightmapAllocation() abstract;

	// returns the sorting id for this surface
	virtual int 				AllocateLightmap(int width, int height,
		int offsetIntoLightmapPage[2],
		IMaterial* pMaterial) abstract;
	// returns the sorting id for this surface
	virtual int					AllocateWhiteLightmap(IMaterial* pMaterial) abstract;

	// lightmaps are in linear color space
	// lightmapPageID is returned by GetLightmapPageIDForSortID
	// lightmapSize and offsetIntoLightmapPage are returned by AllocateLightmap.
	// You should never call UpdateLightmap for a lightmap allocated through
	// AllocateWhiteLightmap.
	virtual void				UpdateLightmap(int lightmapPageID, int lightmapSize[2],
		int offsetIntoLightmapPage[2],
		float* pFloatImage, float* pFloatImageBump1,
		float* pFloatImageBump2, float* pFloatImageBump3) abstract;

	// fixme: could just be an array of ints for lightmapPageIDs since the material
	// for a surface is already known.
	virtual int					GetNumSortIDs() abstract;
	virtual void				GetSortInfo(MaterialSystem_SortInfo_t* sortInfoArray) abstract;

	// Read the page size of an existing lightmap by sort id (returned from AllocateLightmap())
	virtual void				GetLightmapPageSize(int lightmap, int* width, int* height) const abstract;

	virtual void				ResetMaterialLightmapPageInfo() abstract;



	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) abstract;

	// -----------------------------------------------------------
	// Access the render contexts
	// -----------------------------------------------------------
	virtual IMatRenderContext* GetRenderContext() abstract;

	virtual bool				SupportsShadowDepthTextures() abstract;
	virtual void				BeginUpdateLightmaps() abstract;
	virtual void				EndUpdateLightmaps() abstract;

	// -----------------------------------------------------------
	// Methods to force the material system into non-threaded, non-queued mode
	// -----------------------------------------------------------
	virtual uint32_t			Lock() abstract;
	virtual void				Unlock(uint32_t) abstract;

	// Vendor-dependent shadow depth texture format
	virtual Const::ImageFormat	GetShadowDepthTextureFormat() abstract;

	virtual bool				SupportsFetch4() abstract;

	// Create a custom render context. Cannot be used to create MATERIAL_HARDWARE_CONTEXT
	virtual IMatRenderContext* CreateRenderContext(Const::MaterialContextType type) abstract;

	// Set a specified render context to be the global context for the thread. Returns the prior context.
	virtual IMatRenderContext* SetRenderContext(IMatRenderContext*) abstract;

	virtual bool				SupportsCSAAMode(int nNumSamples, int nQualityLevel) abstract;

	virtual void				RemoveModeChangeCallBack(void(*func)()) abstract;

	// Finds or create a procedural material.
	virtual IMaterial* FindProceduralMaterial(const char* pMaterialName, const char* pTextureGroupName, KeyValues* pVMTKeyValues) abstract;

	virtual Const::ImageFormat	GetNullTextureFormat() abstract;

	virtual void				AddTextureAlias(const char* pAlias, const char* pRealName) abstract;
	virtual void				RemoveTextureAlias(const char* pAlias) abstract;

	// returns a lightmap page ID for this allocation, -1 if none available
	// frameID is a number that should be changed every frame to prevent locking any textures that are
	// being used to draw in the previous frame
	virtual int					AllocateDynamicLightmap(int lightmapSize[2], int* pOutOffsetIntoPage, int frameID) abstract;

	virtual void				SetExcludedTextures(const char* pScriptName) abstract;
	virtual void				UpdateExcludedTextures() abstract;

	virtual bool				IsInFrame() const abstract;

	virtual void				CompactMemory() abstract;

	// For sv_pure mode. The filesystem figures out which files the client needs to reload to be "pure" ala the server's preferences.
	virtual void ReloadFilesInList(IFileList* pFilesToReload) abstract;
	virtual	bool				AllowThreading(bool bAllow, int nServiceThread) abstract;

	// Extended version of FindMaterial().
	// Contains context in so it can make decisions (i.e. if it's a model, ignore certain cheat parameters)
	virtual IMaterial* FindMaterialEx(char const* pMaterialName, const char* pTextureGroupName, int nContext, bool complain = true, const char* pComplainPrefix = nullptr) abstract;

#ifdef DX_TO_GL_ABSTRACTION
	virtual void				DoStartupShaderPreloading() abstract;
#endif	

	// Sets the override sizes for all render target size tests. These replace the frame buffer size.
	// Set them when you are rendering primarily to something larger than the frame buffer (as in VR mode).
	virtual void				SetRenderTargetFrameBufferSizeOverrides(int nWidth, int nHeight) abstract;

	// Returns the (possibly overridden) framebuffer size for render target sizing.
	virtual void				GetRenderTargetFrameBufferDimensions(int& nWidth, int& nHeight) abstract;

	// returns the display device name that matches the adapter index we were started with
	virtual char* GetDisplayDeviceName() const abstract;

	// creates a texture suitable for use with materials from a raw stream of bits.
	// The bits will be retained by the material system and can be freed upon return.
	virtual ITexture* CreateTextureFromBits(int w, int h, int mips, Const::ImageFormat fmt, int srcBufferSize, std::byte* srcBits) abstract;

	// Lie to the material system to pretend to be in render target allocation mode at the beginning of time.
	// This was a thing that mattered a lot to old hardware, but doesn't matter at all to new hardware,
	// where new is defined to be "anything from the last decade." However, we want to preserve legacy behavior
	// for the old games because it's easier than testing them.
	virtual void				OverrideRenderTargetAllocation(bool rtAlloc) abstract;

	// creates a texture compositor that will attempt to composite a new textuer from the steps of the specified KeyValues.
	virtual ITextureCompositor* NewTextureCompositor(int w, int h, const char* pCompositeName, int nTeamNum, uint64_t randomSeed, KeyValues* stageDesc, uint32_t texCompositeCreateFlags = 0) abstract;

	// Loads the texture with the specified name, calls pRecipient->OnAsyncFindComplete with the result from the main thread.
	// once the texture load is complete. If the texture cannot be found, the returned texture will return true for IsError().
	virtual void AsyncFindTexture(const char* pFilename, const char* pTextureGroupName, IAsyncTextureOperationReceiver* pRecipient, void* pExtraArgs, bool bComplain = true, int nAdditionalCreationFlags = 0) abstract;

	// creates a texture suitable for use with materials from a raw stream of bits.
	// The bits will be retained by the material system and can be freed upon return.
	virtual ITexture* CreateNamedTextureFromBitsEx(const char* pName, const char* pTextureGroupName, int w, int h, int mips, Const::ImageFormat fmt, int srcBufferSize, std::byte* srcBits, int nFlags) abstract;

	// Creates a texture compositor template for use in later code. 
	virtual bool				AddTextureCompositorTemplate(const char* pName, KeyValues* pTmplDesc, int nTexCompositeTemplateFlags = 0) abstract;

	// Performs final verification of all compositor templates (after they've all been initially loaded).
	virtual bool				VerifyTextureCompositorTemplates() abstract;
};


// Passed as the callback object to Async functions in the material system
// so that callers don't have to worry about memory going out of scope before the 
// results return.
class IAsyncTextureOperationReceiver : public Utils::IRefCounted
{
public:
	virtual void OnAsyncCreateComplete(ITexture * pTex, void* pExtraArgs) abstract;
	virtual void OnAsyncFindComplete(ITexture* pTex, void* pExtraArgs) abstract;
	virtual void OnAsyncMapComplete(ITexture* pTex, void* pExtraArgs, void* pMemory, int nPitch) abstract;
	virtual void OnAsyncReadbackBegin(ITexture* pDst, ITexture* pSrc, void* pExtraArgs) abstract;

	virtual int GetRefCount() const abstract;
};


class IMatRenderContext : public Utils::IRefCounted
{
public:
	virtual void				BeginRender() abstract;
	virtual void				EndRender() abstract;

	virtual void				Flush(bool flushHardware = false) abstract;

	virtual void				BindLocalCubemap(ITexture* pTexture) abstract;

	// pass in an ITexture (that is build with "rendertarget" "1") or
	// pass in NULL for the regular backbuffer.
	virtual void				SetRenderTarget(ITexture* pTexture) abstract;
	virtual ITexture*			GetRenderTarget() abstract;

	virtual void				GetRenderTargetDimensions(int& width, int& height) const abstract;

	// Bind a material is current for rendering.
	virtual void				Bind(IMaterial* material, void* proxyData = 0) abstract;
	// Bind a lightmap page current for rendering.  You only have to 
	// do this for materials that require lightmaps.
	virtual void				BindLightmapPage(int lightmapPageID) abstract;

	// inputs are between 0 and 1
	virtual void				DepthRange(float zNear, float zFar) abstract;

	virtual void				ClearBuffers(bool bClearColor, bool bClearDepth, bool bClearStencil = false) abstract;

	// read to a uint8_t rgb image.
	virtual void				ReadPixels(int x, int y, int width, int height, uint8_t* data, Const::ImageFormat dstFormat) abstract;

	// Sets lighting
	virtual void				SetAmbientLight(float r, float g, float b) abstract;
	virtual void				SetLight(int lightNum, const LightDesc_t& desc) abstract;

	// The faces of the cube are specified in the same order as cubemap textures
	virtual void				SetAmbientLightCube(Vector4D_F cube[6]) abstract;

	// Blit the backbuffer to the framebuffer texture
	virtual void				CopyRenderTargetToTexture(ITexture* pTexture) abstract;

	// Set the current texture that is a copy of the framebuffer.
	virtual void				SetFrameBufferCopyTexture(ITexture* pTexture, int textureIndex = 0) abstract;
	virtual ITexture*			GetFrameBufferCopyTexture(int textureIndex) abstract;

	//
	// end vertex array api
	//

	// matrix api
	virtual void				MatrixMode(Const::MaterialMatrixMode matrixMode) abstract;
	virtual void				PushMatrix() abstract;
	virtual void				PopMatrix() abstract;
	virtual void				LoadMatrix(const Matrix4x4_F& matrix) abstract;
	virtual void				LoadMatrix(const Matrix3x4_F& matrix) abstract;
	virtual void				MultMatrix(const Matrix4x4_F& matrix) abstract;
	virtual void				MultMatrix(const Matrix3x4_F& matrix) abstract;
	virtual void				MultMatrixLocal(const Matrix4x4_F& matrix) abstract;
	virtual void				MultMatrixLocal(const Matrix3x4_F& matrix) abstract;
	virtual void				GetMatrix(Const::MaterialMatrixMode matrixMode, Matrix4x4_F* matrix) abstract;
	virtual void				GetMatrix(Const::MaterialMatrixMode matrixMode, Matrix3x4_F* matrix) abstract;
	virtual void				LoadIdentity() abstract;
	virtual void				Ortho(double left, double top, double right, double bottom, double zNear, double zFar) abstract;
	virtual void				PerspectiveX(double fovx, double aspect, double zNear, double zFar) abstract;
	virtual void				PickMatrix(int x, int y, int width, int height) abstract;
	virtual void				Rotate(float angle, float x, float y, float z) abstract;
	virtual void				Translate(float x, float y, float z) abstract;
	virtual void				Scale(float x, float y, float z) abstract;
	// end matrix api

	// Sets/gets the viewport
	virtual void				Viewport(int x, int y, int width, int height) abstract;
	virtual void				GetViewport(int& x, int& y, int& width, int& height) const abstract;

	// The cull mode
	virtual void				CullMode(Const::MaterialCullMode cullMode) abstract;

	// end matrix api

	// This could easily be extended to a general user clip plane
	virtual void				SetHeightClipMode(Const::MaterialHeightClipMode nHeightClipMode) abstract;
	// garymcthack : fog z is always used for heightclipz for now.
	virtual void				SetHeightClipZ(float z) abstract;

	// Fog methods...
	virtual void				FogMode(Const::MaterialFogMode fogMode) abstract;
	virtual void				FogStart(float fStart) abstract;
	virtual void				FogEnd(float fEnd) abstract;
	virtual void				SetFogZ(float fogZ) abstract;
	virtual Const::MaterialFogMode	GetFogMode() abstract;

	virtual void				FogColor3f(float r, float g, float b) abstract;
	virtual void				FogColor3fv(float const* rgb) abstract;
	virtual void				FogColor3ub(uint8_t r, uint8_t g, uint8_t b) abstract;
	virtual void				FogColor3ubv(uint8_t const* rgb) abstract;

	virtual void				GetFogColor(uint8_t* rgb) abstract;

	// Sets the number of bones for skinning
	virtual void				SetNumBoneWeights(int numBones) abstract;

	// Creates/destroys Mesh
	virtual IMesh*				CreateStaticMesh(uint64_t vertex_format, const char* pTextureBudgetGroup, IMaterial* pMaterial = nullptr) abstract;
	virtual void				DestroyStaticMesh(IMesh* mesh) abstract;

	// Gets the dynamic mesh associated with the currently bound material
	// note that you've got to render the mesh before calling this function 
	// a second time. Clients should *not* call DestroyStaticMesh on the mesh 
	// returned by this call.
	// Use buffered = false if you want to not have the mesh be buffered,
	// but use it instead in the following pattern:
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		Draw partial
	//		Draw partial
	//		Draw partial
	//		meshBuilder.Begin
	//		meshBuilder.End
	//		etc
	// Use Vertex or Index Override to supply a static vertex or index buffer
	// to use in place of the dynamic buffers.
	//
	// If you pass in a material in pAutoBind, it will automatically bind the
	// material. This can be helpful since you must bind the material you're
	// going to use BEFORE calling GetDynamicMesh.
	virtual IMesh*				GetDynamicMesh(
		bool buffered = true,
		IMesh* pVertexOverride = 0,
		IMesh* pIndexOverride = 0,
		IMaterial* pAutoBind = 0
	) abstract;

	// ------------ New Vertex/Index Buffer interface ----------------------------
	// Do we need support for bForceTempMesh and bSoftwareVertexShader?
	// I don't think we use bSoftwareVertexShader anymore. .need to look into bForceTempMesh.
	virtual IVertexBuffer*		CreateStaticVertexBuffer(uint64_t vertex_format, int nVertexCount, const char* pTextureBudgetGroup) abstract;
	virtual IIndexBuffer*		CreateStaticIndexBuffer(Const::MaterialIndexFormat fmt, int nIndexCount, const char* pTextureBudgetGroup) abstract;
	virtual void				DestroyVertexBuffer(IVertexBuffer*) abstract;
	virtual void				DestroyIndexBuffer(IIndexBuffer*) abstract;
	// Do we need to specify the stream here in the case of locking multiple dynamic VBs on different streams?
	virtual IVertexBuffer*		GetDynamicVertexBuffer(int streamID, uint64_t vertex_format, bool bBuffered = true) abstract;
	virtual IIndexBuffer*		GetDynamicIndexBuffer(Const::MaterialIndexFormat fmt, bool bBuffered = true) abstract;
	virtual void				BindVertexBuffer(int streamID, IVertexBuffer* pVertexBuffer, int nOffsetInBytes, int nFirstVertex, int nVertexCount, uint64_t vertex_format, int nRepetitions = 1) abstract;
	virtual void				BindIndexBuffer(IIndexBuffer* pIndexBuffer, int nOffsetInBytes) abstract;
	virtual void				Draw(Const::MaterialPrimitiveType primitiveType, int firstIndex, int numIndices) abstract;
	// ------------ End ----------------------------

	// Selection mode methods
	virtual int					SelectionMode(bool selectionMode) abstract;
	virtual void				SelectionBuffer(unsigned int* pBuffer, int size) abstract;
	virtual void				ClearSelectionNames() abstract;
	virtual void				LoadSelectionName(int name) abstract;
	virtual void				PushSelectionName(int name) abstract;
	virtual void				PopSelectionName() abstract;

	// Sets the Clear Color for ClearBuffer....
	virtual void				ClearColor3ub(uint8_t r, uint8_t g, uint8_t b) abstract;
	virtual void				ClearColor4ub(uint8_t r, uint8_t g, uint8_t b, uint8_t a) abstract;

	// Allows us to override the depth buffer setting of a material
	virtual void				OverrideDepthEnable(bool bEnable, bool bDepthEnable) abstract;

	// FIXME: This is a hack required for NVidia/XBox, can they fix in drivers?
	virtual void				DrawScreenSpaceQuad(IMaterial* pMaterial) abstract;

	// For debugging and building recording files. This will stuff a token into the recording file,
	// then someone doing a playback can watch for the token.
	virtual void				SyncToken(const char* pToken) abstract;

	// FIXME: REMOVE THIS FUNCTION!
	// The only reason why it's not gone is because we're a week from ship when I found the bug in it
	// and everything's tuned to use it.
	// It's returning values which are 2x too big (it's returning sphere diameter x2)
	// Use ComputePixelDiameterOfSphere below in all new code instead.
	virtual float				ComputePixelWidthOfSphere(const Vector3D_F& origin, float flRadius) abstract;

	//
	// Occlusion query support
	//

	// Allocate and delete query objects.
	virtual void*				CreateOcclusionQueryObject() abstract;
	virtual void				DestroyOcclusionQueryObject(void*) abstract;

	// Bracket drawing with begin and end so that we can get counts next frame.
	virtual void				BeginOcclusionQueryDrawing(void*) abstract;
	virtual void				EndOcclusionQueryDrawing(void*) abstract;

	// Get the number of pixels rendered between begin and end on an earlier frame.
	// Calling this in the same frame is a huge perf hit!
	virtual int					OcclusionQuery_GetNumPixelsRendered(void*) abstract;

	virtual void				SetFlashlightMode(bool bEnable) abstract;

	virtual void				SetFlashlightState(const FlashlightState_t& state, const Matrix4x4_F& worldToTexture) abstract;

	// Gets the current height clip mode
	virtual Const::MaterialHeightClipMode GetHeightClipMode() abstract;

	// This returns the diameter of the sphere in pixels based on 
	// the current model, view, + projection matrices and viewport.
	virtual float				ComputePixelDiameterOfSphere(const Vector3D_F& vecAbsOrigin, float flRadius) abstract;

	// By default, the material system applies the VIEW and PROJECTION matrices	to the user clip
	// planes (which are specified in world space) to generate projection-space user clip planes
	// Occasionally (for the particle system in hl2, for example), we want to override that
	// behavior and explictly specify a ViewProj transform for user clip planes
	virtual void				EnableUserClipTransformOverride(bool bEnable) abstract;
	virtual void				UserClipTransform(const Matrix4x4_F& worldToView) abstract;

	virtual bool				GetFlashlightMode() const abstract;

	// Used to make the handle think it's never had a successful query before
	virtual void				ResetOcclusionQueryObject(void*) abstract;

	// FIXME: Remove
	virtual void				Unused3() {}

	// Creates/destroys morph data associated w/ a particular material
	virtual IMorph*				CreateMorph(uint32_t moth_format, const char* pDebugName) abstract;
	virtual void				DestroyMorph(IMorph* pMorph) abstract;

	// Binds the morph data for use in rendering
	virtual void				BindMorph(IMorph* pMorph) abstract;

	// Sets flexweights for rendering
	virtual void				SetFlexWeights(int nFirstWeight, int nCount, const MorphWeight_t* pWeights) abstract;

	// FIXME: Remove
	virtual void				Unused4() {};
	virtual void				Unused5() {};
	virtual void				Unused6() {};
	virtual void				Unused7() {};
	virtual void				Unused8() {};

	// Read w/ stretch to a host-memory buffer
	virtual void				ReadPixelsAndStretch(Rect_t* pSrcRect, Rect_t* pDstRect, uint8_t* pBuffer, Const::ImageFormat dstFormat, int nDstStride) abstract;

	// Gets the window size
	virtual void				GetWindowSize(int& width, int& height) const abstract;

	// This function performs a texture map from one texture map to the render destination, doing
	// all the necessary pixel/texel coordinate fix ups. fractional values can be used for the
	// src_texture coordinates to get linear sampling - integer values should produce 1:1 mappings
	// for non-scaled operations.
	virtual void				DrawScreenSpaceRectangle(
		IMaterial* pMaterial,
		int destx, int desty,
		int width, int height,
		float src_texture_x0, float src_texture_y0,			// which texel you want to appear at
		// destx/y
		float src_texture_x1, float src_texture_y1,			// which texel you want to appear at
		// destx+width-1, desty+height-1
		int src_texture_width, int src_texture_height,		// needed for fixup
		void* pClientRenderable = nullptr,
		int nXDice = 1,
		int nYDice = 1) abstract;

	virtual void				LoadBoneMatrix(int boneIndex, const Matrix3x4_F& matrix) abstract;

	// This version will push the current rendertarget + current viewport onto the stack
	virtual void				PushRenderTargetAndViewport() abstract;

	// This version will push a new rendertarget + a maximal viewport for that rendertarget onto the stack
	virtual void				PushRenderTargetAndViewport(ITexture* pTexture) abstract;

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void				PushRenderTargetAndViewport(ITexture* pTexture, int nViewX, int nViewY, int nViewW, int nViewH) abstract;

	// This version will push a new rendertarget + a specified viewport onto the stack
	virtual void				PushRenderTargetAndViewport(ITexture* pTexture, ITexture* pDepthTexture, int nViewX, int nViewY, int nViewW, int nViewH) abstract;

	// This will pop a rendertarget + viewport
	virtual void				PopRenderTargetAndViewport() abstract;

	// Binds a particular texture as the current lightmap
	virtual void				BindLightmapTexture(ITexture* pLightmapTexture) abstract;

	// Blit a subrect of the current render target to another texture
	virtual void				CopyRenderTargetToTextureEx(ITexture* pTexture, int nRenderTargetID, Rect_t* pSrcRect, Rect_t* pDstRect = nullptr) abstract;
	virtual void				CopyTextureToRenderTargetEx(int nRenderTargetID, ITexture* pTexture, Rect_t* pSrcRect, Rect_t* pDstRect = nullptr) abstract;

	// Special off-center perspective matrix for DoF, MSAA jitter and poster rendering
	virtual void				PerspectiveOffCenterX(double fovx, double aspect, double zNear, double zFar, double bottom, double top, double left, double right) abstract;

	// Rendering parameters control special drawing modes withing the material system, shader
	// system, shaders, and engine. renderparm.h has their definitions.
	virtual void				SetFloatRenderingParameter(int parm_number, float value) abstract;
	virtual void				SetIntRenderingParameter(int parm_number, int value) abstract;
	virtual void				SetVectorRenderingParameter(int parm_number, const Vector3D_F& value) abstract;

	// stencil buffer operations.
	virtual void				SetStencilEnable(bool onoff) abstract;
	virtual void				SetStencilFailOperation(Const::StencilOperation op) abstract;
	virtual void				SetStencilZFailOperation(Const::StencilOperation op) abstract;
	virtual void				SetStencilPassOperation(Const::StencilOperation op) abstract;
	virtual void				SetStencilCompareFunction(Const::StencilComparisonFunction cmpfn) abstract;
	virtual void				SetStencilReferenceValue(int ref) abstract;
	virtual void				SetStencilTestMask(uint32_t msk) abstract;
	virtual void				SetStencilWriteMask(uint32_t msk) abstract;
	virtual void				ClearStencilBufferRectangle(int xmin, int ymin, int xmax, int ymax,int value) abstract;

	virtual void				SetRenderTargetEx(int nRenderTargetID, ITexture* pTexture) abstract;

	// rendering clip planes, beware that only the most recently pushed plane will actually be used in a sizeable chunk of hardware configurations
	// and that changes to the clip planes mid-frame while UsingFastClipping() is true will result unresolvable depth inconsistencies
	virtual void				PushCustomClipPlane(const float* pPlane) abstract;
	virtual void				PopCustomClipPlane() abstract;

	// Returns the number of vertices + indices we can render using the dynamic mesh
	// Passing true in the second parameter will return the max # of vertices + indices
	// we can use before a flush is provoked and may return different values 
	// if called multiple times in succession. 
	// Passing false into the second parameter will return
	// the maximum possible vertices + indices that can be rendered in a single batch
	virtual void				GetMaxToRender(IMesh* pMesh, bool bMaxUntilFlush, int* pMaxVerts, int* pMaxIndices) abstract;

	// Returns the max possible vertices + indices to render in a single draw call
	virtual int					GetMaxVerticesToRender(IMaterial* pMaterial) abstract;
	virtual int					GetMaxIndicesToRender() abstract;
	virtual void				DisableAllLocalLights() abstract;
	virtual int					CompareMaterialCombos(IMaterial* pMaterial1, IMaterial* pMaterial2, int lightMapID1, int lightMapID2) abstract;

	virtual IMesh*				GetFlexMesh() abstract;

	virtual void				SetFlashlightStateEx(const FlashlightState_t& state, const Matrix4x4_F& worldToTexture, ITexture* pFlashlightDepthTexture) abstract;

	// Returns the currently bound local cubemap
	virtual ITexture*			GetLocalCubemap() abstract;

	// This is a version of clear buffers which will only clear the buffer at pixels which pass the stencil test
	virtual void				ClearBuffersObeyStencil(bool bClearColor, bool bClearDepth) abstract;

	//enables/disables all entered clipping planes, returns the input from the last time it was called.
	virtual bool				EnableClipping(bool bEnable) abstract;

	//get fog distances entered with FogStart(), FogEnd(), and SetFogZ()
	virtual void				GetFogDistances(float* fStart, float* fEnd, float* fFogZ) abstract;

	// Hooks for firing PIX events from outside the Material System...
	virtual void				BeginPIXEvent(unsigned long color, const char* szName) abstract;
	virtual void				EndPIXEvent() abstract;
	virtual void				SetPIXMarker(unsigned long color, const char* szName) abstract;

	// Batch API
	// from changelist 166623:
	// - replaced obtuse material system batch usage with an explicit and easier to thread API
	virtual void				BeginBatch(IMesh* pIndices) abstract;
	virtual void				BindBatch(IMesh* pVertices, IMaterial* pAutoBind = nullptr) abstract;
	virtual void				DrawBatch(int firstIndex, int numIndices) abstract;
	virtual void				EndBatch() abstract;

	// Raw access to the call queue, which can be NULL if not in a queued mode
	virtual ICallQueue*			GetCallQueue() abstract;

	// Returns the world-space camera position
	virtual void				GetWorldSpaceCameraPosition(Vector3D_F* pCameraPos) abstract;
	virtual void				GetWorldSpaceCameraVectors(Vector3D_F* pVecForward, Vector3D_F* pVecRight, Vector3D_F* pVecUp) abstract;

	// Tone mapping
	virtual void				ResetToneMappingScale(float monoscale) abstract; 			// set scale to monoscale instantly with no chasing
	virtual void				SetGoalToneMappingScale(float monoscale) abstract; 			// set scale to monoscale instantly with no chasing

	// call TurnOnToneMapping before drawing the 3d scene to get the proper interpolated brightness
	// value set.
	virtual void				TurnOnToneMapping() abstract;

	// Set a linear vector color scale for all 3D rendering.
	// A value of [1.0f, 1.0f, 1.0f] should match non-tone-mapped rendering.
	virtual void				SetToneMappingScaleLinear(const Vector3D_F& scale) abstract;

	virtual Vector3D_F			GetToneMappingScaleLinear() abstract;
	virtual void				SetShadowDepthBiasFactors(float fSlopeScaleDepthBias, float fDepthBias) abstract;

	// Apply stencil operations to every pixel on the screen without disturbing depth or color buffers
	virtual void				PerformFullScreenStencilOperation() abstract;

	// Sets lighting origin for the current model (needed to convert directional lights to points)
	virtual void				SetLightingOrigin(Vector3D_F vLightingOrigin) abstract;

	// Set scissor rect for rendering
	virtual void				SetScissorRect(const int nLeft, const int nTop, const int nRight, const int nBottom, const bool bEnableScissor) abstract;

	// Methods used to build the morph accumulator that is read from when HW morph<ing is enabled.
	virtual void				BeginMorphAccumulation() abstract;
	virtual void				EndMorphAccumulation() abstract;
	virtual void				AccumulateMorph(IMorph* pMorph, int nMorphCount, const MorphWeight_t* pWeights) abstract;

	virtual void				PushDeformation(const DeformationBase_t* Deformation) abstract;
	virtual void				PopDeformation() abstract;
	virtual int					GetNumActiveDeformations() const abstract;

	virtual bool				GetMorphAccumulatorTexCoord(Vector2D_F* pTexCoord, IMorph* pMorph, int nVertex) abstract;

	// Version of get dynamic mesh that specifies a specific vertex format
	virtual IMesh*				GetDynamicMeshEx(uint64_t vertexFormat, bool bBuffered = true,
		IMesh* pVertexOverride = 0,	IMesh* pIndexOverride = 0, IMaterial* pAutoBind = 0) abstract;

	virtual void				FogMaxDensity(float flMaxDensity) abstract;

	virtual IMaterial*			GetCurrentMaterial() abstract;
	virtual int					GetCurrentNumBones() const abstract;
	virtual void*				GetCurrentProxy() abstract;

	// Color correction related methods..
	// Client cannot call IColorCorrectionSystem directly because it is not thread-safe
	// FIXME: Make IColorCorrectionSystem threadsafe?
	virtual void				EnableColorCorrection(bool bEnable) abstract;
	virtual uint32_t			AddLookup(const char* pName) abstract;
	virtual bool				RemoveLookup(uint32_t handle) abstract;
	virtual void				LockLookup(uint32_t handle) abstract;
	virtual void				LoadLookup(uint32_t handle, const char* pLookupName) abstract;
	virtual void				UnlockLookup(uint32_t handle) abstract;
	virtual void				SetLookupWeight(uint32_t handle, float flWeight) abstract;
	virtual void				ResetLookupWeights() abstract;
	virtual void				SetResetable(uint32_t handle, bool bResetable) abstract;

	//There are some cases where it's simply not reasonable to update the full screen depth texture (mostly on PC).
	//Use this to mark it as invalid and use a dummy texture for depth reads.
	virtual void				SetFullScreenDepthTextureValidityFlag(bool bIsValid) abstract;

	// A special path used to tick the front buffer while loading on the 360
	virtual void				SetNonInteractivePacifierTexture(ITexture* pTexture, float flNormalizedX, float flNormalizedY, float flNormalizedSize) abstract;
	virtual void				SetNonInteractiveTempFullscreenBuffer(ITexture* pTexture, Const::MaterialNonInteractiveMode mode) abstract;
	virtual void				EnableNonInteractiveMode(Const::MaterialNonInteractiveMode mode) abstract;
	virtual void				RefreshFrontBufferNonInteractive() abstract;
	// Allocates temp render data. Renderdata goes out of scope at frame end in multicore
	// Renderdata goes out of scope after refcount goes to zero in singlecore.
	// Locking/unlocking increases + decreases refcount
	virtual void*				LockRenderData(int nSizeInBytes) abstract;
	virtual void				UnlockRenderData(void* pData) abstract;

	// Temp render data gets immediately freed after it's all unlocked in single core.
	// This prevents it from being freed
	virtual void				AddRefRenderData() abstract;
	virtual void				ReleaseRenderData() abstract;

	// Returns whether a pointer is render data. NOTE: passing NULL returns true
	virtual bool				IsRenderData(const void* pData) const abstract;
	virtual void				PrintfVA(char* fmt, va_list vargs) abstract;
	virtual void				Printf(const char* fmt, ...) abstract;
	virtual float				Knob(char* knobname, float* setvalue = nullptr) abstract;
	// Allows us to override the alpha write setting of a material
	virtual void				OverrideAlphaWriteEnable(bool bEnable, bool bAlphaWriteEnable) abstract;
	virtual void				OverrideColorWriteEnable(bool bOverrideEnable, bool bColorWriteEnable) abstract;

	virtual void				ClearBuffersObeyStencilEx(bool bClearColor, bool bClearAlpha, bool bClearDepth) abstract;

	// Create a texture from the specified src render target, then call pRecipient->OnAsyncCreateComplete from the main thread.
	// The texture will be created using the destination format, and will optionally have mipmaps generated.
	// In case of error, the provided callback function will be called with the error texture.
	virtual void				AsyncCreateTextureFromRenderTarget(ITexture* pSrcRt, const char* pDstName, Const::ImageFormat dstFmt, bool bGenMips, int nAdditionalCreationFlags, IAsyncTextureOperationReceiver* pRecipient, void* pExtraArgs) abstract;
};


template<bool _IsRendering = false>
class IAutoMatRenderContext
{
public:
	IAutoMatRenderContext() = default;

	IAutoMatRenderContext(IMatRenderContext* pCtx) : m_RenderCtx(pCtx)
	{
		if (pCtx)
		{
			pCtx->AddRef();
			if constexpr(_IsRendering)
				pCtx->BeginRender();
		}
	}

	~IAutoMatRenderContext()
	{
		if (m_RenderCtx)
		{
			if constexpr (_IsRendering)
				m_RenderCtx->EndRender();
			m_RenderCtx->Release();
		}
	}

	IAutoMatRenderContext(const IAutoMatRenderContext&) = delete;
	IAutoMatRenderContext(IAutoMatRenderContext&& other)
	{
		if (m_RenderCtx != other.m_RenderCtx)
		{
			m_RenderCtx = other.m_RenderCtx;
			other.m_RenderCtx = nullptr;
		}
	}

	IAutoMatRenderContext& operator=(const IAutoMatRenderContext&) = delete;
	IAutoMatRenderContext& operator=(IAutoMatRenderContext&& other)
	{
		if (m_RenderCtx != other.m_RenderCtx)
		{
			m_RenderCtx = other.m_RenderCtx;
			other.m_RenderCtx = nullptr;
		}
		return *this;
	}

private:
	IMatRenderContext* m_RenderCtx{ };
};

using IAutoMatRenderContextR = IAutoMatRenderContext<true>;


struct MaterialAdapterInfo_t
{
	char DriverName[512];
	uint32_t VendorID;
	uint32_t DeviceID;
	uint32_t SubSysID;
	uint32_t Revision;
	int DXSupportLevel;			// This is the *preferred* dx support level
	int MaxDXSupportLevel;
	uint32_t DriverVersionHigh;
	uint32_t DriverVersionLow;
};

struct MaterialVideoMode_t
{
	int Width;			// if width and height are 0 and you select 
	int Height;			// windowed mode, it'll use the window size
	Const::ImageFormat Format;	// use ImageFormats (ignored for windowed mode)
	int RefreshRate;		// 0 == default (ignored for windowed mode)
};

struct MaterialSystem_SortInfo_t
{
	IMaterial*	Material;
	int			LightmapPageID;
};

struct LightDesc_t
{
	Const::LightType	Type;
	Vector3D_F		Color;
	Vector3D_F		Position;
	Vector3D_F		Direction;
	float			Range;
	float			Falloff;
	float			Attenuation0;
	float			Attenuation1;
	float			Attenuation2;
	float			Theta;
	float			Phi;
	// These aren't used by DX8. . used for software lighting.
	float			ThetaDot;
	float			PhiDot;
	unsigned int	Flags;
};

TF2_NAMESPACE_END();

#pragma once

#include "TF2Config.hpp"

TF2_NAMESPACE_BEGIN(::Const);

namespace VertexFormat
{
	static constexpr uint32_t Unknown		= 0x0000;
	static constexpr uint32_t Position		= 0x0001;
	static constexpr uint32_t Normal		= 0x0002;
	static constexpr uint32_t Color			= 0x0004;
	static constexpr uint32_t Specular		= 0x0008;
	static constexpr uint32_t TangentS		= 0x0010;
	static constexpr uint32_t TangentT		= 0x0020;
	static constexpr uint32_t TangentSpace	= TangentS | TangentT;

	// Indicates we're using wrinkle
	static constexpr uint32_t Wrinkle		= 0x0040;
	// Indicates we're using bone indices
	static constexpr uint32_t BoneIndex		= 0x0080;
	// Indicates this is a vertex shader
	static constexpr uint32_t VertexSharder = 0x0100;
	// Indicates this format shouldn't be bloated to cache align it
// (only used for VertexUsage)
	static constexpr uint32_t Format		= 0x0200;
	// Indicates that compressed vertex elements are to be used (see also VertexCompressionType_t)
	static constexpr uint32_t Compressed	= 0x400;
	// Update this if you add or remove bits...
	static constexpr uint32_t LastBit		= 10;

	static constexpr uint32_t BoneWeightBit	= LastBit + 1;
	static constexpr uint32_t UserDataBit	= LastBit + 4;
	static constexpr uint32_t TexCoordBit	= LastBit + 7;

	static constexpr uint32_t FormatFieldMask	= 0xFF;
}


enum class VertexElement
{
	NONE = -1,

	// Deliberately explicitly numbered so it's a pain in the ass to change, so you read this:
	// #!#!#NOTE#!#!# update GetVertexElementSize, VertexElementToDeclType and
	//                CVBAllocTracker (elementTable) when you update this!
	Position = 0,
	Normal = 1,
	Color = 2,
	Specular = 3,
	Tangent_s = 4,
	Tangent_t = 5,
	Wrinkle = 6,
	Boneindex = 7,
	Boneweights1 = 8,
	Boneweights2 = 9,
	Boneweights3 = 10,
	Boneweights4 = 11,
	Userdata1 = 12,
	Userdata2 = 13,
	Userdata3 = 14,
	Userdata4 = 15,
	Texcoord1d_0 = 16,
	Texcoord1d_1 = 17,
	Texcoord1d_2 = 18,
	Texcoord1d_3 = 19,
	Texcoord1d_4 = 20,
	Texcoord1d_5 = 21,
	Texcoord1d_6 = 22,
	Texcoord1d_7 = 23,
	Texcoord2d_0 = 24,
	Texcoord2d_1 = 25,
	Texcoord2d_2 = 26,
	Texcoord2d_3 = 27,
	Texcoord2d_4 = 28,
	Texcoord2d_5 = 29,
	Texcoord2d_6 = 30,
	Texcoord2d_7 = 31,
	Texcoord3d_0 = 32,
	Texcoord3d_1 = 33,
	Texcoord3d_2 = 34,
	Texcoord3d_3 = 35,
	Texcoord3d_4 = 36,
	Texcoord3d_5 = 37,
	Texcoord3d_6 = 38,
	Texcoord3d_7 = 39,
	Texcoord4d_0 = 40,
	Texcoord4d_1 = 41,
	Texcoord4d_2 = 42,
	Texcoord4d_3 = 43,
	Texcoord4d_4 = 44,
	Texcoord4d_5 = 45,
	Texcoord4d_6 = 46,
	Texcoord4d_7 = 47,

	Count = 48
};


//-----------------------------------------------------------------------------
// Shader state flags can be read from the FLAGS materialvar
// Also can be read or written to with the Set/GetMaterialVarFlags() call
// Also make sure you add/remove a string associated with each flag below to CShaderSystem::ShaderStateString in ShaderSystem.cpp
//-----------------------------------------------------------------------------
namespace MaterialVarFlags
{
	static constexpr uint32_t debug						= (1 << 0);
	static constexpr uint32_t no_debug_override			= (1 << 1);
	static constexpr uint32_t no_draw					= (1 << 2);
	static constexpr uint32_t use_in_fillrate_mode		= (1 << 3);

	static constexpr uint32_t vertexcolor				= (1 << 4);
	static constexpr uint32_t vertexalpha				= (1 << 5);
	static constexpr uint32_t selfillum					= (1 << 6);
	static constexpr uint32_t additive					= (1 << 7);
	static constexpr uint32_t alphatest					= (1 << 8);
	static constexpr uint32_t multipass					= (1 << 9);
	static constexpr uint32_t zneare					= (1 << 10);
	static constexpr uint32_t model						= (1 << 11);
	static constexpr uint32_t flat						= (1 << 12);
	static constexpr uint32_t nocull					= (1 << 13);
	static constexpr uint32_t nofog						= (1 << 14);
	static constexpr uint32_t ignorez					= (1 << 15);
	static constexpr uint32_t decal						= (1 << 16);
	static constexpr uint32_t envmapsphere				= (1 << 17);
	static constexpr uint32_t noalphamod				= (1 << 18);
	static constexpr uint32_t envmapcameraspace			= (1 << 19);
	static constexpr uint32_t basealphaenvmapmask		= (1 << 20);
	static constexpr uint32_t translucent				= (1 << 21);
	static constexpr uint32_t normalmapalphaenvmapmask	= (1 << 22);
	static constexpr uint32_t needs_software_skinning	= (1 << 23);
	static constexpr uint32_t opaquetexture				= (1 << 24);
	static constexpr uint32_t envmapmode				= (1 << 25);
	static constexpr uint32_t suppress_decals			= (1 << 26);
	static constexpr uint32_t halflambert				= (1 << 27);
	static constexpr uint32_t wireframe					= (1 << 28);
	static constexpr uint32_t allowalphatocoverage		= (1 << 29);
	static constexpr uint32_t ignore_alpha_modulation	= (1 << 30);

	// NOTE: Only add flags here that either should be read from
	// .vmts or can be set directly from client code. Other, internal
	// flags should to into the flag enum in imaterialinternal.h
};


//-----------------------------------------------------------------------------
// Internal flags not accessible from outside the material system. Stored in Flags2
//-----------------------------------------------------------------------------
namespace MaterialVarFlags2
{
	static constexpr uint32_t lighting_unlit							= 0;
	static constexpr uint32_t lighting_vertex_lit						= (1 << 1);
	static constexpr uint32_t lighting_lightmap							= (1 << 2);
	static constexpr uint32_t lighting_bumped_lightmap					= (1 << 3);
	static constexpr uint32_t lighting_mask								= Lighting_vertex_lit | Lighting_lightmap | Lighting_bumped_lightmap;

	// Fixme: Should This Be A Part Of The Above Lighting Enums?
	static constexpr uint32_t diffuse_bumpmapped_model					= (1 << 4);
	static constexpr uint32_t uses_env_cubemap							= (1 << 5);
	static constexpr uint32_t needs_tangent_spaces						= (1 << 6);
	static constexpr uint32_t needs_software_lighting					= (1 << 7);

	// Gr - Hdr Path Puts Lightmap Alpha In Separate Texture...
	static constexpr uint32_t blend_with_lightmap_alpha					= (1 << 8);
	static constexpr uint32_t needs_baked_lighting_snapshots			= (1 << 9);
	static constexpr uint32_t use_flashlight							= (1 << 10);
	static constexpr uint32_t use_fixed_function_baked_lighting			= (1 << 11);
	static constexpr uint32_t needs_fixed_function_flashlight			= (1 << 12);
	static constexpr uint32_t use_editor								= (1 << 13);
	static constexpr uint32_t needs_power_of_two_frame_buffer_texture	= (1 << 14);
	static constexpr uint32_t needs_full_frame_buffer_texture			= (1 << 15);
	static constexpr uint32_t is_spritecard								= (1 << 16);
	static constexpr uint32_t uses_vertexid								= (1 << 17);
	static constexpr uint32_t supports_hw_skinning						= (1 << 18);
	static constexpr uint32_t supports_flashlight						= (1 << 19);
};


//-----------------------------------------------------------------------------
// Preview image return values
//-----------------------------------------------------------------------------
enum class PreviewImageRetVal
{
	Bad,
	Ok,
	NoPreview
};


enum class MaterialPropertyTypes
{
	// bool
	NeedLightmap = 0,					
	// int (enum MaterialPropertyOpacityTypes_t)
	Opacity,								
	// vec3_t
	Reflectivity,						
	// bool
	NeedsBumpedLightmap				
};


enum class ImageFormat
{
	UNKNOWN = -1,
	RGBA8888 = 0,
	ABGR8888,
	RGB888,
	BGR888,
	RGB565,
	I8,
	IA88,
	P8,
	A8,
	RGB888_BLUESCREEN,
	BGR888_BLUESCREEN,
	ARGB8888,
	BGRA8888,
	DXT1,
	DXT3,
	DXT5,
	BGRX8888,
	BGR565,
	BGRX5551,
	BGRA4444,
	DXT1_ONEBITALPHA,
	BGRA5551,
	UV88,
	UVWQ8888,
	RGBA16161616F,
	RGBA16161616,
	UVLX8888,
	R32F,			// Single-channel 32-bit floating point
	RGB323232F,
	RGBA32323232F,

	// Depth-stencil texture formats for shadow depth mapping
	NV_DST16,		// 
	NV_DST24,		//
	NV_INTZ,		// Vendor-specific depth-stencil texture
	NV_RAWZ,		// formats for shadow depth mapping 
	ATI_DST16,		// 
	ATI_DST24,		//
	NV_NULL,		// Dummy format which takes no video memory

	// Compressed normal map formats
	ATI2N,			// One-surface ATI2N / DXN format
	ATI1N,			// Two-surface ATI1N format

	DXT1_RUNTIME,
	DXT5_RUNTIME,

	Count
};

enum class MaterialThreadMode
{
	Single,
	QueuedSingle,
	QueuedMulti
};

enum class MaterialContextType
{
	Hardware,
	Queued,
	Null
};


// NOTE: All size modes will force the render target to be smaller than or equal to
// the size of the framebuffer.
enum class RenderTargetSizeMode
{
	NoChange = 0,			// Only allowed for render targets that don't want a depth buffer
	// (because if they have a depth buffer, the render target must be less than or equal to the size of the framebuffer).
	Default = 1,				// Don't play with the specified width and height other than making sure it fits in the framebuffer.
	PicMip = 2,				// Apply picmip to the render target's width and height.
	HDR = 3,					// frame_buffer_width / 4
	FullFrameBuffer = 4,	// Same size as frame buffer, or next lower power of 2 if we can't do that.
	OffScreen = 5,			// Target of specified size, don't mess with dimensions
	FullFrameRoundedUp = 6, // Same size as the frame buffer, rounded up if necessary for systems that can't do non-power of two textures.
	ReplayScreenshot = 7,	// Rounded down to power of 2, essentially...
	Literal = 8,			// Use the size passed in. Don't clamp it to the frame buffer size. Really.
	LiteralPicMip = 9		// Use the size passed in, don't clamp to the frame buffer size, but do apply picmip restrictions.

};

//-----------------------------------------------------------------------------
// Flags to specify type of depth buffer used with RT
//-----------------------------------------------------------------------------
// GR - this is to add RT with no depth buffer bound
enum class MaterialRenderTargetDepth
{
	Shared = 0x0,
	Seperate = 0x1,
	None = 0x2,
	RT_Only = 0x3,
};


enum class ShaderParamType
{
	Texture,
	Int,
	Color,
	Vec2,
	Vec3,
	Vec4,
	EnvMap,
	Float,
	Bool,
	FourCC,
	Matrix,
	Material,
	String,
	Matrix4x2
};


enum class MaterialMatrixMode
{
	View,
	Projection,

	// Texture matrices
	Texture0,
	Texture1,
	Texture2,
	Texture3,
	Texture4,
	Texture5,
	Texture6,
	Texture7,

	Model,

	// Total number of matrices
	NumModes = Model + 1,
	// Number of texture transforms
	Transforms = Texture7 - Texture0 + 1
};


enum class LightType
{
	Disable,
	Point,
	Directional,
	Spot
};


enum class MaterialPrimitiveType
{
	Points,
	Lines,
	Triangles,
	Triangles_Strip,
	// a single line loop
	Line_Strip,
	// this is a *single* polygon
	Line_Loop,
	Polygon,
	Quads,
	Instanced_Quads,

	// This is used for static meshes that contain multiple types of
	// primitive types.	When calling draw, you'll need to specify
	// a primitive type.
	Heterogenous
};


// acceptable property values for MATERIAL_PROPERTY_OPACITY
enum class MaterialPropertyOpacityTypes
{
	AlphaTest,
	Opaque,
	Translucent
};


enum class MaterialBufferTypes
{
	Font,
	Back
};


enum class MaterialCullMode
{
	// this culls polygons with counterclockwise winding
	CCW,
	// this culls polygons with clockwise winding
	CW
};


enum class MaterialIndexFormat
{
	Unknown,
	Bit16,
	Bit32
};


enum class MaterialFogMode
{
	None, 
	Linear,
	BelowFogZ
};


enum class MaterialHeightClipMode
{
	Disable,
	Render_AboveHeight,
	Render_BelowHeight
};


enum class MaterialNonInteractiveMode
{
	None = -1,
	Startup,
	LevelLoad,

	Count
};


namespace MorphFormatFlags
{
	static constexpr uint32_t Position	= 0x0001,	// 3D
	static constexpr uint32_t Normal	= 0x0002,	// 3D
	static constexpr uint32_t Wrinkle	= 0x0004,	// 1D
	static constexpr uint32_t Speed		= 0x0008,	// 1D
	static constexpr uint32_t Side		= 0x0010,	// 1D
};


enum class StencilOperation
{
	Keep = 1,
	Zero,
	Replace,
	Incrast,
	Decrast,
	Invert,
	Increment,
	Decrement,
	
	ForceDword = 0x7fffffff
};

enum class StencilComparisonFunction
{
	Never = 1,
	Less,
	Equal,
	LessEqual,
	Greater,
	NotEqual,
	GreaterEqual,
	Always,

	ForceDword  = 0x7fffffff
};




TF2_NAMESPACE_END();

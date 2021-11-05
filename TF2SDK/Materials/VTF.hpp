#pragma once

#include "TF2Config.hpp"

TF2_NAMESPACE_BEGIN(::Const);

// TODO : https://github.com/OthmanAba/TeamFortress2/blob/1b81dded673d49adebf4d0958e52236ecc28a956/tf2_src/public/vtf/vtf.h#L37

namespace CompiledVTF
{
	static constexpr uint32_t textureflags_pointsample = 0x00000001;
	static constexpr uint32_t textureflags_trilinear = 0x00000002;
	static constexpr uint32_t textureflags_clamps = 0x00000004;
	static constexpr uint32_t textureflags_clampt = 0x00000008;
	static constexpr uint32_t textureflags_anisotropic = 0x00000010;
	static constexpr uint32_t textureflags_hint_dxt5 = 0x00000020;
	static constexpr uint32_t textureflags_srgb = 0x00000040;
	static constexpr uint32_t textureflags_normal = 0x00000080;
	static constexpr uint32_t textureflags_nomip = 0x00000100;
	static constexpr uint32_t textureflags_nolod = 0x00000200;
	static constexpr uint32_t textureflags_all_mips = 0x00000400;
	static constexpr uint32_t textureflags_procedural = 0x00000800;

	//TheseAreAutomaticallyGeneratedByVtexFromTheTextureData.
	static constexpr uint32_t textureflags_onebitalpha = 0x00001000;
	static constexpr uint32_t textureflags_eightbitalpha = 0x00002000;

	 //NewerFlagsFromThe*.txtConfigFile
	static constexpr uint32_t textureflags_envmap = 0x00004000;
	static constexpr uint32_t textureflags_rendertarget = 0x00008000;
	static constexpr uint32_t textureflags_depthrendertarget = 0x00010000;
	static constexpr uint32_t textureflags_nodebugoverride = 0x00020000;
	static constexpr uint32_t textureflags_singlecopy = 0x00040000;

	static constexpr uint32_t textureflags_staging_memory = 0x00080000;
	static constexpr uint32_t textureflags_immediate_cleanup = 0x00100000;
	static constexpr uint32_t textureflags_ignore_picmip = 0x00200000;

	static constexpr uint32_t textureflags_unused_00400000 = 0x00400000;
	static constexpr uint32_t textureflags_nodepthbuffer = 0x00800000;

	static constexpr uint32_t textureflags_unused_01000000 = 0x01000000;

	static constexpr uint32_t textureflags_clampu = 0x02000000;

	static constexpr uint32_t textureflags_vertextexture = 0x04000000;					//UseableAsAVertexTexture

	static constexpr uint32_t textureflags_ssbump = 0x08000000;

	static constexpr uint32_t textureflags_unused_10000000 = 0x10000000;

	//ClampToBorderColorOnAllTextureCoordinates
	static constexpr uint32_t textureflags_border = 0x20000000;

	static constexpr uint32_t textureflags_streamable_coarse = 0x40000000;
	static constexpr uint32_t textureflags_streamable_fine = 0x80000000;
	static constexpr uint32_t textureflags_streamable = (textureflags_streamable_coarse | textureflags_streamable_fine);
}

TF2_NAMESPACE_END();
#pragma once

#include "Consts.hpp"
#include "BSP.hpp"
#include <tf2/utils/UtlString.hpp>

TF2_NAMESPACE_BEGIN();

class ITexture;
class CEngineSprite;
class IMaterial;
class IMaterialVar;

struct studiomeshdata_t;
struct decal_t;
struct msurface1_t;
struct msurfacelighting_t;
struct msurfacenormal_t;

// !!! if this is changed, it must be changed in asm_draw.h too !!!
using mvertex_t = Vector3D_F;

// !!! if this is changed, it must be changed in asm_draw.h too !!!
using medge_t = unsigned short[2];

struct mtexinfo_t
{
	Vector4D_F	TextureVecsTexelsPerWorldUnits[2];	// [s/t] unit Vector3D_Fs in world space. 
													// [i][3] is the s/t offset relative to the origin.
	Vector4D_F	LightmapVecsLuxelsPerWorldUnits[2];
	float		LuxelsPerWorldUnit;
	float		WorldUnitsPerLuxel;
	unsigned short Flags;		// SURF_ flags.
	unsigned short TexinfoFlags;// TEXINFO_ flags.
	IMaterial*	Material;
};

struct mnode_t
{
	// common with leaf
	int			Contents;		// <0 to differentiate from leafs
	// -1 means check the node for visibility
	// -2 means don't check the node for visibility

	int			Visframe;		// node needs to be traversed if current

	mnode_t*	Parent;
	short		Area;			// If all leaves below this node are in the same area, then
	// this is the area index. If not, this is -1.
	short		Flags;

	Vector4D_F alignas(16) Center;
	Vector4D_F alignas(16) HalfDiagonal;

	// node specific
	cplane_t*	Plane;
	mnode_t*	Children[2];

	unsigned short		firstsurface;
	unsigned short		numsurfaces;
};


struct mleaf_t
{
public:

	// common with node
	int			Contents;		// contents mask
	int			Visframe;		// node needs to be traversed if current

	mnode_t*	Parent;

	short						Area;
	short						Flags;
	Vector4D_F alignas(16)		Center;
	Vector4D_F alignas(16)		HalfDiagonal;


	// leaf specific
	short		Cluster;
	short		LeafWaterDataID;

	unsigned short		FirstMarksurface;
	unsigned short		NumMarkSurfaces;

	short		NumMarkNodeSurfaces;
	short		_Unused;

	unsigned short	DispListStart;			// index into displist of first displacement
	unsigned short	DispCount;				// number of displacements in the list for this leaf
};


struct mleafwaterdata_t
{
	float	SurfaceZ;
	float	MinZ;
	short	SurfaceTexInfoID;
	short	FirstLeafIndex;
};


struct mcubemapsample_t
{
	Vector3D_F Origin;
	ITexture*	Texture;
	unsigned char Size; // default (mat_envmaptgasize) if 0, 1<<(size-1) otherwise.
};


struct mportal_t
{
	unsigned short* VertList;
	int				NumPortalVerts;
	cplane_t*		Plane;
	unsigned short	Cluster[2];
	//	int				visframe;
};


struct mcluster_t
{
	unsigned short* PortalList;
	int				Numportals;
};


struct mmodel_t
{
	Vector3D_F		Mins, Maxs;
	Vector3D_F		Origin;		// for sounds or lights
	float			Radius;
	int				HeadNode;
	int				FirstFace, NumFaces;
};

struct mprimitive_t
{
	int				Type;
	unsigned short	FirstIndex;
	unsigned short	IndexCount;
	unsigned short	FirstVert;
	unsigned short	VertCount;
};

struct mprimvert_t
{
	Vector3D_F	Pos;
	float		TexCoord[2];
	float		LightCoord[2];
};

struct mleafambientlighting_t
{
	struct ColorRGBExp32
	{
		std::byte r, g, b;
		signed char Exponent;
	};
	ColorRGBExp32	Cube[6];
	std::byte x;	// fixed point fraction of leaf bounds
	std::byte y;	// fixed point fraction of leaf bounds
	std::byte z;	// fixed point fraction of leaf bounds
	std::byte _pad;	// unused
};

struct mleafambientindex_t
{
	unsigned short AmbientSampleCount;
	unsigned short FirstAmbientSample;
};

struct LightShadowZBufferSample_t
{
	float TraceDistance;								// how far we traced. 0 = invalid
	float HitDistance;									// where we hit
};

#define SHADOW_ZBUF_RES 8									// 6 * 64 * 2 * 4 = 3k bytes per light

struct lightzbuffer_t
{
	LightShadowZBufferSample_t CubeMap[6][SHADOW_ZBUF_RES][SHADOW_ZBUF_RES];
};

struct worldbrushdata_t
{
	int			Numsubmodels;

	int			Numplanes;
	cplane_t*	Planes;

	int			Numleafs;		// number of visible leafs, not counting 0
	mleaf_t*	Leafs;

	int			NumLeafWaterData;
	mleafwaterdata_t* LeafWaterData;

	int			NumVertexes;
	mvertex_t*	Vertexes;

	int			NumOccluders;
	doccluderdata_t* Occluders;

	int			NumOccluderPolys;
	doccluderpolydata_t* OccluderPolys;

	int			NumoccludervertIndices;
	int*		OccludervertIndices;

	int				NumVertNormalIndices;	// These index vertnormals.
	unsigned short* VertNormalIndices;

	int			NumVertNormals;
	Vector3D_F* VertNormals;

	int			NumNodes;
	mnode_t*	Nodes;
	unsigned short* LeafMinDistToWater;

	int			NumTexInfo;
	mtexinfo_t* TexInfo;

	int			NumTexData;
	csurface_t* TexData;

	int         NumDispInfos;
	void*		DispInfos;	// Use DispInfo_Index to get IDispInfos..

	/*
	int         numOrigSurfaces;
	msurface_t  *pOrigSurfaces;
	*/

	int					NumSurfaces;
	msurface1_t*		Surfaces1;
	msurface2_t*		Surfaces2;
	msurfacelighting_t* SurfaceLighting;
	msurfacenormal_t*	SurfaceNormals;

	bool		UnloadedLightmaps;

	int			NumVertIndices;
	unsigned short* VertIndices;

	int				NumMarkSurfaces;
	msurface2_t*	MarkSurfaces;

	mleafambientlighting_t::ColorRGBExp32* LightData;

	int				NumWorldLights;
	dworldlight_t*	WorldLights;

	lightzbuffer_t* ShadowZBuffers;

	// non-polygon primitives (strips and lists)
	int				NumPrimitives;
	mprimitive_t*	Primitives;

	int			NumPrimVerts;
	mprimvert_t* PrimVerts;

	int			NumPrimIndices;
	unsigned short* PrimIndices;

	int				NumAreas;
	darea_t*		Areas;

	int				NumAreaPortals;
	dareaportal_t*	AreaPortals;

	int				NumClipPortalVerts;
	Vector3D_F*		ClipPortalVerts;

	mcubemapsample_t*	NumCubemapSamples;
	int					CubemapSamples;

	int				NumDispInfoReferences;
	unsigned short* DispInfoReferences;

	mleafambientindex_t*	LeafAmbient;
	mleafambientlighting_t* AmbientSamples;
};


// only models with type "mod_brush" have this data
struct brushdata_t
{
	worldbrushdata_t* Shared;
	int				FirstModelSurface, NumModelSurfaces;

	unsigned short	RenderHandle;
	unsigned short	FirstNode;
};


// only models with type "mod_sprite" have this data
struct spritedata_t
{
	int				Numframes;
	int				Width;
	int				Height;
	CEngineSprite*	Sprite;
};


struct ModelInfo
{
	void*				FileHandle;
	utils::UtlString	Name;

	int					LoadFlags;		// mark loaded/not loaded
	int					ServerCount;	// marked at load
	IMaterial**			pMaterials;	// null-terminated runtime material cache; ((intptr_t*)(ppMaterials))[-1] == nMaterials

	modtype_t			Type;
	int					Flags;			// MODELFLAG_???

	// volume occupied by the model graphics	
	Vector3D_F			Mins, Maxs;
	float				Radius;

	union
	{
		brushdata_t		Brush;
		unsigned short	Studio;
		spritedata_t	Sprite;
	};
};


TF2_NAMESPACE_END();
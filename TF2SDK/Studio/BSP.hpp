#pragma once

#include "Math/Vector.hpp"

TF2_NAMESPACE_BEGIN();

class CPhysCollide;
class IDispInfo;
class SurfInfo;

struct GameRay;
struct ModelInfo;
struct RayDispOutput_t;


// lights that were used to illuminate the world
enum class emittype_t
{
	Surface,		// 90 degree spotlight
	Point,			// simple point light source
	Spotlight,		// spotlight with penumbra
	Skylight,		// directional light with no falloff (surface must trace to SKY texture)
	Quakelight,	// linear falloff, non-lambertian
	Skyambient,	// spherical light source with no falloff (surface must trace to SKY texture)
};

struct doccluderdata_t
{
	int			Flags;
	int			Firstpoly;				// index into doccluderpolys
	int			Polycount;
	Vector3D_F	Mins;
	Vector3D_F	Maxs;
	int			Area;
};

struct doccluderdataV1_t
{
	int			Flags;
	int			Firstpoly;				// index into doccluderpolys
	int			Polycount;
	Vector3D_F	Mins;
	Vector3D_F	Maxs;
};

struct doccluderpolydata_t
{
	int			FirstVertexIndex;		// index into doccludervertindices
	int			Vertexcount;
	int			PlaneNum;
};



struct vcollide_t
{
	unsigned short	SolidCount : 15;
	unsigned short	IsPacked : 1;
	unsigned short	DescSize;
	// VPhysicsSolids
	CPhysCollide**	Solids;
	char*			KeyValues;
};

struct cmodel_t
{
	Vector3D_F	Mins, Maxs;
	Vector3D_F	Origin;		// for sounds or lights
	int			Headnode;

	vcollide_t	VCollisionData;
};

struct csurface_t
{
	const char* Name;
	short		SurfaceProps;
	unsigned short	Flags;		// BUGBUG: These are declared per surface, not per material, but this database is per-material now
};



enum class modtype_t
{
	Bad,
	Brush,
	Sprite,
	Studio
};

struct cplane_t
{
	Vector3D_F	Normal;
	float		Dist;
	std::byte	Type;			// for fast side tests
	std::byte	Signbits;		// signx + (signy<<1) + (signz<<1)
	std::byte	Pad[2];
};


#pragma pack(1)
// NOTE: 32-bytes.  Aligned/indexed often
struct msurface2_t
{
	unsigned int			Flags;			// see SURFDRAW_ #defines (only 22-bits right now)
	// These are packed in to flags now
	//unsigned char			vertCount;		// number of verts for this surface
	//unsigned char			sortGroup;		// only uses 2 bits, subdivide?
	cplane_t*				Plane;			// pointer to shared plane	
	int						Firstvertindex;	// look up in model->vertindices[] (only uses 17-18 bits?)
	unsigned short 			Decals;
	unsigned short			ShadowDecals; // unsigned short
	unsigned short			FirstOverlayFragment;	// First overlay fragment on the surface (short)
	short					MaterialSortID;
	unsigned short			VertBufferIndex;

	unsigned short			DynamicShadowsEnabled : 1;	// Can this surface receive dynamic shadows?
	unsigned short			Texinfo : 15;

	IDispInfo*				DispInfo;         // displacement map information
	int						VisFrame;		// should be drawn when node is crossed
};
#pragma pack()

// NOTE: 16-bytes, preserve size/alignment - we index this alot
struct msurface1_t
{
	// garymct - are these needed? - used by decal projection code
	int		TextureMins[2];		// smallest unnormalized s/t position on the surface.
	short	TextureExtents[2];	// ?? s/t texture size, 1..512 for all non-sky surfaces

	unsigned short NumPrims;
	unsigned short FirstPrimID;			// index into primitive list if numPrims > 0
};


// each area has a list of portals that lead into other areas
// when portals are closed, other areas may not be visible or
// hearable even if the vis info says that it should be
struct dareaportal_t
{
	unsigned short	PortalKey;		// Entities have a key called portalnumber (and in vbsp a variable
									// called areaportalnum) which is used
									// to bind them to the area portals by comparing with this value.

	unsigned short	OtherArea;		// The area this portal looks into.

	unsigned short	FirstClipPortalVert;	// Portal geometry.
	unsigned short	ClipPortalVerts;

	int				PlaneNum;
};


struct darea_t
{
	int		NumAreaPortals;
	int		FirstAreaPortal;
};


struct dworldlight_t
{
	Vector3D_F	Origin;
	Vector3D_F	Intensity;
	Vector3D_F	Bormal;			// for surfaces and spotlights
	int			Cluster;
	emittype_t	Type;
	int			Style;
	float		Stopdot;		// start of penumbra for emit_spotlight
	float		Stopdot2;		// end of penumbra for emit_spotlight
	float		Exponent;		// 
	float		Radius;			// cutoff distance
	// falloff for emit_spotlight + emit_point: 
	// 1 / (constant_attn + linear_attn * dist + quadratic_attn * dist^2)
	float		Constant_attn;
	float		Linear_attn;
	float		Quadratic_attn;
	int			Flags;			// Uses a combination of the DWL_FLAGS_ defines.
	int			Texinfo;		// 
	int			Owner;			// entity that this light it relative to
};

class IDispInfo
{
public:
	class GetIntersectingSurfaces_Struct
	{
	public:
		ModelInfo*			Model;
		const Vector3D_F*	Center;
		const std::byte*	CenterPVS;	// PVS for the leaf m_pCenter is in.
		float				Radius;
		bool				OnlyVisible;

		SurfInfo*			Infos;
		int					MaxInfos;
		int					SetInfos;
	};

	virtual				~IDispInfo() = default;

	// Builds a list of displacement triangles intersecting the sphere.
	virtual void		GetIntersectingSurfaces(GetIntersectingSurfaces_Struct* pStruct) = 0;

	virtual void		RenderWireframeInLightmapPage(int pageId) = 0;

	virtual void		GetBoundingBox(Vector3D_F& bbMin, Vector3D_F& bbMax) = 0;

	// Get and set the parent surfaces.
	virtual void		SetParent(void* surfID) = 0;
	virtual void* GetParent() = 0;

	// Add dynamic lights to the lightmap for this surface.
	virtual void AddDynamicLights(struct dlight_t* pLights, unsigned int lightMask) = 0;
	// Compute the mask for the lights hitting this surface.
	virtual unsigned int ComputeDynamicLightMask(struct dlight_t* pLights) = 0;

	// Add and remove decals.
	// flSize is like the radius of the decal so the decal isn't put on any disp faces it's too far away from.
	virtual void* NotifyAddDecal(void* pDecal, float flSize) = 0;
	virtual void				NotifyRemoveDecal(void* h) = 0;

	virtual void* AddShadowDecal(void* shadowHandle) = 0;
	virtual void				RemoveShadowDecal(void* handle) = 0;

	// Compute shadow fragments for a particular shadow, return the vertex + index count of all fragments
	virtual bool		ComputeShadowFragments(void* h, int& vertexCount, int& indexCount) = 0;

	// Tag the surface and check if it's tagged. You can untag all the surfaces
	// with DispInfo_ClearAllTags. Note: it just uses a frame counter to track the
	// tag state so it's really really fast to call ClearAllTags (just increments
	// a variable 99.999% of the time).
	virtual bool		GetTag() = 0;
	virtual void		SetTag() = 0;

	// Cast a ray against this surface
	virtual	bool	TestRay(const GameRay& ray, float start, float end, float& dist, Vector2D_F* lightmapUV, Vector2D_F* textureUV) = 0;

	// Computes the texture + lightmap coordinate given a displacement uv
	virtual void	ComputeLightmapAndTextureCoordinate(const RayDispOutput_t& uv, Vector2D_F* luv, Vector2D_F* tuv) = 0;
};

TF2_NAMESPACE_END();
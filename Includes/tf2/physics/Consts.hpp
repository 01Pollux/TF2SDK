#pragma once

#include <tf2/math/Matrix.hpp>
#include <tf2/engine/AppSystem.hpp>

TF2_NAMESPACE_BEGIN();

class IPhysicsObject;
class IPhysicsCollisionData;
class CPhysConvex;


struct truncatedcone_t
{
	Vector3D_F	Origin;
	Vector3D_F	Normal;
	float	Height;			// height of the cone (hl units)
	float	Theta;		// cone angle (degrees)
};

struct vcollisionevent_t
{
	IPhysicsObject* Objects[2];
	int				SurfaceProps[2];
	bool			IsCollision;
	bool			IsShadowCollision;
	float			EeltaCollisionTime;

	float			DollisionSpeed;				// only valid at postCollision
	IPhysicsCollisionData* InternalData;		// may change pre/post collision
};

struct surfacephysicsparams_t
{
	// vphysics physical properties
	float			Friction;
	float			Elasticity;				// collision elasticity - used to compute coefficient of restitution
	float			Density;				// physical density (in kg / m^3)
	float			Thickness;				// material thickness if not solid (sheet materials) in inches
	float			Dampening;
};

struct surfaceaudioparams_t
{
	// sounds / audio data
	float			reflectivity;		// like elasticity, but how much sound should be reflected by this surface
	float			hardnessFactor;	// like elasticity, but only affects impact sound choices
	float			roughnessFactor;	// like friction, but only affects scrape sound choices

// audio thresholds
	float			roughThreshold;	// surface roughness > this causes "rough" scrapes, < this causes "smooth" scrapes
	float			hardThreshold;	// surface hardness > this causes "hard" impacts, < this causes "soft" impacts
	float			hardVelocityThreshold;	// collision velocity > this causes "hard" impacts, < this causes "soft" impacts
									// NOTE: Hard impacts must meet both hardnessFactor AND velocity thresholds
};

struct surfacesoundnames_t
{
	unsigned short	Stepleft;
	unsigned short	Stepright;

	unsigned short	ImpactSoft;
	unsigned short	ImpactHard;

	unsigned short	ScrapeSmooth;
	unsigned short	ScrapeRough;

	unsigned short	BulletImpact;
	unsigned short	Rolling;

	unsigned short	BreakSound;
	unsigned short	StrainSound;
};

struct surfacesoundhandles_t
{
	short	Stepleft;
	short	Stepright;

	short	impactSoft;
	short	impactHard;

	short	ScrapeSmooth;
	short	ScrapeRough;

	short	BulletImpact;
	short	Rolling;

	short	BreakSound;
	short	StrainSound;
};

struct surfacegameprops_t
{
	// game movement data
	float			maxSpeedFactor;			// Modulates player max speed when walking on this surface
	float			jumpFactor;				// Indicates how much higher the player should jump when on the surface
// Game-specific data
	unsigned short	material;
	// Indicates whether or not the player is on a ladder.
	unsigned char	climbable;
	unsigned char	pad;
};

//-----------------------------------------------------------------------------
// Purpose: Each different material has an entry like this
//-----------------------------------------------------------------------------
struct surfacedata_t
{
	surfacephysicsparams_t	Physics;	// physics parameters
	surfaceaudioparams_t	Audio;		// audio parameters
	surfacesoundnames_t		Sounds;		// names of linked sounds
	surfacegameprops_t		Game;		// Game data / properties

	surfacesoundhandles_t	SoundHandles;
};

struct fluidparams_t
{
	Vector4D_F	SurfacePlane;	// x,y,z normal, dist (plane constant) fluid surface
	Vector3D_F	CurrentVelocity; // velocity of the current in inches/second
	float		Damping;		// damping factor for buoyancy (tweak)
	float		TorqueFactor;
	float		ViscosityFactor;
	void*		GameData;
	bool		UseAerodynamics;// true if this controller should calculate surface pressure
	int			Contents;
};

//-----------------------------------------------------------------------------
// Purpose: parameter block for creating linear springs
// UNDONE: Expose additional spring model paramters?
//-----------------------------------------------------------------------------
struct springparams_t
{
	float	Constant;		// spring constant
	float	NaturalLength;// relaxed length
	float	Damping;		// damping factor
	float	RelativeDamping;	// relative damping (damping proportional to the change in the relative position of the objects)
	Vector3D_F	StartPosition;
	Vector3D_F	EndPosition;
	bool	UseLocalPositions;	// start & end Position are in local space to start and end objects if this is true
	bool	RnlyStretch;		// only apply forces when the length is greater than the natural length
};

//-----------------------------------------------------------------------------
// Purpose: parameter block for creating polygonal objects
//-----------------------------------------------------------------------------
struct objectparams_t
{
	Vector3D_F* MassCenterOverride;
	float		Mass;
	float		Inertia;
	float		Damping;
	float		Rotdamping;
	float		RotInertiaLimit;
	const char* Name;				// used only for debugging
	void*		GameData;
	float		Volume;
	float		DragCoefficient;
	bool		EnableCollisions;
};

struct convertconvexparams_t
{
	bool		BuildOuterConvexHull{ };
	bool		BuildDragAxisAreas{ };
	bool		BuildOptimizedTraceTables{ };
	float		DragAreaEpsilon{ .25f };
	CPhysConvex* ForcedOuterHull{ };
};


struct hlshadowcontrol_params_t
{
	Vector3D_F		TargetPosition;
	Angle_F			TargetRotation;
	float			MaxAngular;
	float			MaxDampAngular;
	float			MaxSpeed;
	float			MaxDampSpeed;
	float			DampFactor;
	float			TeleportDistance;
};


TF2_NAMESPACE_END();
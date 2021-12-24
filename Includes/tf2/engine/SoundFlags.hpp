#pragma once

#include <tf2/config.hpp>

TF2_NAMESPACE_BEGIN(::Const);

enum class SoundChannel
{
	Replace = -1,
	CHAN_AUTO = 0,
	CHAN_WEAPON = 1,
	CHAN_VOICE = 2,
	CHAN_ITEM = 3,
	CHAN_BODY = 4,
	CHAN_STREAM = 5,		// allocate stream channel from the static or dynamic area
	CHAN_STATIC = 6,		// allocate channel from the static area 
	CHAN_VOICE2 = 7,
	CHAN_VOICE_BASE = 8,		// allocate channel for network voice data
	CHAN_USER_BASE = (CHAN_VOICE_BASE + 128)		// Anything >= this number is allocated to game code.
};

static constexpr float SoundVolume_Normal = 1.f;

namespace SoundAttenuation
{
	static constexpr float None = 0.f;
	// HL2 world is 8x bigger now! We want to hear gunfire from farther.
	// Don't change this without consulting Kelly or Wedge (sjb).
	static constexpr float Gunefire = 0.27f;
	static constexpr float Normal = 0.8f;
	static constexpr float Static = 1.25f;
	static constexpr float Ricochet = 1.5f;
	static constexpr float Idle = 2.f;

	// This is a limit due to network encoding.
	// It encodes attenuation * 64 in 8 bits, so the maximum is (255 / 64)
	static constexpr float Max = 3.98f;
}

enum class SoundLevel
{
	None = 0,

	DB20 = 20,	// rustling leaves
	DB25 = 25,	// whispering
	DB30 = 30,	// library
	DB35 = 35,
	DB40 = 40,
	DB45 = 45,	// refrigerator

	DB50 = 50,	// 3.9	// average home
	DB55 = 55,	// 3.0

	Idle = 60,	// 2.0	
	DB60 = 60,	// 2.0	normal conversation, clothes dryer

	DB65 = 65,	// 1.5	washing machine, dishwasher
	Static = 66,// 1.25

	DB70 = 70,	// 1.0	// car, vacuum cleaner, mixer, electric sewing machine

	Normal = 75,
	DB75 = 75,	// 0.8	// busy traffic

	Talking = 80,	// 0.7
	DB80 = 80,		// 0.7	mini-bike, alarm clock, noisy restaurant, office tabulator, outboard motor, passing snowmobile

	DB85 = 85,		// 0.6	average factory, electric shaver
	DB90 = 90,		// 0.5	screaming child, passing motorcycle, convertible ride on frw
	DB95 = 95,
	DB100 = 100,	// 0.4	// subway train, diesel truck, woodworking shop, pneumatic drill, boiler shop, jackhammer
	DB105 = 105,	// helicopter, power mower
	DB110 = 110,	// snowmobile drvrs seat, inboard motorboat, sandblasting
	DB120 = 120,	// auto horn, propeller aircraft
	DB130 = 130,	// air raid siren

	GunFire = 140,	// 0.27	THRESHOLD OF PAIN, gunshot, jet engine
	DB140 = 140,	// 0.2

	DB150 = 150,	// 0.2

	DB180 = 180,	// rocket launching

	// NOTE: Valid soundlevel_t values are 0-255.
	//       256-511 are reserved for sounds using goldsrc compatibility attenuation.
};

// Used to encode 0-255 for regular soundlevel_t's and 256-511 for goldsrc-compatible ones.
static constexpr size_t MaxSoundLevel_Bits = 9;
static constexpr size_t MinSoundLevel_Value = 0;
static constexpr size_t MaxSoundLevel_Value = (1 << MaxSoundLevel_Bits) - 1;

static constexpr SoundLevel AttenuationToSoundLevel(float attn)
{
	return attn ? static_cast<SoundLevel>(50 + 20 / attn) : SoundLevel::None;
}

static constexpr float SoundLevelToAttenuation(SoundLevel lvl)
{
	return lvl > SoundLevel::DB50 ? (20.f / (static_cast<float>(lvl) - 50.f)) : 4.f;
}


//-----------------------------------------------------------------------------
// Flags to be or-ed together for the iFlags field
//-----------------------------------------------------------------------------
namespace SoundFlags
{
	using type = size_t;

	// to keep the compiler happy
	static constexpr type None = 0;
	// change sound vol
	static constexpr type Change_Volume = (1 << 0);
	// change sound pitch
	static constexpr type Change_Pitch = (1 << 1);
	// stop the sound
	static constexpr type Stop = (1 << 2);
	// we're spawning, used in some cases for ambients
	// not sent over net, only a param between dll and server.
	static constexpr type Spawning = (1 << 3);
	
	// sound has an initial delay
	static constexpr type Delay = (1 << 4);
	// stop all looping sounds on the entity.
	static constexpr type StopLooping = (1 << 5);
	// being played again by a microphone through a speaker
	static constexpr type Speaker = (1 << 6);	// this sound should be paused if the game is paused
	static constexpr type ShouldPause = (1 << 7);
	static constexpr type Ignore_Phonemes = (1 << 8);
	// used to change all sounds emitted by an entity, regardless of scriptname
	static constexpr type Ignore_Name = (1 << 9);
	static constexpr type Dont_Overwrite_Existing = (1 << 10);

	static constexpr type MaxBits = 11;
	static constexpr type SoundIndex_Bits = 14;
	static constexpr type MaxSounds = 1 << SoundIndex_Bits;

	static constexpr type MaxDelayEncode_Bits = 13;
	// Subtract one to leave room for the sign bit
	// 4096 msec or about 4 seconds
	static constexpr type MaxDelayEncode = (1 << MaxDelayEncode_Bits) - 1;
};


// values are possible : 0-255, where 255 is very high
namespace SoundPitch
{
	// non-pitch shifted
	static constexpr size_t Normal = 100;
	static constexpr size_t Low = 95;
	static constexpr size_t High = 120;
}

namespace SoundPacket
{
	static const float Volume = 1.f;
	static const float Pitch = 100;
	static const float Delay = 0.f;
}

struct SoundInfo
{
	// Sound Guid
	int			Guid;
	void*		FilenameHandle;		// filesystem filename handle - call IFilesystem to conver this to a string
	int			SoundSource;
	int			Channel;
	// If a sound is being played through a speaker entity (e.g., on a monitor,), this is the
	//  entity upon which to show the lips moving, if the sound has sentence data
	int			SpeakerEntity;
	float		Volume;
	float		LastSpatializedVolume;
	// Radius of this sound effect (spatialization is different within the radius)
	float		Radius;
	int			Pitch;
	Vector3D_F*	Origin;
	Vector3D_F*	Direction;

	// if true, assume sound source can move and update according to entity
	bool		UpdatePositions;
	// true if playing linked sentence
	bool		IsSentence;
	// if true, bypass all dsp processing for this sound (ie: music)	
	bool		DryMix;
	// true if sound is playing through in-game speaker entity.
	bool		Speaker;
	// true if sound is playing with special DSP effect
	bool		SpecialDSP;
	// for snd_show, networked sounds get colored differently than local sounds
	bool		FromServer;
};


TF2_NAMESPACE_END();

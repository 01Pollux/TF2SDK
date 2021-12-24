#pragma once

#include "SoundFlags.hpp"
#include <tf2/client/RecipientFilter.hpp>

#include <tf2/math/Vector.hpp>
#include <tf2/utils/UtlVector.hpp>

TF2_NAMESPACE_BEGIN();

class IEngineSound;

namespace Interfaces
{
	TF2_EXPORT_INTERFACE(IEngineSound, ClientSound, "IEngineSoundClient003");
}


class IEngineSound
{
public:
	// Precache a particular sample
	virtual bool PrecacheSound(const char* pSample, bool bPreload = false, bool bIsUISound = false) abstract;
	virtual bool IsSoundPrecached(const char* pSample) abstract;
	virtual void PrefetchSound(const char* pSample) abstract;

	// Just loads the file header and checks for duration (not hooked up for .mp3's yet)
	// Is accessible to server and client though
	virtual float GetSoundDuration(const char* pSample) abstract;

	// Pitch of 100 is no pitch shift.  Pitch > 100 up to 255 is a higher pitch, pitch < 100
	// down to 1 is a lower pitch.   150 to 70 is the realistic range.
	// EmitSound with pitch != 100 should be used sparingly, as it's not quite as
	// fast (the pitchshift mixer is not native coded).

	// NOTE: setting iEntIndex to -1 will cause the sound to be emitted from the local
	// player (client-side only)
	virtual void EmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSample,
		float flVolume, float flAttenuation, int iFlags = 0, int iPitch = Const::SoundPitch::Normal, int iSpecialDSP = 0,
		const Vector3D_F* pOrigin = nullptr, const Vector3D_F* pDirection = nullptr, Utils::UtlVector<Vector3D_F>* pUtlVecOrigins = nullptr, bool bUpdatePositions = true, float soundtime = .0f, int speakerentity = -1) abstract;

	virtual void EmitSound(IRecipientFilter& filter, int iEntIndex, int iChannel, const char* pSample,
		float flVolume, Const::SoundLevel iSoundlevel, int iFlags = 0, int iPitch = Const::SoundPitch::Normal, int iSpecialDSP = 0,
		const Vector3D_F* pOrigin = nullptr, const Vector3D_F* pDirection = nullptr, Utils::UtlVector<Vector3D_F>* pUtlVecOrigins = nullptr, bool bUpdatePositions = true, float soundtime = .0f, int speakerentity = -1) abstract;

	virtual void EmitSentenceByIndex(IRecipientFilter& filter, int iEntIndex, int iChannel, int iSentenceIndex,
		float flVolume, Const::SoundLevel iSoundlevel, int iFlags = 0, int iPitch = Const::SoundPitch::Normal, int iSpecialDSP = 0,
		const Vector3D_F* pOrigin = nullptr, const Vector3D_F* pDirection = nullptr, Utils::UtlVector<Vector3D_F>* pUtlVecOrigins = nullptr, bool bUpdatePositions = true, float soundtime = .0f, int speakerentity = -1) abstract;

	virtual void StopSound(int iEntIndex, int iChannel, const char* pSample) abstract;

	// stop all active sounds (client only)
	virtual void StopAllSounds(bool bClearBuffers) abstract;

	// Set the room type for a player (client only)
	virtual void SetRoomType(IRecipientFilter& filter, int roomType) abstract;

	// Set the dsp preset for a player (client only)
	virtual void SetPlayerDSP(IRecipientFilter& filter, int dspType, bool fastReset) abstract;

	// emit an "ambient" sound that isn't spatialized
	// only available on the client, assert on server
	virtual void EmitAmbientSound(const char* pSample, float flVolume, int iPitch = Const::SoundPitch::Normal, int flags = 0, float soundtime = .0f) abstract;


	//	virtual EntChannel_t	CreateEntChannel() abstract;

	virtual float GetDistGainFromSoundLevel(Const::SoundLevel soundlevel, float dist) abstract;

	// Client .dll only functions
	virtual int		GetGuidForLastSoundEmitted() abstract;
	virtual bool	IsSoundStillPlaying(int guid) abstract;
	virtual void	StopSoundByGuid(int guid) abstract;
	// Set's master volume (0.0->1.0)
	virtual void	SetVolumeByGuid(int guid, float fvol) abstract;

	// Retrieves list of all active sounds
	virtual void	GetActiveSounds(Utils::UtlVector<Const::SoundInfo>& sndlist) abstract;

	virtual void	PrecacheSentenceGroup(const char* pGroupName) abstract;
	virtual void	NotifyBeginMoviePlayback() abstract;
	virtual void	NotifyEndMoviePlayback() abstract;
};

TF2_NAMESPACE_END();

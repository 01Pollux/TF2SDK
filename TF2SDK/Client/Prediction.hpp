#pragma once

#include "TF2Config.hpp"
#include "Math/Vector.hpp"
#include "Entity/HandleEntity.hpp"

TF2_NAMESPACE_BEGIN();

class IPrediction;
class IGameMovement;

namespace Interfaces
{
	TF2_EXPORT_INTERFACE(IPrediction, Prediction, "VClientPrediction001");
	TF2_EXPORT_INTERFACE(IGameMovement, GameMovement, "GameMovement001");
}


class IMoveHelper;
class UserCmd;
class IBaseEntityInternal;
struct PredMoveData;

//-----------------------------------------------------------------------------
// Purpose: Engine interface into client side prediction system
//-----------------------------------------------------------------------------
class IPrediction
{
public:
	virtual			~IPrediction() = default;

	virtual void	Init() abstract;
	virtual void	Shutdown() abstract;

	// Run prediction
	virtual void	Update(
						int startframe,				// World update ( un-modded ) most recently received
						bool validframe,			// Is frame data valid
						int incoming_acknowledged,	// Last command acknowledged to have been run by server (un-modded)
						int outgoing_command		// Last command (most recent) sent to server (un-modded)
					) abstract;

	// We are about to get a network update from the server.  We know the update #, so we can pull any
	//  data purely predicted on the client side and transfer it to the new from data state.
	virtual void	PreEntityPacketReceived(int commands_acknowledged, int current_world_update_packet) abstract;
	virtual void	PostEntityPacketReceived() abstract;
	virtual void	PostNetworkDataReceived(int commands_acknowledged) abstract;

	virtual void	OnReceivedUncompressedPacket() abstract;

	// The engine needs to be able to access a few predicted values
	virtual void	GetViewOrigin(Vector3D_F& org) abstract;
	virtual void	SetViewOrigin(Vector3D_F& org) abstract;
	virtual void	GetViewAngles(Angle_F& ang) abstract;
	virtual void	SetViewAngles(Angle_F& ang) abstract;
	virtual void	GetLocalViewAngles(Angle_F& ang) abstract;
	virtual void	SetLocalViewAngles(Angle_F& ang) abstract;

	virtual bool	InPrediction() const abstract;
	virtual bool	IsFirstTimePredicted() const abstract;

	virtual int		GetIncomingPacketNumber() const abstract;

	virtual void	RunCommand(IBaseEntityInternal* player, UserCmd* ucmd, IMoveHelper* moveHelper) abstract;

	virtual void	SetupMove(IBaseEntityInternal* player, UserCmd* ucmd, IMoveHelper* pHelper, PredMoveData* move) abstract;
	virtual void	FinishMove(IBaseEntityInternal* player, UserCmd* ucmd, PredMoveData* move) abstract;
	virtual void	SetIdealPitch(IBaseEntityInternal* player, const Vector3D_F& origin, const Angle_F& angles, const Angle_F& viewheight) abstract;
};


class IGameMovement
{
public:
	virtual			~IGameMovement() = default;

	// Process the current movement command
	virtual void	ProcessMovement(IBaseEntityInternal* pPlayer, PredMoveData* pMove)  abstract;
	virtual void	StartTrackPredictionErrors(IBaseEntityInternal* pPlayer)  abstract;
	virtual void	FinishTrackPredictionErrors(IBaseEntityInternal* pPlayer)  abstract;
	virtual void	DiffPrint(char const* fmt, ...)  abstract;

	// Allows other parts of the engine to find out the normal and ducked player bbox sizes
	virtual Vector3D_F	GetPlayerMins(bool ducked) const  abstract;
	virtual Vector3D_F	GetPlayerMaxs(bool ducked) const  abstract;
	virtual Vector3D_F  GetPlayerViewOffset(bool ducked) const  abstract;

};



struct PredMoveData
{
	bool			FirstRunOfFunctions : 1;
	bool			GameCodeMovedPlayer : 1;

	IBaseHandle		PlayerHandle;	// edict index on server, client entity handle on client

	int				ImpulseCommand;	// Impulse command issued.
	Angle_F			ViewAngles;	// Command view angles (local space)
	Angle_F			AbsViewAngles;	// Command view angles (world space)
	int				Buttons;			// Attack buttons.
	int				OldButtons;		// From host_client->oldbuttons;
	float			ForwardMove;
	float			OldForwardMove;
	float			SideMove;
	float			UpMove;

	float			MaxSpeed;
	float			ClientMaxSpeed;

	// Variables from the player edict (sv_player) or entvars on the client.
	// These are copied in here before calling and copied out after calling.
	Vector3D_F		Velocity;		// edict::velocity		// Current movement direction.
	Angle_F			Angles;		// edict::angles
	Angle_F			OldAngles;

	// Output only
	float			OutStepHeight;	// how much you climbed this move
	Vector3D_F		OutWishVel;		// This is where you tried 
	Vector3D_F		OutJumpVel;		// This is your jump velocity

	// Movement constraints	(radius 0 means no constraint)
	Vector3D_F		ConstraintCenter;
	float			ConstraintRadius;
	float			ConstraintWidth;
	float			ConstraintSpeedFactor;

	Vector3D_F		AbsOrigin;		// edict::origin
};


TF2_NAMESPACE_END();

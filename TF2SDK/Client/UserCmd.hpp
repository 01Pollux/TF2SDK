#pragma once

#include "Math/Vector.hpp"

TF2_NAMESPACE_BEGIN(::Const)

class UserCmd
{
public:
	virtual ~UserCmd() = default;

public:
	int			CmdNumber{ };

	int			TickCount{ };

	Angle_F		ViewAngles{ };
	float		ForwardMove{ };
	float		SideMove{ };
	float		UpMove{ };
	int			Buttons{ };
	std::byte   Impulse{ };
	int			WeaponSelect{ };
	int			WeaponSubtype{ };

	int			RandomSeed{ };

	short		MouseX{ };
	short		MouseY{ };

	bool		HasBeenPredicted{ };

	struct Keys
	{
		static constexpr uint32_t Attack	= 1 << 0;
		static constexpr uint32_t Jump		= 1 << 1;
		static constexpr uint32_t Duck		= 1 << 2;
		static constexpr uint32_t Forward	= 1 << 3;
		static constexpr uint32_t Back		= 1 << 4;
		static constexpr uint32_t Use		= 1 << 5;
		static constexpr uint32_t Cancel	= 1 << 6;
		static constexpr uint32_t Left		= 1 << 7;
		static constexpr uint32_t Right		= 1 << 8;
		static constexpr uint32_t MoveLeft	= 1 << 9;
		static constexpr uint32_t MoveRight = 1 << 10;
		static constexpr uint32_t Attack2	= 1 << 11;
		static constexpr uint32_t Run		= 1 << 12;
		static constexpr uint32_t Reload	= 1 << 13;
		static constexpr uint32_t Alt1		= 1 << 14;
		static constexpr uint32_t Alt2		= 1 << 15;
		// Used by client.dll for when scoreboard is held down
		static constexpr uint32_t Score		= 1 << 16;
		// Player is holding the speed key
		static constexpr uint32_t Speed		= 1 << 17;
		// Player holding walk key
		static constexpr uint32_t Walk		= 1 << 18;
		// Zoom key for HUD zoom
		static constexpr uint32_t Zoom		= 1 << 19;
		// weapon defines these bits
		static constexpr uint32_t Weapon1	= 1 << 20;
		// weapon defines these bits
		static constexpr uint32_t Weapon2	= 1 << 21;
		static constexpr uint32_t Bullrush	= 1 << 22;
		static constexpr uint32_t Grenade1	= 1 << 23;
		static constexpr uint32_t Grenade2	= 1 << 24;
		static constexpr uint32_t Attack3	= 1 << 25;
	};
};


TF2_NAMESPACE_END();

#include <shadowgarden/interfaces/GameData.hpp>

#include <tf2/client/GameRules.hpp>

#include <tf2/utils/Thunks.hpp>

TF2_NAMESPACE_BEGIN();

bool IGameRules::ShouldCollide(Const::EntCollisionGroup group0, Const::EntCollisionGroup group1)
{
	if (group0 > group1)
		std::swap(group0, group1);

	using enum Const::EntCollisionGroup;

	switch (group1)
	{
	case Interactive:
	{
		if (group0 != None)
			return false;
		break;
	}

	case InVehicle:
	case Tank:
		return false;

	case Weapon:
	{
		switch (group0)
		{
		case Player:
		case Vehicle:
		case NPC:
			return false;
		}
		break;
	}

	case Projectile:
	{
		switch (group0)
		{
		case Debris:
		case Weapon:
		case Projectile:
			return false;
		}
		break;
	}

	case DoorBlocket:
	{
		if (group0 != NPC)
			return false;
		break;
	}

	case Respawn_Rooms:
	{
		if (group0 == Player || group0 == PlayerMovements)
			return false;
		break;
	}
	}


	switch (group0)
	{
	case Debris:
	{
		if (group1 == Weapon)
			return true;
		break;
	}

	case DebrisTirgger:
		return false;

	case InteractiveDebris:
	{
		switch (group1)
		{
		case InteractiveDebris:
		case Player:
		case PlayerMovements:
			return false;
		}
		break;
	}

	case Player:
	{
		switch (group1)
		{
		case Rockets:
		case Rocket_But_Not_With_Other_Rockets:
			return true;

		case PassableDoor:
		case Grenades:
		case CombatObject:
			return false;

		default:
		{
			// TODO: Code_R0x102210CE
		}
		}
		return false;
	}

	case BreakableGlass:
	{
		if (group1 == BreakableGlass)
			return false;
		break;
	}

	case PlayerMovements:
	{
		switch (group1)
		{
		case Weapon:
		case Projectile:
		case Grenades:
		case CombatObject:
		case Rockets:
		case Rocket_But_Not_With_Other_Rockets:
		case PushAway:
			return false;
		}
		break;
	}

	case Weapon:
	case Projectile:
	{
		switch (group1)
		{
		case Rockets:
		case Rocket_But_Not_With_Other_Rockets:
			return false;
		}
		break;
	}
	
	case InVehicle:
	{
		return false;
	}

	case VehicleClip:
	{
		if (group1 == VehicleClip)
			return true;
		break;
	}

	case Grenades:
	{
		switch (group1)
		{
		case Grenades:
		case Rockets:
		case Rocket_But_Not_With_Other_Rockets:
			return false;
		}
		break;
	}
	
	case Rockets:
	case Rocket_But_Not_With_Other_Rockets:
	{
		if (group1 == Rocket_But_Not_With_Other_Rockets)
			return false;
		break;
	}

	}

	return true;
}

TF2_NAMESPACE_END();

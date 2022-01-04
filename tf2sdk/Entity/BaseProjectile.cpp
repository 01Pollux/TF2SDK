#include <algorithm>

#include <tf2/engine/GlobalVars.hpp>

#include <tf2/entity/BaseWeapon.hpp>
#include <tf2/entity/BaseProjectile.hpp>

TF2_NAMESPACE_BEGIN();

void utils::QueryProjectileInfo(const IEconEntityInternal* pEnt, Const::EntClassID class_id, float* speed, float* gravity, float* start_velocity)
{
	constexpr auto set_if_valid = [](float* pVal, float res) { if (pVal) *pVal = res; };

	set_if_valid(gravity, 0.f);
	set_if_valid(start_velocity, 0.f);

	switch (class_id)
	{
	case Const::EntClassID::CTFJar:
	case Const::EntClassID::CTFJarMilk:
	case Const::EntClassID::CTFProjectile_Jar:
	case Const::EntClassID::CTFProjectile_JarMilk:
	case Const::EntClassID::CTFProjectile_ThrowableBreadMonster:
	case Const::EntClassID::CTFProjectile_Arrow:
	case Const::EntClassID::CTFProjectile_BallOfFire:
	case Const::EntClassID::CTFProjectile_EnergyBall:
	case Const::EntClassID::CTFProjectile_EnergyRing:
	case Const::EntClassID::CTFProjectile_GrapplingHook:
	case Const::EntClassID::CTFProjectile_HealingBolt:
	case Const::EntClassID::CTFProjectile_MechanicalArmOrb:
	case Const::EntClassID::CTFProjectile_SpellBats:
	case Const::EntClassID::CTFProjectile_SpellFireball:
	case Const::EntClassID::CTFProjectile_SpellKartBats:
	case Const::EntClassID::CTFProjectile_SpellKartOrb:
	case Const::EntClassID::CTFProjectile_SpellLightningOrb:
	case Const::EntClassID::CTFProjectile_SpellMeteorShower:
	case Const::EntClassID::CTFProjectile_SpellMirv:
	case Const::EntClassID::CTFProjectile_SpellPumpkin:
	case Const::EntClassID::CTFProjectile_SpellSpawnBoss:
	case Const::EntClassID::CTFProjectile_SpellSpawnHorde:
	case Const::EntClassID::CTFProjectile_SpellSpawnZombie:
	case Const::EntClassID::CTFProjectile_SpellTransposeTeleport:
	{
		set_if_valid(speed, 1000.f);
		break;
	}

	case Const::EntClassID::CTFCleaver:
	case Const::EntClassID::CTFProjectile_Cleaver:
	case Const::EntClassID::CTFWeaponFlameBall:
	{
		set_if_valid(speed, 7000.f);
		break;
	}

	case Const::EntClassID::CTFBat_Wood:
	case Const::EntClassID::CTFStunBall:
	case Const::EntClassID::CTFBat_Giftwrap:
	case Const::EntClassID::CTFBall_Ornament:
	{
		set_if_valid(speed, 3000.f);
		set_if_valid(gravity, .5f);
		break;
	}

	case Const::EntClassID::CTFRocketLauncher:
	case Const::EntClassID::CTFRocketLauncher_AirStrike:
	case Const::EntClassID::CTFParticleCannon:	// Cow mangler 5000
	case Const::EntClassID::CTFProjectile_Rocket:
	{
		// Liberty Launcher
		set_if_valid(speed, pEnt->ItemDefinitionIndex == 414 ? 1540.f : 1100.f);
		set_if_valid(gravity, .5f);
		break;
	}

	case Const::EntClassID::CTFRocketLauncher_DirectHit:
	{
		set_if_valid(speed, 1980.f);
		break;
	}

	case Const::EntClassID::CTFRaygun: // Bison
	case Const::EntClassID::CTFDRGPomson:
	{
		set_if_valid(speed, 1200.f);
		break;
	}

	case Const::EntClassID::CTFFlareGun:
	case Const::EntClassID::CTFFlareGun_Revenge:
	case Const::EntClassID::CTFProjectile_Flare:
	{
		set_if_valid(speed, 2000.f);
		set_if_valid(gravity, .5f);
		break;
	}

	case Const::EntClassID::CTFJarGas:
	case Const::EntClassID::CTFProjectile_JarGas:
	{
		set_if_valid(speed, 2000.f);
		break;
	}

	case Const::EntClassID::CTFGrenadeLauncher:
	{
		// Loch-n-Load
		set_if_valid(speed, pEnt->ItemDefinitionIndex == 308 ? 1513.3f : 1216.f);
		set_if_valid(gravity, 1.f);
		set_if_valid(start_velocity, 200.f);
		break;
	}

	case Const::EntClassID::CTFCannon:
	{
		set_if_valid(speed, 1453.9f);
		set_if_valid(gravity, 1.f);
		set_if_valid(start_velocity, 200.f);
		break;
	}

	case Const::EntClassID::CTFPipebombLauncher:
	case Const::EntClassID::CTFGrenadePipebombProjectile:
	{
		const float charge_time = static_cast<const IBaseWeaponInternal*>(pEnt)->PipeChargeBeginTime ?
			0.f : interfaces::GlobalVars->CurTime - static_cast<const IBaseWeaponInternal*>(pEnt)->PipeChargeBeginTime;
		const float speed_begin = pEnt->ItemDefinitionIndex == 1150 ? 925.38f : 925.38f;

		set_if_valid(speed, std::lerp(925.38f, 2409.2f, std::min(charge_time / 4.f, 1.f)));
		set_if_valid(gravity, 1.f);
		set_if_valid(start_velocity, 200.f);
		break;
	}

	case Const::EntClassID::CObjectSentrygun:
	case Const::EntClassID::CTFProjectile_SentryRocket:
	{
		set_if_valid(speed, 1100.f);
		break;
	}

	case Const::EntClassID::CTFCrossbow:
	case Const::EntClassID::CTFShotgunBuildingRescue:
	{
		set_if_valid(speed, 2400.f);
		break;
	}

	case Const::EntClassID::CTFSyringeGun:
	{
		set_if_valid(speed, 1000.f);
		set_if_valid(gravity, 0.3f);
		break;
	}

	case Const::EntClassID::CTFCompoundBow:
	{
		const float charge_time = static_cast<const IBaseWeaponInternal*>(pEnt)->SniperChargeDamage ?
			0.f : interfaces::GlobalVars->CurTime - static_cast<const IBaseWeaponInternal*>(pEnt)->SniperChargeDamage;
		const float speed_begin = pEnt->ItemDefinitionIndex == 1150 ? 925.38f : 925.38f;

		set_if_valid(speed, std::lerp(1812.1f, 2600.f, std::min(charge_time / 1.f, 1.f)));
		set_if_valid(gravity, std::lerp(0.5f, 0.1f, std::min(charge_time / 1.f, 1.f)));
		break;
	}

	case Const::EntClassID::CTFGrapplingHook:
	{
		set_if_valid(speed, 1500.f);
		break;
	}
	}
}

TF2_NAMESPACE_END();
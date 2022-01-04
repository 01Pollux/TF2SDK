
#include <functional>
#include <px/interfaces/GameData.hpp>
#include <tf2/entity/BaseWeapon.hpp>
#include <tf2/utils/Thunks.hpp>

TF2_NAMESPACE_BEGIN();

Const::WeaponSlot IBaseWeaponInternal::GetWeaponSlot() const noexcept
{
	// CHudWeaponSelection::DrawSelection
	// First virtual function
	static utils::MemberVFuncThunk<Const::WeaponSlot> get_slot{ interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon", "vtable" }, "GetWeaponSlot").value_or(-1) };
	assert(get_slot);
	return get_slot(this);
}

bool IBaseWeaponInternal::DoSwingTrace(GameTrace& trace) noexcept
{
	static utils::MemberVFuncThunk<bool, GameTrace&> do_swingtrace{ interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon", "vtable"  }, "DoSwingTrace").value_or(-1) };
	assert(do_swingtrace);
	return do_swingtrace(this, trace);
}

int IBaseWeaponInternal::GetSwingRange() const noexcept
{
	static utils::MemberVFuncThunk<int> get_swingrange{ interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon", "vtable"  }, "GetSwingRange").value_or(-1) };
	assert(get_swingrange);
	return get_swingrange(this);
}

Const::WeaponType IBaseWeaponInternal::GetWeaponId() const noexcept
{
	static utils::MemberVFuncThunk<Const::WeaponType> get_weaponid{ interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon", "vtable"  }, "GetWeaponId").value_or(-1) };
	assert(get_weaponid);
	return get_weaponid(this);
}

float IBaseWeaponInternal::GetProjectileSpeed() const noexcept
{
	static utils::MemberVFuncThunk<float> get_speed{ interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon", "vtable"  }, "GetProjectileSpeed").value_or(-1) };
	assert(get_speed);
	return get_speed(this);
}

float IBaseWeaponInternal::GetProjectileGravity() const noexcept
{
	static utils::MemberVFuncThunk<float> get_gravity{ interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon", "vtable"  }, "GetProjectileGravity").value_or(-1) };
	assert(get_gravity);
	return get_gravity(this);
}

float IBaseWeaponInternal::GetProjectileSpread() const noexcept
{
	static utils::MemberVFuncThunk<float> get_spread{ interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon", "vtable"  }, "GetProjectileSpread").value_or(-1) };
	assert(get_spread);
	return get_spread(this);
}

float IBaseWeaponInternal::GetProjectileDamage() const noexcept
{
	static utils::MemberVFuncThunk<float> get_damage{ interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon", "vtable"  }, "GetProjectileDamage").value_or(-1) };
	assert(get_damage);
	return get_damage(this);
}

TF2_NAMESPACE_END();

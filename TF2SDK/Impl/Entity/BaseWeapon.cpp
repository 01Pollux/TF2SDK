
#include <Interfaces/GameData.hpp>
#include "Entity/BaseWeapon.hpp"
#include "Utils/Thunks.hpp"

TF2_NAMESPACE_BEGIN();

int IBaseWeaponInternal::GetWeaponSlot() const noexcept
{
	static Utils::IMemberVFuncThunk<int> get_slot{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon" }, "GetWeaponSlot").value_or(-1) };
	return get_slot(this);
}

bool IBaseWeaponInternal::DoSwingTrace(GameTrace& trace) noexcept
{
	static Utils::IMemberVFuncThunk<bool, GameTrace&> do_swingtrace{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon" }, "DoSwingTrace").value_or(-1) };
	return do_swingtrace(this, trace);
}

int IBaseWeaponInternal::GetSwingRange() const noexcept
{
	static Utils::IMemberVFuncThunk<int> get_swingrange{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon" }, "GetSwingRange").value_or(-1) };
	return get_swingrange(this);
}

TF2_NAMESPACE_END();

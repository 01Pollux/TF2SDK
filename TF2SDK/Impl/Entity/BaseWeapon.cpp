
#include <Interfaces/GameData.hpp>
#include "Entity/BaseWeapon.hpp"
#include "Utils/Thunks.hpp"

TF2_NAMESPACE_BEGIN();

int IBaseWeaponInternal::GetWeaponSlot() const noexcept
{
	// CHudWeaponSelection::DrawSelection
	// First virtual function
	// 327
	static Utils::IMemberVFuncThunk<int> get_slot{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon", "vtable" }, "GetWeaponSlot").value_or(-1)};
	return get_slot(this);
}

bool IBaseWeaponInternal::DoSwingTrace(GameTrace& trace) noexcept
{
	static Utils::IMemberVFuncThunk<bool, GameTrace&> do_swingtrace{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon", "vtable"  }, "DoSwingTrace").value_or(-1) };
	return do_swingtrace(this, trace);
}

int IBaseWeaponInternal::GetSwingRange() const noexcept
{
	static Utils::IMemberVFuncThunk<int> get_swingrange{ Interfaces::SDKManager::Get()->ReadOffset({ "CBaseWeapon", "vtable"  }, "GetSwingRange").value_or(-1) };
	return get_swingrange(this);
}

TF2_NAMESPACE_END();

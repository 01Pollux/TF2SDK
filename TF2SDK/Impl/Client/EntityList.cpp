
#include <Interfaces/GameData.hpp>

#include "Utils/UtlVector.hpp"

#include "Client/EntityList.hpp"

TF2_NAMESPACE_BEGIN();

static Utils::UtlVector<IClientEntityListener*>* FindEntityListener()
{
	static auto pEntListener = 
		IntPtr(Interfaces::SDKManager::Get()->ReadSignature("pCEntityListPtr") + 
			Interfaces::SDKManager::Get()->ReadOffset({ "CEntityList", "offsets" }, "EntListeners").value_or(0)
		).get<Utils::UtlVector<IClientEntityListener*>>();
	assert(pEntListener);
	return pEntListener;
}

void IClientEntityListener::AddEntityListener() noexcept
{
	auto pListeners = FindEntityListener();
	if (pListeners->Find(this) == -1)
		pListeners->push_to_tail(this);
}

void IClientEntityListener::RemoveEntityListener() noexcept
{
	auto pListeners = FindEntityListener();
	pListeners->find_and_erase(this);
}

TF2_NAMESPACE_END();

#include <px/interfaces/GameData.hpp>

#include <tf2/utils/UtlVector.hpp>

#include <tf2/client/EntityList.hpp>

TF2_NAMESPACE_BEGIN();

static utils::UtlVector<IClientEntityListener*>* FindEntityListener()
{
	static auto pEntListener = 
		IntPtr(interfaces::SDKManager::Get()->ReadSignature("pCEntityListPtr") + 
			interfaces::SDKManager::Get()->ReadOffset({ "CEntityList", "offsets" }, "EntListeners").value_or(0)
		).get<utils::UtlVector<IClientEntityListener*>>();
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

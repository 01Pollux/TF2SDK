
#include <string.h>
#include <tf2/gameprop/SendProp.hpp>
#include <tf2/engine/EngineIFace.hpp>

TF2_NAMESPACE_BEGIN();


static bool FindInSendProp(const SendTable* pTable, const char* target_name, SendProp** outProp, uint32_t* outOffset)
{
	assert(pTable);
	for (int i = 0; i < pTable->NumProps; i++)
	{
		SendProp* prop = &pTable->Props[i];
		const char* name = prop->Name;

		if (name && !strcmp(name, target_name))
		{
			if (outProp)
				*outProp = prop;

			if (outOffset)
				*outOffset = prop->Offset;

			return true;
		}

		if (prop->DataTable)
		{
			if (!FindInSendProp(prop->DataTable, target_name, outProp, outOffset))
				continue;

			if (outOffset)
				*outOffset += prop->Offset;
			return true;
		}
	}
	return false;
}

bool PropFinder::FindSendProp(ServerClass* pClass, const char* prop_name, SendProp** pProp, uint32_t* pOffset)
{
	assert(pClass);
	return FindInSendProp(pClass->SendTable, prop_name, pProp, pOffset);
}

bool PropFinder::FindSendProp(const char* class_name, const char* prop_name, SendProp** pProp, uint32_t* pOffset)
{
	for (ServerClass* sc = interfaces::ServerDLL->GetAllServerClasses(); sc != nullptr; sc = sc->NextClass)
	{
		if (!strcmp(sc->NetworkName, class_name))
			return FindInSendProp(sc->SendTable, prop_name, pProp, pOffset);
	}
	return false;
}

bool PropFinder::FindSendProp(Const::EntClassID class_id, const char* prop_name, SendProp** pProp, uint32_t* pOffset)
{
	for (ServerClass* sc = interfaces::ServerDLL->GetAllServerClasses(); sc != nullptr; sc = sc->NextClass)
	{
		if (sc->ClassID == class_id)
			return FindInSendProp(sc->SendTable, prop_name, pProp, pOffset);
	}
	return false;
}


TF2_NAMESPACE_END();

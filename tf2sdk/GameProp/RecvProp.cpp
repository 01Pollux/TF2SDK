
#include <string.h>
#include <tf2/gameprop/RecvProp.hpp>
#include <tf2/engine/ClientDll.hpp>

TF2_NAMESPACE_BEGIN();


static bool FindInRecvTable(const RecvTable* pTable, const char* target_name, RecvProp** outProp, uint32_t* outOffset)
{
	assert(pTable);
	for (int i = 0; i < pTable->NumProps; i++)
	{
		RecvProp* prop = &pTable->Props[i];
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
			if (!FindInRecvTable(prop->DataTable, target_name, outProp, outOffset))
				continue;

			if (outOffset)
				*outOffset += prop->Offset;
			return true;
		}
	}
	return false;
}

bool PropFinder::FindRecvProp(ClientClass* pClass, const char* prop_name, RecvProp** pProp, uint32_t* pOffset)
{
	assert(pClass);
	return FindInRecvTable(pClass->RecvTable, prop_name, pProp, pOffset);
}

bool PropFinder::FindRecvProp(const char* class_name, const char* prop_name, RecvProp** pProp, uint32_t* pOffset)
{
	for (ClientClass* cc = Interfaces::ClientDLL->GetAllClasses(); cc != nullptr; cc = cc->NextClass)
	{
		if (!strcmp(cc->NetworkName, class_name))
			return FindInRecvTable(cc->RecvTable, prop_name, pProp, pOffset);
	}
	return false;
}

bool PropFinder::FindRecvProp(Const::EntClassID class_id, const char* prop_name, RecvProp** pProp, uint32_t* pOffset)
{
	for (ClientClass* cc = Interfaces::ClientDLL->GetAllClasses(); cc != nullptr; cc = cc->NextClass)
	{
		if (cc->ClassID == class_id)
			return FindInRecvTable(cc->RecvTable, prop_name, pProp, pOffset);
	}
	return false;
}


TF2_NAMESPACE_END();

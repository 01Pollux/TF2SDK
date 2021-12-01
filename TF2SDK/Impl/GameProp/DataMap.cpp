
#include "GameProp/DataMap.hpp"

TF2_NAMESPACE_BEGIN();

bool PropFinder::FindDataMap(EntityDataMap* datamap, const char* prop_name, DM_TypeDescription** pDesc, uint32_t* pOffset)
{
	assert(datamap);
	while (datamap)
	{
		for (int i = 0; i < datamap->DataNumFields; i++)
		{
			DM_TypeDescription* desc = &datamap->DataDesc[i];
			const char* field_name = desc->FieldName;
			if (!field_name)
				continue;

			if (!strcmp(field_name, prop_name))
			{
				if (pDesc)
					*pDesc = desc;
				if (pOffset)
					*pOffset = desc->Offset[static_cast<size_t>(Const::DataMapTypeDesc::PackType::Normal)];
				return true;
			}

			EntityDataMap* nested = desc->DataDesc;
			if (!nested || !FindDataMap(nested, prop_name, pDesc, pOffset))
				continue;

			if (pOffset)
				*pOffset += desc->Offset[static_cast<size_t>(Const::DataMapTypeDesc::PackType::Normal)];
			return true;
		}

		datamap = datamap->BaseMap;
	}
	return false;
}

TF2_NAMESPACE_END();

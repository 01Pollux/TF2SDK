#include <px/interfaces/GameData.hpp>

#include <tf2/entity/EconEntity.hpp>
#include <tf2/utils/Thunks.hpp>

TF2_NAMESPACE_BEGIN();

void IAttributeList::SetAttribute(int index, float value) noexcept
{
	for (auto& attr : Attributes)
	{
		if (attr.AttributeDefinitionIndex == index)
		{
			attr.FloatValue = value;
			return;
		}
	}

	if (Attributes.size() < 15)
		Attributes.push_to_tail(EconItemAttribute{ static_cast<uint16_t>(index), value});
}

float IAttributeList::GetAttribute(int index) noexcept
{
	for (auto& attrib : Attributes)
		if (attrib.AttributeDefinitionIndex == index)
			return attrib.FloatValue;

	return 0.f;
}

void IAttributeList::RemoveAttribute(int index) noexcept
{
	for (uint32_t i = 0; i < Attributes.size(); i++)
	{
		if (Attributes[i].AttributeDefinitionIndex == index)
		{
			Attributes.erase_fast(i); 
			break;
		}
	}
}

PX_SDK_TF2 int IAttributeList::AttributeHookValue(int fail_value, const char* attribute_name, const IBaseEntityInternal* pEnt, utils::UtlVector<IBaseEntityInternal*>* pItemList, bool is_global_string)
{
	static utils::FuncThunk<int, int, const char*, const IBaseEntityInternal*, utils::UtlVector<IBaseEntityInternal*>*, bool>
		get_attribute_value{ interfaces::SDKManager::Get()->ReadSignature({ "CEconEntity", "AttributeHookValue" }, "int").get()};
	assert(get_attribute_value);
	return get_attribute_value(fail_value, attribute_name, pEnt, pItemList, is_global_string);
}

PX_SDK_TF2 float IAttributeList::AttributeHookValue(float fail_value, const char* attribute_name, const IBaseEntityInternal* pEnt, utils::UtlVector<IBaseEntityInternal*>* pItemList, bool is_global_string)
{
	static utils::FuncThunk<float, float, const char*, const IBaseEntityInternal*, utils::UtlVector<IBaseEntityInternal*>*, bool>
		get_attribute_value{ interfaces::SDKManager::Get()->ReadSignature({ "CEconEntity", "AttributeHookValue" }, "float").get() };
	assert(get_attribute_value);
	return get_attribute_value(fail_value, attribute_name, pEnt, pItemList, is_global_string);
}


IEconItemSchema* IEconItemSchema::Get()
{
	static utils::FuncThunk<IEconItemSchema*> 
		get_item_schema{ interfaces::SDKManager::Get()->ReadSignature({ "CEconEntity" }, "GetItemSchema").get() };
	assert(get_item_schema);
	return get_item_schema();
}

TF2_NAMESPACE_END();

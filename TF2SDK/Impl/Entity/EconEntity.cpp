#include <Interfaces/GameData.hpp>

#include "Entity/EconEntity.hpp"
#include "Utils/Thunks.hpp"

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
	for (size_t i = 0; i < Attributes.size(); i++)
	{
		if (Attributes[i].AttributeDefinitionIndex == index)
		{
			Attributes.erase_fast(i); 
			break;
		}
	}
}

SG_SDK_TF2 int IAttributeList::AttributeHookValue(int fail_value, const char* attribute_name, const IBaseEntityInternal* pEnt, Utils::UtlVector<IBaseEntityInternal*>* pItemList, bool is_global_string)
{
	static Utils::IFuncThunk<int, int, const char*, const IBaseEntityInternal*, Utils::UtlVector<IBaseEntityInternal*>*, bool>
		get_attribute_value{ Interfaces::SDKManager::Get()->ReadSignature({ "CEconEntity", "AttributeHookValue" }, "int").get()};
	return get_attribute_value(fail_value, attribute_name, pEnt, pItemList, is_global_string);
}

SG_SDK_TF2 float IAttributeList::AttributeHookValue(float fail_value, const char* attribute_name, const IBaseEntityInternal* pEnt, Utils::UtlVector<IBaseEntityInternal*>* pItemList, bool is_global_string)
{
	static Utils::IFuncThunk<float, float, const char*, const IBaseEntityInternal*, Utils::UtlVector<IBaseEntityInternal*>*, bool>
		get_attribute_value{ Interfaces::SDKManager::Get()->ReadSignature({ "CEconEntity", "AttributeHookValue" }, "float").get() };
	return get_attribute_value(fail_value, attribute_name, pEnt, pItemList, is_global_string);
}


IEconItemSchema* IEconItemSchema::Get()
{
	static Utils::IFuncThunk<IEconItemSchema*> get_item_schema{ Interfaces::SDKManager::Get()->ReadSignature({ "CEconEntity" }, "GetItemSchema").get() };
	return get_item_schema();
}

TF2_NAMESPACE_END();

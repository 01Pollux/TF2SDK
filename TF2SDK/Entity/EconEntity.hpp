#pragma once

#include "BaseEntity.hpp"
#include "Utils/UtlVector.hpp"

TF2_NAMESPACE_BEGIN();

class EconItemAttribute
{
public:
	uint32_t*			VTable;
	uint16_t			AttributeDefinitionIndex;
	float				FloatValue;
	unsigned int		RefundableCurrency;

public:
	constexpr EconItemAttribute(uint16_t index, float value) noexcept :
		AttributeDefinitionIndex(index), FloatValue(value)
	{ }
};

class IAttributeList
{
public:
	uint32_t*							VTable;
	Utils::UtlVector<EconItemAttribute>	Attributes;

	/// <summary>
	/// set an existing attribute's value of index
	/// </summary>
	SG_SDK_TF2 void
		SetAttribute(int index, float value) noexcept;

	/// <summary>
	/// get an attribute by index
	/// </summary>
	SG_SDK_TF2 float
		GetAttribute(int index) noexcept;
	/// <summary>
	/// remove an attribute by index
	/// </summary>
	SG_SDK_TF2 void
		RemoveAttribute(int index) noexcept;

	/// <summary>
	/// Get attributes count
	/// </summary>
	int GetNumAttributes() const noexcept { return Attributes.Count(); }

	/// <summary>
	/// Get attribute value by name of an entity
	/// </summary>
	/// <param name="fail_value">default value to use if the attribute wasn't found</param>
	SG_SDK_TF2 static int
		AttributeHookValue(int fail_value, const char* attribute_name, const IBaseEntityInternal* pEnt, Utils::UtlVector<IBaseEntityInternal*>* pItemList = nullptr, bool is_global_string = true);

	/// <summary>
	/// Get attribute value by name of an entity
	/// </summary>
	/// <param name="fail_value">default value to use if the attribute wasn't found</param>
	SG_SDK_TF2 static float
		AttributeHookValue(float fail_value, const char* attribute_name, const IBaseEntityInternal* pEnt, Utils::UtlVector<IBaseEntityInternal*>* pItemList = nullptr, bool is_global_string = true);
};

namespace Const
{
	enum class UnusualWeaponEffect
	{
		Hot = 701,
		Isotope,
		Cool,
		EnergyOrb
	};

	enum class TFSheens
	{
		Team = 1,
		Deadly_Daffodil,
		Manndarin,
		Mean_Green,
		Agnoizing_Emerald,
		Villainous_Violet,
		Hot_Rod
	};
}


class IEconEntityInternal : public IBaseEntityInternal
{
public:
	SG_DECL_RECVPROP(IEconEntityInternal, int,				Const::EntClassID::CEconEntity, "m_iItemDefinitionIndex",				ItemDefinitionIndex,		0);
	SG_DECL_RECVPROP(IEconEntityInternal, IAttributeList,	Const::EntClassID::CEconEntity, "m_AttributeList",						AttributeList,				0);
	SG_DECL_RECVPROP(IEconEntityInternal, IAttributeList,	Const::EntClassID::CEconEntity, "m_NetworkedDynamicAttributesForDemos",	DynamicAttributeForDemos,	0);
};


class IEconItemSchema
{
public:
	/// <summary>
	/// Get game's Econ schema
	/// </summary>
	/// <returns></returns>
	SG_SDK_TF2 static IEconItemSchema* 
		Get();
};


class _IEconLootListDefinition
{
public:
	class _Iterator
	{
		virtual ~_Iterator() = default;
		virtual void OnIterate(uint16_t) abstract;
	};

	virtual ~_IEconLootListDefinition() = default;
	_NODISCARD virtual bool BPublicListContents() abstract;
	_NODISCARD virtual const char* GetLootListHeaderLocalizationKey() abstract;
	_NODISCARD virtual const char* GetLootListFooterLocalizationKey() abstract;
	_NODISCARD virtual const char* GetLootListCollectionReference() abstract;
	virtual void EnumerateUserFacingPotentialDrops(_Iterator*) abstract;
};

class IEconLootListDefinition : public _IEconLootListDefinition
{
public:
	struct DropPeriod
	{
		uint32_t	Start;
		uint32_t	End;
	};

	struct DropItem
	{
		int				ItemOrLootlistDef;
		float			Weight;
		DropPeriod		DropTime;
	};

	struct ExtraDropItem
	{
		float			Chance;
		bool			PremiumOnly;
		const char*		LootListDefName;
		int				RequiredHolidayIndex;
		DropPeriod		DropPeriod;
	};
};

TF2_NAMESPACE_END();

#pragma once

#include "RecvProp.hpp"
#include "SendProp.hpp"
#include "DataMap.hpp"

#include <type_traits>
#include <array>

TF2_NAMESPACE_BEGIN();

template<typename _TypeInfo>
class UnknowProp
{
public:
	using type = typename _TypeInfo::type;
	using typeinfo = _TypeInfo;
	using reference = type&;
	using const_reference = const type&;
	using pointer = type*;
	using const_pointer = const type*;

public:	
	UnknowProp() = delete;

	operator reference() noexcept				{ return get_ref(); }
	operator const_reference() const noexcept	{ return get_ref(); }

	template<typename _OTypeInfo>
	const_reference operator=(const UnknowProp<_OTypeInfo>& o) { return this->GetVarRef() = static_cast<const_reference>(o.GetVarConstRef()); }
	template<typename _Ty>
	const_reference operator=(const _Ty& o) { return (get_ref() = static_cast<const_reference>(o)); }


#define UNKNOWNPROP_IMPL_OPERATOR(SYMBOL)								\
	reference operator##SYMBOL##=(const UnknowProp& prop) noexcept		\
	{																	\
		get_ref() SYMBOL##= prop.get_ref();								\
		return *this;													\
	}																	\
	template<typename _Ty>												\
	reference operator##SYMBOL##=(const _Ty& o)	noexcept				\
	{																	\
		get_ref() SYMBOL##= static_cast<const_reference>(o);			\
		return *this;													\
	}																	\
	type operator##SYMBOL(const UnknowProp& prop) const noexcept		\
	{																	\
		return get_ref() SYMBOL prop.get_ref();							\
	}																	\
	template<typename _Ty>												\
	type operator##SYMBOL(const _Ty& o) const noexcept					\
	{																	\
		return get_ref() SYMBOL static_cast<const_reference>(o);		\
	}

	UNKNOWNPROP_IMPL_OPERATOR(+);
	UNKNOWNPROP_IMPL_OPERATOR(-);
	UNKNOWNPROP_IMPL_OPERATOR(/);
	UNKNOWNPROP_IMPL_OPERATOR(*);
	UNKNOWNPROP_IMPL_OPERATOR(%);
	UNKNOWNPROP_IMPL_OPERATOR(&);
	UNKNOWNPROP_IMPL_OPERATOR(|);
	UNKNOWNPROP_IMPL_OPERATOR(^);
	UNKNOWNPROP_IMPL_OPERATOR(<<);
	UNKNOWNPROP_IMPL_OPERATOR(>>);

#undef UNKNOWNPROP_IMPL_OPERATOR

	template<typename _Ty = type> _NODISCARD auto operator<=>(const _Ty& o) const noexcept	{ return get_ref() <=> static_cast<const_reference>(o); }
	template<typename _Ty = type> _NODISCARD bool operator!=(const _Ty& o) const noexcept	{ return get_ref() != static_cast<const_reference>(o); }

	template<typename _Ty = type> _NODISCARD auto operator<=>(const UnknowProp<_Ty>& prop) const noexcept { return get_ref() <=> static_cast<const_reference>(prop.get_ref()); }
	template<typename _Ty = type> _NODISCARD bool operator!=(const UnknowProp<_Ty>& prop) const noexcept { return get_ref() != static_cast<const_reference>(prop.get_ref()); }

	_NODISCARD auto operator+() const noexcept { return +this->get_ref(); }
	_NODISCARD auto operator-() const noexcept { return -this->get_ref(); }
	_NODISCARD auto operator~() const noexcept { return ~this->get_ref(); }

	auto operator++()		{ return ++get_ref(); }
	auto operator--()		{ return --get_ref(); }
	auto operator++(int)	{ return get_ref()++; }
	auto operator--(int)	{ return get_ref()--; }

	_NODISCARD const_pointer operator&() const noexcept			{ return this->get_ptr(); }
	_NODISCARD pointer operator&() noexcept						{ return this->get_ptr(); }

	_NODISCARD const_pointer operator->() const noexcept		{ return this->get_ptr(); }
	_NODISCARD pointer operator->() noexcept					{ return this->get_ptr(); }

	_NODISCARD const auto& operator[](int i) const noexcept		{ return get_ref()[i]; }
	_NODISCARD auto& operator[](int i) noexcept					{ return get_ref()[i]; }

	_NODISCARD auto begin() const noexcept
	{
		constexpr bool has_begin = requires(const type& v) { std::begin(v); };

		if constexpr (has_begin)
			return std::begin(get_ref());
		else
			return get_ptr();
	}

	_NODISCARD auto begin() noexcept
	{
		constexpr bool has_begin = requires(const type& v) { std::begin(v); };

		if constexpr (has_begin)
			return std::begin(get_ref());
		else
			return get_ptr();
	}

	_NODISCARD auto end() const noexcept
	{
		constexpr bool has_end = requires(const type& v) { std::end(v); };

		if constexpr (has_end)
			return std::end(get_ref());
		else
			return get_ptr() + sizeof(type);
	}

	_NODISCARD auto end() noexcept
	{
		constexpr bool has_end = requires(const type& v) { std::end(v); };

		if constexpr (has_end)
			return std::end(get_ref());
		else
			return get_ptr() + sizeof(type);
	}

	void set(const_reference o) noexcept { get_ref() = o; }

	_NODISCARD reference get() noexcept				{ return get_ref(); }
	_NODISCARD const_reference get() const noexcept { return get_ref(); }

	_NODISCARD pointer data() noexcept				{ return get_ptr(); }
	_NODISCARD const_pointer data() const noexcept { return get_ptr(); }

	template<typename _OTy>	_NODISCARD _OTy get() const noexcept		{ return std::bit_cast<_OTy>(get_ref()); }
	template<typename _OTy>	_NODISCARD _OTy* data() noexcept			{ return std::bit_cast<_OTy*>(get_ptr()); }
	template<typename _OTy>	_NODISCARD const _OTy* data() const noexcept	{ return std::bit_cast<const _OTy*>(get_ptr()); }

	_NODISCARD const ptrdiff_t offset() const noexcept	{ return get_offset(); }

	template<typename = std::enable_if_t<std::is_array_v<type>>>
	_NODISCARD constexpr size_t size() const noexcept	{ return sizeof(type) / sizeof(std::remove_extent_t<type>); }

	uintptr_t get_this() const noexcept					{ return std::bit_cast<uintptr_t>(this) - _TypeInfo::offset_to_this(); }
	uintptr_t get_this() noexcept						{ return std::bit_cast<uintptr_t>(this) - _TypeInfo::offset_to_this(); }

private:
	pointer get_ptr() noexcept
	{
		return std::bit_cast<pointer>(get_this() + get_offset());
	}

	const_pointer get_ptr() const noexcept
	{
		return std::bit_cast<const_pointer>(get_this() + get_offset());
	}

	reference get_ref() noexcept
	{
		return *get_ptr();
	}

	const_reference get_ref() const noexcept
	{
		return *get_ptr();
	}

	uint32_t get_offset() const noexcept
	{
		if constexpr (_TypeInfo::use_this_ptr)
		{
			static uint32_t offset = _TypeInfo::offset_to_prop(get_this());
			return offset;
		}
		else
		{
			static uint32_t offset = _TypeInfo::offset_to_prop();
			return offset;
		}
	}
};



#define SG_DECL_UNKOWN_PROP_A(Class, Type, Size, GETTER, CustomName, EXTRA)			\
struct TypeInfo_##CustomName##Class													\
{																					\
	using type = std::conditional_t<(Size > 0), std::array<Type, Size>, Type>;		\
	static constexpr bool use_this_ptr = false;										\
	static constexpr size_t offset_to_this() noexcept								\
	{																				\
		return offsetof(Class, CustomName);											\
	}																				\
	static uint32_t offset_to_prop() noexcept										\
	{																				\
		return GETTER + EXTRA;														\
	}																				\
	static uint32_t offset_to_prop(uintptr_t) noexcept								\
	{																				\
		return 0;																	\
	}																				\
};																					\
UnknowProp<TypeInfo_##CustomName##Class> CustomName

#define SG_DECL_UNKOWN_PROP(Class, Type, GETTER, CustomName, EXTRA)					\
		SG_DECL_UNKOWN_PROP_A(Class, Type, 0, GETTER, CustomName, EXTRA)


#define SG_DECL_UNKOWN_PROP_THIS_A(Class, Type, Size, GETTER, CustomName, EXTRA)	\
struct TypeInfo_##CustomName##Class													\
{																					\
	using type = std::conditional_t<(Size > 0), std::array<Type, Size>, Type>;		\
	static constexpr bool use_this_ptr = true;										\
	static constexpr size_t offset_to_this() noexcept								\
	{																				\
		return offsetof(Class, CustomName);											\
	}																				\
	static uint32_t offset_to_prop() noexcept										\
	{																				\
		return 0;																	\
	}																				\
	static uint32_t offset_to_prop(uintptr_t pthis) noexcept						\
	{																				\
		return GETTER + EXTRA;														\
	}																				\
};																					\
UnknowProp<TypeInfo_##CustomName##Class> CustomName

#define SG_DECL_UNKOWN_PROP_THIS(Class, Type, GETTER, CustomName, EXTRA)			\
		SG_DECL_UNKOWN_PROP_THIS_A(Class, Type, 0, GETTER, CustomName, EXTRA)		\


#define SG_DECL_RECVPROP_A(Class, Type, Size, Class_Name, Prop_Name, Custom_Name, ExtraOffset)	\
SG_DECL_UNKOWN_PROP_A(																			\
	Class, 																						\
	Type,																						\
	Size,																						\
	[]()																						\
	{																							\
		uint32_t offset{ };																		\
		PropFinder::FindRecvProp(Class_Name, Prop_Name, nullptr, &offset);						\
		return offset;																			\
	}(),																						\
	Custom_Name,																				\
	ExtraOffset																					\
)

#define SG_DECL_RECVPROP(Class, Type, Class_Name, Prop_Name, Custom_Name, ExtraOffset)			\
		SG_DECL_RECVPROP_A(Class, Type, 0, Class_Name, Prop_Name, Custom_Name, ExtraOffset)


#define SG_DECL_DATAMAP_A(Class, Type, Size, Is_Pred, Prop_Name, Custom_Name, ExtraOffset)	\
SG_DECL_UNKOWN_PROP_THIS_A(																	\
	Class, 																					\
	Type, 																					\
	Size, 																					\
	[pthis](auto datamap)																	\
	{																						\
		uint32_t offset{ };																	\
		PropFinder::FindDataMap(datamap, Prop_Name, nullptr, &offset);						\
		return offset;																		\
	}(((Class*)(pthis))->GetDataMap(Is_Pred)),												\
	Custom_Name,																			\
	ExtraOffset																				\
)


#define SG_DECL_DATAMAP(Class, Type, Is_Pred, Prop_Name, Custom_Name, ExtraOffset)			\
		SG_DECL_DATAMAP_A(Class, Type, 0, Is_Pred, Prop_Name, Custom_Name, ExtraOffset)


TF2_NAMESPACE_END();

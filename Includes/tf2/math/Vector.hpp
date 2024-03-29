#pragma once

#include <type_traits>
#include <array>
#include <cmath>

#include <tf2/config.hpp>
#include <px/console/args.hpp>

TF2_NAMESPACE_BEGIN();

template<typename _Ty, size_t _Size>
class VectorXD
{
public:
	using value_type		= _Ty;
	using reference			= value_type&;
	using const_reference	= const value_type&;
	using pointer			= _Ty*;
	using const_pointer		= const _Ty*;

	using array_type				= std::array<value_type, _Size>;
	using difference_type			= array_type::difference_type;

	using iterator					= array_type::iterator;
	using const_iterator			= array_type::const_iterator;

	using reverse_iterator			= array_type::reverse_iterator;
	using const_reverse_iterator	= array_type::const_reverse_iterator;

	constexpr VectorXD() = default;

	constexpr VectorXD(std::initializer_list<value_type> list) noexcept
	{
		std::copy_n(list.begin(), std::min(list.size(), _Size), begin());
	}

	constexpr VectorXD(const array_type& arr) noexcept : m_Data{ arr } { }

	template<typename _OTy, size_t _OSize>
	requires std::negation_v<std::is_same<VectorXD, VectorXD<_OTy, _OSize>>>
	constexpr VectorXD(const VectorXD<_OTy, _OSize>& vec) noexcept 
	{
		std::copy_n(vec.begin(), std::min(_OSize, _Size), begin());
	}

	[[nodiscard]] constexpr bool is_valid() const noexcept
	{
		if constexpr (std::is_floating_point_v<value_type>)
		{
			constexpr value_type nan = std::numeric_limits<value_type>::quiet_NaN();
			for (value_type x : m_Data)
			{
				if (x == nan)
					return false;
			}
		}
		return true;
	}

	[[nodiscard]] constexpr const_reference operator[](difference_type i) const noexcept
	{
		return m_Data[i];
	}
	[[nodiscard]] constexpr reference operator[](difference_type i) noexcept
	{
		return m_Data[i];
	}

	[[nodiscard]] constexpr const_pointer data() const noexcept
	{
		return m_Data.data();
	}

	[[nodiscard]] pointer data() noexcept
	{
		return m_Data.data();
	}
	
	[[nodiscard]] constexpr const array_type& get() const noexcept
	{
		return m_Data;
	}

	[[nodiscard]] array_type& get() noexcept
	{
		return m_Data;
	}

	[[nodiscard]] constexpr size_t size() const noexcept
	{
		return _Size;
	}

	template<typename _OTy>
		requires (sizeof(_OTy) == sizeof(_Ty) * _Size)
	[[nodiscard]] constexpr const _OTy& to() const noexcept
	{
		return *std::bit_cast<const _OTy*>(data());
	}

	template<typename _OTy>
		requires (sizeof(_OTy) == sizeof(_Ty) * _Size)
	[[nodiscard]] _OTy& to() noexcept
	{
		return *std::bit_cast<_OTy*>(data());
	}

	constexpr auto operator<=>(const VectorXD&) const = default;

	void negate() noexcept
	{
		for (reference v : m_Data)
			v = -v;
	}

	[[nodiscard]] constexpr float dot(const VectorXD& other) const noexcept
	{
		float res = 0.;
		for (
			auto meit = cbegin(), oit = other.cbegin();
			meit != cend();
			meit++, oit++
		)
		{
			res += static_cast<float>(*meit) * *oit;
		}
		return res;
	}

	[[nodiscard]] constexpr float length_sqr() const noexcept
	{
		return dot(*this);
	}

	[[nodiscard]] constexpr float length() const noexcept
	{
		return std::sqrt(length_sqr());
	}

	[[nodiscard]] constexpr bool is_length_greater_than(value_type val) const noexcept
	{
		return length_sqr() > (val * val);
	}

	[[nodiscard]] constexpr bool is_length_less_than(value_type val) const noexcept
	{
		return length_sqr() < (val * val);
	}

	[[nodiscard]] constexpr bool is_zero(value_type tolerance = std::numeric_limits<value_type>::epsilon()) const noexcept
	{
		for (value_type v : m_Data)
		{
			if (v < -tolerance || v > tolerance)
				return false;
		}
		return true;
	}

	float normalize() noexcept
	{
		float len = this->length();
		if (len)
		{
			this->operator/=(static_cast<value_type>(len));
			return len;
		}
		else
		{
			fill(static_cast<value_type>(0));
			return 0.f;
		}
	}

	[[nodiscard]] constexpr bool within(const VectorXD& min, const VectorXD& max) const noexcept
	{
		for (
			auto meit = cbegin(), minit = min.cbegin(), maxit = max.cbegin();
			meit != cend();
			meit++, minit++, maxit++
		)
		{
			if (*meit < *minit || *meit > *maxit)
				return false;
		}
		return true;
	}
	
	[[nodiscard]] constexpr float distance_to(const VectorXD& other) const noexcept
	{
		return (other - *this).length();
	}

	[[nodiscard]] constexpr float distance_to_sqr(const VectorXD& other) const noexcept
	{
		return (other - *this).length_sqr();
	}

	void unsafe_copy_from(const_pointer pval)
	{
		for (reference v : m_Data)
		{
			v = *pval;
			++pval;
		}
	}

	void unsafe_copy_to(pointer pval) const
	{
		for (const_reference v : m_Data)
		{
			*pval = v;
			++pval;
		}
	}

	// Multiply, add, and assign to this (ie: *this = a + b * scalar). This
	void mult_add(const VectorXD& a, const VectorXD& b, float scalar) noexcept
	{
		iterator meit = begin();
		for (
			auto ait = a.cbegin(), bit = b.cbegin();
			meit != cend();
			meit++, ait++, bit++
		)
		{
			*meit = *ait + static_cast<value_type>(*bit * scalar);
		}
	}

public:
	constexpr void fill(const_reference val)
	{
		m_Data.fill(val);
	}

	constexpr void swap(array_type& other) noexcept(std::is_nothrow_swappable_v<_Ty>)
	{
		m_Data.swap(other);
	}

	[[nodiscard]] constexpr iterator begin() noexcept
	{
		return m_Data.begin();
	}

	[[nodiscard]] constexpr const_iterator begin() const noexcept
	{
		return m_Data.begin();
	}

	[[nodiscard]] constexpr iterator end() noexcept
	{
		return m_Data.end();
	}

	[[nodiscard]] constexpr const_iterator end() const noexcept
	{
		return m_Data.end();
	}

	[[nodiscard]] constexpr reverse_iterator rbegin() noexcept
	{
		return m_Data.rbegin();
	}

	[[nodiscard]] constexpr const_reverse_iterator rbegin() const noexcept
	{
		return m_Data.rbegin();
	}

	[[nodiscard]] constexpr reverse_iterator rend() noexcept
	{
		return m_Data.rend();
	}

	[[nodiscard]] constexpr const_reverse_iterator rend() const noexcept
	{
		return m_Data.rend();
	}

	[[nodiscard]] constexpr const_iterator cbegin() const noexcept
	{
		return begin();
	}

	[[nodiscard]] constexpr const_iterator cend() const noexcept
	{
		return end();
	}

	[[nodiscard]] constexpr const_reverse_iterator crbegin() const noexcept
	{
		return rbegin();
	}

	[[nodiscard]] constexpr const_reverse_iterator crend() const noexcept
	{
		return rend();
	}


public:
#define VECTORXD_IMPL_MATH_OP(SYMBOL)														\
	constexpr VectorXD& operator##SYMBOL##=(const VectorXD& other) noexcept					\
	{																						\
		auto meit = begin();																\
		for (auto oit = other.begin(); meit != end(); meit++, oit++)						\
		{																					\
			*meit SYMBOL##= *oit;															\
		}																					\
		return *this;																		\
	}																						\
	template<typename _VTy>																	\
	constexpr VectorXD& operator##SYMBOL##=(_VTy other) noexcept							\
	{																						\
		for (reference v : m_Data)															\
		{																					\
			v SYMBOL##= static_cast<value_type>(other);										\
		}																					\
		return *this;																		\
	}																						\
	[[nodiscard]] constexpr VectorXD operator##SYMBOL(const VectorXD& other) const noexcept	\
	{																						\
		VectorXD res{ };																	\
		auto rit = res.begin();																\
		for (																				\
			auto meit = begin(), oit = other.begin();										\
			meit != end();																	\
			meit++, oit++, rit++															\
		)																					\
		{																					\
			*rit = *meit SYMBOL *oit;														\
		}																					\
		return res;																			\
	}																						\
	template<typename _VTy>																	\
	[[nodiscard]] constexpr VectorXD operator##SYMBOL(_VTy other) const noexcept			\
	{																						\
		VectorXD res{ };																	\
		auto rit = res.begin();																\
		for (																				\
			auto meit = begin();															\
			meit != end();																	\
			meit++, rit++																	\
		)																					\
		{																					\
			*rit = *meit SYMBOL static_cast<value_type>(other);								\
		}																					\
		return res;																			\
	}

	VECTORXD_IMPL_MATH_OP(+);
	VECTORXD_IMPL_MATH_OP(-);
	VECTORXD_IMPL_MATH_OP(/);
	VECTORXD_IMPL_MATH_OP(*);
	VECTORXD_IMPL_MATH_OP(%);
	VECTORXD_IMPL_MATH_OP(&);
	VECTORXD_IMPL_MATH_OP(| );
	VECTORXD_IMPL_MATH_OP(^);
	VECTORXD_IMPL_MATH_OP(<<);
	VECTORXD_IMPL_MATH_OP(>>);


#undef VECTORXD_IMPL_MATH_OP

	VectorXD& operator=(value_type val) noexcept
	{
		for (auto meit = begin(); meit != end(); meit++)
		{
			*meit = val;
		}
		return *this;
	}

	[[nodiscard]] constexpr VectorXD operator-() const noexcept
	{
		VectorXD res{ };
		auto rit = res.begin();
		for (auto meit = cbegin(); meit != cend(); meit++, rit++)
		{
			*rit = -*meit;
		}
		return res;
	}

	[[nodiscard]] constexpr VectorXD min(const VectorXD& other) const noexcept
	{
		VectorXD res{ };
		auto rit = res.begin();
		for (auto meit = cbegin(), oit = other.cbegin(); meit != cend(); meit++, oit++, rit++)
		{
			*rit = *meit < *oit ? *meit : *oit;
		}
		return res;
	}

	[[nodiscard]] constexpr VectorXD max(const VectorXD& other) const noexcept
	{
		VectorXD res{ };
		auto rit = res.begin();
		for (auto meit = cbegin(), oit = other.cbegin(); meit != cend(); meit++, oit++, rit++)
		{
			*rit = *meit > *oit ? *meit : *oit;
		}
		return res;
	}

private:
	array_type m_Data{ };
};

template<typename _Ty, typename... _Rest>
VectorXD(_Ty, _Rest...) -> VectorXD<typename std::_Enforce_same<_Ty, _Rest...>::type, 1 + sizeof ...(_Rest)>;

using Vector2D_I = VectorXD<int, 2>;
using Vector3D_I = VectorXD<int, 3>;
using Vector4D_I = VectorXD<int, 4>;

using Vector2D_F = VectorXD<float, 2>;
using Vector3D_F = VectorXD<float, 3>;
using Vector4D_F = VectorXD<float, 4>;

using Vector2D_D = VectorXD<double, 2>;
using Vector3D_D = VectorXD<double, 3>;
using Vector4D_D = VectorXD<double, 4>;

using Angle_I = Vector3D_I;
using Angle_F = Vector3D_F;
using Angle_D = Vector3D_D;

using Quaternion_I = Vector4D_I;
using Quaternion_F = Vector4D_F;
using Quaternion_D = Vector4D_D;

using Color3_8		= VectorXD<uint8_t, 3>;
using Color3_F		= VectorXD<float, 3>;

using Color4_8		= VectorXD<uint8_t, 4>;
using Color4_F		= VectorXD<float, 4>;

TF2_NAMESPACE_END();

PX_CONVAR_TYPEINFO_INHERIT(px::tf2::Vector2D_I, px::tf2::Vector2D_I::array_type);
PX_CONVAR_TYPEINFO_INHERIT(px::tf2::Vector3D_I, px::tf2::Vector2D_I::array_type);
PX_CONVAR_TYPEINFO_INHERIT(px::tf2::Vector4D_I, px::tf2::Vector2D_I::array_type);

PX_CONVAR_TYPEINFO_INHERIT(px::tf2::Vector2D_F, px::tf2::Vector2D_F::array_type);
PX_CONVAR_TYPEINFO_INHERIT(px::tf2::Vector3D_F, px::tf2::Vector3D_F::array_type);
PX_CONVAR_TYPEINFO_INHERIT(px::tf2::Vector4D_F, px::tf2::Vector4D_F::array_type);

PX_CONVAR_TYPEINFO_INHERIT(px::tf2::Vector2D_D, px::tf2::Vector2D_D::array_type);
PX_CONVAR_TYPEINFO_INHERIT(px::tf2::Vector3D_D, px::tf2::Vector3D_D::array_type);
PX_CONVAR_TYPEINFO_INHERIT(px::tf2::Vector4D_D, px::tf2::Vector4D_D::array_type);

PX_CONVAR_TYPEINFO_INHERIT(px::tf2::Color3_8, px::tf2::Color3_8::array_type);
PX_CONVAR_TYPEINFO_INHERIT(px::tf2::Color4_8, px::tf2::Color4_8::array_type);
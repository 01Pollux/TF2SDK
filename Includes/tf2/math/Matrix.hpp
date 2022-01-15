#pragma once

#include "Vector.hpp"

TF2_NAMESPACE_BEGIN();

template<typename _Ty, size_t _Size0, size_t _Size1>
class MatrixAxB
{
public:
	using element_type		= VectorXD<_Ty, _Size0>;
	using reference			= element_type&;
	using const_reference	= const element_type&;
	using pointer			= element_type*;
	using const_pointer		= const element_type*;

	using array_type		= std::array<element_type, _Size1>;
	using difference_type	= array_type::difference_type;

	using iterator = array_type::iterator;
	using const_iterator = array_type::const_iterator;

	using reverse_iterator = array_type::reverse_iterator;
	using const_reverse_iterator = array_type::const_reverse_iterator;

	constexpr MatrixAxB() = default;

	constexpr MatrixAxB(const array_type& other) noexcept
	{
		auto meit = m_Data.begin();
		for (auto oit = other.begin(); oit != other.end(); oit++, meit++)
		{
			*meit = *oit;
		}
	}

	constexpr MatrixAxB(const std::initializer_list<typename element_type::array_type>& other) noexcept
	{
		std::copy_n(other.begin(), _Size1, m_Data.begin());
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
		m_Data.data();
	}
	[[nodiscard]] constexpr pointer data() noexcept
	{
		m_Data.data();
	}

public:
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
	[[nodiscard]] constexpr element_type get_column(difference_type col_idx) const noexcept
	{
		element_type col{ };
		auto colit = col.begin();
		for (auto meit = begin(); meit != end(); meit++, colit++)
		{
			*colit = (*meit)[col_idx];
		}
		return col;
	}

	void set_column(difference_type col_idx, const element_type& col)
	{
		auto colit = col.begin();
		for (auto meit = begin(); meit != end(); meit++, colit++)
		{
			(*meit)[col_idx] = *colit;
		}
	}

	[[nodiscard]] constexpr const element_type& get_row(difference_type row_idx) const noexcept
	{
		return m_Data[row_idx];
	}

	void set_row(difference_type row_idx, const element_type& row) noexcept
	{
		m_Data[row_idx] = row;
	}

	[[nodiscard]] constexpr MatrixAxB transpose() const noexcept
	{
		MatrixAxB matx{ };
		constexpr size_t smallest = _Size0 < _Size1 ? _Size0 : _Size1;
		for (size_t i = 0; i < smallest; i++)
		{
			for (size_t j = 0; j < smallest; j++)
				matx[j][i] = m_Data[i][j];
		}
		return matx;
	}

	[[nodiscard]] constexpr MatrixAxB mult(const MatrixAxB& other) const noexcept
	{
		MatrixAxB matx{ };
		constexpr size_t smallest = _Size0 < _Size1 ? _Size0 : _Size1;
		for (size_t i = 0; i < _Size1; i++)
		{
			for (size_t j = 0; j < _Size0; j++)
			{
				for (size_t k = 0; k < smallest; k++)
				{
					matx[i][j] += m_Data[i][k] * other[k][j];
				}
			}
		}
		return matx;
	}

private:
	array_type m_Data{ };
};


using Matrix3x2_I = MatrixAxB<int, 3, 2>;
using Matrix3x2_F = MatrixAxB<float, 3, 2>;
using Matrix3x2_D = MatrixAxB<double, 3, 2>;

using Matrix3x3_I = MatrixAxB<int, 3, 3>;
using Matrix3x3_F = MatrixAxB<float, 3, 3>;
using Matrix3x3_D = MatrixAxB<double, 3, 3>;

using Matrix3x4_I = MatrixAxB<int, 3, 4>;
using Matrix3x4_F = MatrixAxB<float, 3, 4>;
using Matrix3x4_D = MatrixAxB<double, 3, 4>;

using Matrix2x2_I = MatrixAxB<int, 2, 2>;
using Matrix2x2_F = MatrixAxB<float, 2, 2>;
using Matrix2x2_D = MatrixAxB<double, 2, 2>;

using Matrix2x3_I = MatrixAxB<int, 2, 3>;
using Matrix2x3_F = MatrixAxB<float, 2, 3>;
using Matrix2x3_D = MatrixAxB<double, 2, 3>;

using Matrix2x4_I = MatrixAxB<int, 2, 4>;
using Matrix2x4_F = MatrixAxB<float, 2, 4>;
using Matrix2x4_D = MatrixAxB<double, 2, 4>;

using Matrix4x2_I = MatrixAxB<int, 4, 2>;
using Matrix4x2_F = MatrixAxB<float, 4, 2>;
using Matrix4x2_D = MatrixAxB<double, 4, 2>;

using Matrix4x3_I = MatrixAxB<int, 4, 3>;
using Matrix4x3_F = MatrixAxB<float, 4, 3>;
using Matrix4x3_D = MatrixAxB<double, 4, 3>;

using Matrix4x4_I = MatrixAxB<int, 4, 4>;
using Matrix4x4_F = MatrixAxB<float, 4, 4>;
using Matrix4x4_D = MatrixAxB<double, 4, 4>;

TF2_NAMESPACE_END();
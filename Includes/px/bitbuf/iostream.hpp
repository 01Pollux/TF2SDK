#pragma once

#include <iostream>
#include "traits.hpp"

template<typename _Ty>
    requires (std::is_base_of_v<std::basic_ios<typename _Ty::char_type, typename _Ty::traits_type>, _Ty>)
inline _Ty& operator<<(_Ty& os, px::bit_type bit)
{
    os << (bit == px::bit_type::zero ? '0' : '1');
    return os;
}

template<typename _Ty>
    requires (std::is_base_of_v<std::basic_ios<typename _Ty::char_type, typename _Ty::traits_type>, _Ty>)
inline _Ty& operator>>(_Ty& os, px::bit_type& outbit)
{
    char bit;
    os >> bit;
    outbit = bit == '1' ? px::bit_type::one : px::bit_type::zero;
    return os;
}

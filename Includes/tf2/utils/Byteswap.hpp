#pragma once

#include <bit>
#include "TF2Config.hpp"

TF2_NAMESPACE_BEGIN(::utils);


class ByteSwap
{
public:
	ByteSwap()
	{
		set_target_endian(is_machine_big_endian());
	}

	static constexpr bool is_machine_big_endian() noexcept
	{
		return std::endian::native == std::endian::big;
	}

	void set_target_endian(bool big)
	{
		m_IsBigEndian = big;
		m_IsSwappingBytes = is_machine_big_endian() != big;
	}

	void flip_endian()
	{
		m_IsBigEndian = !m_IsBigEndian;
		m_IsSwappingBytes = !m_IsSwappingBytes;
	}

	void activate_byteswapping(bool state)
	{
		set_target_endian(is_machine_big_endian() != state);
	}

	//-----------------------------------------------------------------------------
	// Returns true if the target machine is the same as this one in endianness.
	//
	// Used to determine when a byteswap needs to take place.
	//-----------------------------------------------------------------------------
	bool is_swapping_bytes() const noexcept
	{
		return m_IsSwappingBytes;
	}

	bool is_big_endian() const noexcept
	{
		return m_IsBigEndian;
	}
	
	//-----------------------------------------------------------------------------
	// IsByteSwapped()
	//
	// When supplied with a chunk of input data and a constant or magic number
	// (in native format) determines the endienness of the current machine in
	// relation to the given input data.
	//
	// Returns:
	//		1  if input is the same as nativeConstant.
	//		0  if input is byteswapped relative to nativeConstant.
	//		-1 if input is not the same as nativeConstant and not byteswapped either.
	//
	// ( This is useful for detecting byteswapping in magic numbers in structure 
	// headers for example. )
	//-----------------------------------------------------------------------------
	template<typename _Ty>
	int is_source_native_endian(const _Ty& input, const _Ty& nativeConstant)
	{
		// If it's the same, it isn't byteswapped:
		if (input == nativeConstant)
			return 1;

		_Ty output;
		byte_swap_internal(&output, &input);
		return output == nativeConstant ? 0 : -1;
	}

	//-----------------------------------------------------------------------------
	// Swaps an input buffer full of type T into the given output buffer.
	//
	// Swaps [count] items from the inputBuffer to the outputBuffer.
	// If inputBuffer is omitted or NULL, then it is assumed to be the same as
	// outputBuffer - effectively swapping the contents of the buffer in place.
	//-----------------------------------------------------------------------------
	template<typename _Ty> 
	void swap_buffer(_Ty* outputBuffer, const _Ty* inputBuffer = nullptr, size_t count = 1)
	{
		// Fail gracefully in release:
		if (count <= 0 || !outputBuffer)
			return;

		// Optimization for the case when we are swapping in place.
		if (!inputBuffer)
			inputBuffer = outputBuffer;

		// Swap everything in the buffer:
		for (size_t i = 0; i < count; i++)
			byte_swap_internal(&outputBuffer[i], &inputBuffer[i]);
	}
	
	//-----------------------------------------------------------------------------
	// Swaps an input buffer full of type T into the given output buffer.
	//
	// Swaps [count] items from the inputBuffer to the outputBuffer.
	// If inputBuffer is omitted or NULL, then it is assumed to be the same as
	// outputBuffer - effectively swapping the contents of the buffer in place.
	//-----------------------------------------------------------------------------
	template<typename _Ty> 
	void swap_buffer_to_target_endian(_Ty* outputBuffer, const _Ty* inputBuffer = nullptr, int count = 1)
	{
		// Fail gracefully in release:
		if (count <= 0 || !outputBuffer)
			return;

		// Optimization for the case when we are swapping in place.
		if (!inputBuffer)
			inputBuffer = outputBuffer;

		// Are we already the correct endienness? ( or are we swapping 1 byte items? )
		if (!is_swapping_bytes() || (sizeof(_Ty) == 1))
		{
			// If we were just going to swap in place then return.
			if (!inputBuffer)
				return;

			// Otherwise copy the inputBuffer to the outputBuffer:
			memcpy(outputBuffer, inputBuffer, count * sizeof(_Ty));
			return;

		}

		// Swap everything in the buffer:
		for (int i = 0; i < count; i++)
			byte_swap_internal(&outputBuffer[i], &inputBuffer[i]);
	}

private:
	//-----------------------------------------------------------------------------
	// The lowest level byte swapping workhorse of doom.  output always contains the 
	// swapped version of input.  ( Doesn't compare machine to target endianness )
	//-----------------------------------------------------------------------------
	template<typename _Ty> 
	static void byte_swap_internal(_Ty* output, const _Ty* input)
	{
		for (size_t i = 0; i < sizeof(_Ty); i++)
			std::bit_cast<uint8_t*>(output)[i] = std::bit_cast<uint8_t*>(input)[sizeof(_Ty) - (i + 1)];
	}

	uint32_t m_IsSwappingBytes : 1;
	uint32_t m_IsBigEndian : 1;
};


TF2_NAMESPACE_END();

#pragma once

#include <tf2/config.hpp>

TF2_NAMESPACE_BEGIN(::Utils);

class bf_write
{
public:
	bf_write() = default;

	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	bf_write(void* pData, int nBytes, int nMaxBits = -1) noexcept { start_writing(pData, nBytes, 0, nMaxBits); }
	bf_write(const char* pDebugName, void* pData, int nBytes, int nMaxBits = -1) noexcept : DebugName(pDebugName) { start_writing(pData, nBytes, 0, nMaxBits); }

	// Start writing to the specified buffer.
	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	SG_SDK_TF2 void 
		start_writing(void* pData, int nBytes, int iStartBit = 0, int nMaxBits = -1);

	void reset() noexcept { CurBit = IsOverflow = 0; }

	void set_assert(bool bassert) noexcept { AssertOnOverflow = bassert; }

	const char* get_name() const noexcept { return DebugName; }
	void set_name(const char* dbgname) noexcept { DebugName = dbgname; }

	void seek(int bitPos) { CurBit = bitPos; }

	void write_bit(int bit)
	{
		if (!check_for_overflow(1))
			write_bit_nocheck(bit);
	}
	SG_SDK_TF2 void
		write_bit_nocheck(int nValue);
	SG_SDK_TF2 void
		write_bit_at(int iBit, int nValue);

	// Write signed or unsigned. Range is only checked in debug.
	SG_SDK_TF2 void
		write_ubit(unsigned int data, int numbits);
	SG_SDK_TF2 void
		write_sbit(int data, int numbits);

	// Write a list of bits in.
	SG_SDK_TF2 bool
		write_bits(const void* in_data, int numbits);

	// writes a varint encoded integer
	SG_SDK_TF2 void
		write_uint32(uint32_t data);
	SG_SDK_TF2 void
		write_uint64(uint64_t data);
	SG_SDK_TF2 void
		write_sint32(int32_t data);
	SG_SDK_TF2 void
		write_sint64(int64_t data);

	SG_SDK_TF2 void
		write_angle(float fAngle, int numbits);
	SG_SDK_TF2 void
		write_coord(const float f);
	SG_SDK_TF2 void
		write_vec3(const float fa[3]);

	// Byte functions.
public:

	SG_SDK_TF2 void
		write_char(int8_t val);
	SG_SDK_TF2 void
		write_byte(uint8_t val);
	SG_SDK_TF2 void
		write_short(int16_t val);
	SG_SDK_TF2 void
		write_word(uint16_t val);
	SG_SDK_TF2 void
		write_long(int32_t val);
	SG_SDK_TF2 void
		write_longlong(int64_t val);
	void write_float(float val)
	{
		write_bits(&val, sizeof(float) << 3);
	}
	bool write_bytes(const void* pBuf, int nBytes)
	{
		return write_bits(pBuf, nBytes << 3);
	}

	// Returns false if it overflows the buffer.
	SG_SDK_TF2 bool
		write_string(const char* str);

	// Status.
public:
	int	bits_written()	const noexcept { return CurBit; }
	int	bytes_written() const noexcept { return (bits_written() + 7) >> 3; }
	int	max_bits()		const noexcept { return DataBits; }
	int	bits_left()		const noexcept { return max_bits() - bits_written(); }
	int	bytes_left()	const noexcept { return bits_left() >> 3; }
	uint8_t* data()				noexcept { return std::bit_cast<uint8_t*>(Data); }
	const uint8_t* data() const noexcept { return std::bit_cast<const uint8_t*>(Data); }

	// Has the buffer overflowed?
	bool check_for_overflow(int nBits)
	{
		if (CurBit + nBits > DataBits)
			mark_as_overflowed();
		return IsOverflow;
	}

	bool has_overflown() const noexcept { return IsOverflow; }
	void mark_as_overflowed() noexcept { IsOverflow = true; }

private:
	unsigned long*	Data{ };
	int				DataBytes{ };
	int				DataBits{ -1 };

	int				CurBit{ };
	bool			IsOverflow{ };

	bool			AssertOnOverflow{ true };
	const char*		DebugName{ };
};



class bf_read
{
public:
	bf_read() = default;

	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	bf_read(const void* pData, int nBytes, int nBits = -1) noexcept { start_reading(pData, nBytes, nBits); }
	bf_read(const char* pDebugName, const void* pData, int nBytes, int nBits = -1) noexcept : DebugName(pDebugName) { start_reading(pData, nBytes, nBits); }

	// Start reading from the specified buffer.
	// pData's start address must be dword-aligned.
	// nMaxBits can be used as the number of bits in the buffer. 
	// It must be <= nBytes*8. If you leave it at -1, then it's set to nBytes * 8.
	void start_reading(const void* pData, int nBytes, int iStartBit = 0, int nBits = -1);

	void reset() noexcept { CurBit = IsOverflow = 0; }

	void set_assert(bool bassert) noexcept { AssertOnOverflow = bassert; }

	const char* get_name() const noexcept { return DebugName; }
	void set_name(const char* dbgname) noexcept { DebugName = dbgname; }

	bool seek(int bitPos) 
	{ 
		if (bitPos < 0 || bitPos >= DataBits)
			return false;
		CurBit = bitPos;
		return true;
	}
	bool seek_relative(int bitPos) { return seek(CurBit + bitPos); }

	SG_SDK_TF2 int
		read_bit();

public:
	// Read a list of bits in.
	SG_SDK_TF2 void
		read_bits(void* pOut, int nBits);

	// Read a list of bits in, but don't overrun the destination buffer.
	// Returns the number of bits read into the buffer. The remaining
	// bits are skipped over.
	SG_SDK_TF2 float
		read_angle(int numbits);

	SG_SDK_TF2 uint32_t
		peek_ubit(int numbits);
	SG_SDK_TF2 uint32_t
		read_ubit(int numbits);
	SG_SDK_TF2 int
		read_sbit(int numbits);

	// reads a varint encoded integer
	SG_SDK_TF2 uint32_t
		read_uint32();
	SG_SDK_TF2 uint64_t
		read_uint64();
	SG_SDK_TF2 int32_t
		read_int32();
	SG_SDK_TF2 int64_t
		read_int64();

	SG_SDK_TF2 float 
		read_coord();
	SG_SDK_TF2 void
		read_vec3(float fa[3]);

	// Byte functions (these still read data in bit-by-bit).
public:

	inline int8_t	read_char() { return static_cast<int8_t>(read_ubit(8)); }
	inline uint8_t	read_byte() { return static_cast<uint8_t>(read_ubit(16)); }
	inline int16_t	read_short() { return static_cast<int16_t>(read_ubit(16)); }
	inline uint16_t read_word() { return static_cast<uint16_t>(read_ubit(16)); }
	inline int32_t	read_long() { return static_cast<int32_t>(read_ubit(32)); }
	SG_SDK_TF2 int64_t read_longlong();
	float			read_float()
	{
		float ret;
		read_bits(&ret, 32);
		return ret;
	}
	bool			read_bytes(void* pOut, int nBytes)
	{
		read_bits(pOut, nBytes << 3);
		return !has_overflown();
	}

	// Returns false if bufLen isn't large enough to hold the
	// string in the buffer.
	//
	// Always reads to the end of the string (so you can read the
	// next piece of data waiting).
	//
	// If bLine is true, it stops when it reaches a '\n' or a null-terminator.
	//
	// pStr is always null-terminated (unless bufLen is 0).
	//
	// pOutNumChars is set to the number of characters left in pStr when the routine is 
	// complete (this will never exceed bufLen-1).
	//
	bool read_string(char* pStr, int bufLen, bool bLine = false, int* pOutNumChars = NULL);

	// Status.
public:
	int	bits_written()	const noexcept { return CurBit; }
	int	bytes_written() const noexcept { return (bits_written() + 7) >> 3; }
	int	max_bits()		const noexcept { return DataBits; }
	int	bits_left()		const noexcept { return max_bits() - bits_written(); }
	int	bytes_left()	const noexcept { return bits_left() >> 3; }
	const uint8_t* data() const noexcept { return std::bit_cast<const uint8_t*>(Data); }
	int	remaining_bytes()const noexcept { return DataBytes; }

	// Has the buffer overflowed?
	bool check_for_overflow(int nBits)
	{
		if (CurBit + nBits > DataBits)
			mark_as_overflowed();
		return IsOverflow;
	}

	bool has_overflown() const noexcept { return IsOverflow; }
	void mark_as_overflowed() noexcept { IsOverflow = true; }

private:
	const uint8_t*	Data;
	int				DataBytes;
	int				DataBits;

	int				CurBit;
	bool			IsOverflow;
	bool			AssertOnOverflow;
	const char*		DebugName;
};

TF2_NAMESPACE_END();

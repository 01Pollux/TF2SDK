#pragma once

#include "UtlMemory.hpp"
#include "Byteswap.hpp"
#include <ctype.h>

TF2_NAMESPACE_BEGIN(::utils);

//-----------------------------------------------------------------------------
// Description of character conversions for string output
// Here's an example of how to use the macros to define a character conversion
// BEGIN_CHAR_CONVERSION( CStringConversion, '\\' )
//	{ '\n', "n" },
//	{ '\t', "t" }
// END_CHAR_CONVERSION( CStringConversion, '\\' )
//-----------------------------------------------------------------------------
class UtlCharConversion
{
public:
	struct ConversionArray
	{
		char m_ActualChar;
		const char* m_ReplacementString;
	};

	UtlCharConversion(char nEscapeChar, const char* pDelimiter, uint32_t nCount, ConversionArray* pArray) :
		m_EscapeChar(nEscapeChar),
		m_Delimiter(pDelimiter),
		m_Count(nCount),
		m_DelimiterLength(strlen(pDelimiter))
	{
		for (uint32_t i = 0; i < nCount; ++i)
		{
			m_KeyList[i] = pArray[i].m_ActualChar;
			ConversionInfo& info = m_Replacements[static_cast<uint8_t>(m_KeyList[i])];
			info.m_ReplacementString = pArray[i].m_ReplacementString;
			info.m_Length = strlen(info.m_ReplacementString);
			if (info.m_Length > m_MaxConversionLength)
			{
				m_MaxConversionLength = info.m_Length;
			}
		}
	}

	char GetEscapeChar() const
	{
		return m_EscapeChar;
	}

	const char* GetDelimiter() const
	{
		return m_Delimiter;
	}

	uint32_t GetDelimiterLength() const
	{
		return m_DelimiterLength;
	}

	const char* GetConversionString(char c) const
	{
		return m_Replacements[static_cast<uint8_t>(c)].m_ReplacementString;
	}

	int GetConversionLength(char c) const
	{
		return m_Replacements[static_cast<uint8_t>(c)].m_Length;
	}

	int MaxConversionLength() const
	{
		return m_MaxConversionLength;
	}

	// Finds a conversion for the passed-in string, returns length
	virtual char FindConversion(const char* pString, uint32_t* pLength)
	{
		for (uint32_t i = 0; i < m_Count; ++i)
		{
			if (!strcmp(pString, GetConversionString(m_KeyList[i])))
			{
				*pLength = GetConversionLength(m_KeyList[i]);
				return m_KeyList[i];
			}
		}

		*pLength = 0;
		return '\0';
	}

protected:
	struct ConversionInfo
	{
		uint32_t m_Length;
		const char* m_ReplacementString;
	};

	char		m_EscapeChar;
	const char* m_Delimiter;
	uint32_t		m_DelimiterLength;
	uint32_t		m_Count;
	uint32_t		m_MaxConversionLength{ };
	char		m_KeyList[256];
	ConversionInfo m_Replacements[256]{ };
};

#define BEGIN_CHAR_CONVERSION( _name, _delimiter, _escapeChar )	\
	static UtlCharConversion::ConversionArray_t s_pConversionArray##_name[] = {

#define END_CHAR_CONVERSION( _name, _delimiter, _escapeChar ) \
	}; \
	UtlCharConversion _name(_escapeChar, _delimiter, sizeof(s_pConversionArray## name) / sizeof(CUtlCharConversion::ConversionArray_t), s_pConversionArray##_name);

#define BEGIN_CUSTOM_CHAR_CONVERSION( _className, _name, _delimiter, _escapeChar ) \
	static UtlCharConversion::ConversionArray_t s_pConversionArray##_name[] = {

#define END_CUSTOM_CHAR_CONVERSION( _className, _name, _delimiter, _escapeChar ) \
	}; \
	_className _name( _escapeChar, _delimiter, sizeof(s_pConversionArray##_name) / sizeof(CUtlCharConversion::ConversionArray_t), s_pConversionArray##_name);



class UtlBuffer
{
public:
	enum class SeekType
	{
		Head,
		Current,
		Tail
	};

	// Describes how get + put work (as strings, or binary)
	static constexpr uint32_t BufferFlags_Text = 0x1;
	// This is used w/ external buffers and causes the utlbuf to switch to reallocatable memory if an overflow happens when Putting.
	static constexpr uint32_t BufferFlags_External_Growable = 0x2;
	// For text buffers only, does this contain \n or \n\r?
	static constexpr uint32_t BufferFlags_Has_CRLF = 0x4;
	// For external buffers; prevents null termination from happening.
	static constexpr uint32_t BufferFlags_ReadOnly = 0x8;
	// Used to disable/enable push/pop tabs
	static constexpr uint32_t BufferFlags_NoAutoTabs = 0x10;

	// Overflow functions when a get or put overflows
	using OverflowCallback = bool(UtlBuffer::*)(int size);

	// Constructors for growable + external buffers for serialization/unserialization
	UtlBuffer(int growSize = 0, int initSize = 0, uint32_t nFlags = 0) :
		m_Error(0)
	{
		m_Memory.init(growSize, initSize);
		m_Get = 0;
		m_Put = 0;
		m_nTab = 0;
		m_nOffset = 0;
		m_Flags = nFlags;
		if ((initSize != 0) && !is_read_only())
		{
			m_nMaxPut = -1;
			add_null_termination();
		}
		else
		{
			m_nMaxPut = 0;
		}
		set_overflow_func(&UtlBuffer::OnGetOverflow, &UtlBuffer::OnPutOverflow);
	}

	UtlBuffer(const void* pBuffer, int size, uint32_t nFlags = 0) :
		m_Memory(std::bit_cast<uint8_t*>(pBuffer), size), m_Error(0)
	{
		m_Get = 0;
		m_Put = 0;
		m_nTab = 0;
		m_nOffset = 0;
		m_Flags = nFlags;
		if (is_read_only())
		{
			m_nMaxPut = size;
		}
		else
		{
			m_nMaxPut = -1;
			add_null_termination();
		}
		set_overflow_func(&UtlBuffer::OnGetOverflow, &UtlBuffer::OnPutOverflow);
	}

	uint32_t		get_flags() const noexcept
	{
		return m_Flags;
	}

	// NOTE: This will assert if you attempt to recast it in a way that
	// is not compatible. The only valid conversion is binary-> text w/CRLF
	void			set_buffer_type(bool bIsText, bool bContainsCRLF) noexcept
	{
		if (bIsText)
			m_Flags |= BufferFlags_Text;
		else
			m_Flags &= ~BufferFlags_Text;
		
		if (bContainsCRLF)
			m_Flags |= BufferFlags_Has_CRLF;
		else
			m_Flags &= ~BufferFlags_Has_CRLF;
	}

	// Makes sure we've got at least this much memory
	void			reserve(uint32_t num)
	{
		num += 1;
		if (m_Memory.is_external())
		{
			if (is_growable() && (m_Memory.capacity() < num))
			{
				m_Memory.convert_to_growable(0);
			}
			else
			{
				num -= 1;
			}
		}

		m_Memory.reserve(num);
	}

	// Attaches the buffer to external memory....
	void			set_external_buffer(void* pMemory, int nSize, int nInitialPut, uint32_t nFlags = 0)
	{
		m_Memory.set_external_buffer(std::bit_cast<const uint8_t*>(pMemory), nSize);

		// Reset all indices; we just changed memory
		m_Get = 0;
		m_Put = nInitialPut;
		m_nTab = 0;
		m_Error = 0;
		m_nOffset = 0;
		m_Flags = nFlags;
		m_nMaxPut = -1;

		add_null_termination();
	}

	bool			is_external_buffer() const
	{
		return m_Memory.is_external();
	}

	// Takes ownership of the passed memory, including freeing it when this buffer is destroyed.
	void			take_ownership(void* pMemory, int nSize, int nInitialPut, uint32_t nFlags = 0)
	{
		m_Memory.take_ownership(std::bit_cast<uint8_t*>(pMemory), nSize);

		m_Get = 0;
		m_Put = nInitialPut;
		m_nTab = 0;
		m_Error = 0;
		m_nOffset = 0;
		m_Flags = nFlags;
		m_nMaxPut = -1;

		add_null_termination();
	}

	// copies data from another buffer
	void copy_buffer(const UtlBuffer& buffer)
	{
		copy_buffer(buffer.data(), buffer.tell_put());
	}

	void copy_buffer(const void* pubData, int cubData)
	{
		clear();
		if (cubData)
			put(pubData, cubData);
	}

	void swap(UtlBuffer& buf)
	{
		std::swap(m_Get, buf.m_Get);
		std::swap(m_Put, buf.m_Put);
		std::swap(m_nMaxPut, buf.m_nMaxPut);
		std::swap(m_Error, buf.m_Error);
		m_Memory.swap(buf.m_Memory);
	}

	void swap(UtlMemory<uint8_t>& buf)
	{
		m_Get = 0;
		m_Put = buf.capacity();
		m_nMaxPut = buf.capacity();
		m_Error = 0;
		m_Memory.swap(buf);
	}

	// Controls endian-ness of binary utlbufs - default matches the current platform
	void			activate_byteswapping(bool bActivate) noexcept
	{
		m_Byteswap.activate_byteswapping(bActivate);
	}

	void			set_big_endian(bool bigEndian) noexcept
	{
		m_Byteswap.set_target_endian(bigEndian);
	}

	bool			is_big_endian() const noexcept
	{
		m_Byteswap.is_big_endian();
	}

	// Resets the buffer; but doesn't free memory
	void			clear()
	{
		m_Get = 0;
		m_Put = 0;
		m_Error = 0;
		m_nOffset = 0;
		m_nMaxPut = -1;

		add_null_termination();
	}

	// Clears out the buffer; frees memory
	void			destroy()
	{
		m_Get = 0;
		m_Put = 0;
		m_nOffset = 0;
		m_nMaxPut = 0;
		m_Error = 0;

		m_Memory.destroy();
	}

	// Read stuff out.
	// Binary mode: it'll just read the bits directly in, and characters will be
	//		read for strings until a null character is reached.
	// Text mode: it'll parse the file, turning text #s into real numbers.
	//		GetString will read a string until a space is reached
#define BUFFER_GET_TYPE(name, rettype, fmt)	\
	rettype get_##name()					\
	{										\
		rettype val; get_type(val, fmt);	\
		return val;							\
	}
	
	BUFFER_GET_TYPE(char,	int8_t, "%c");
	BUFFER_GET_TYPE(uchar,	uint8_t, "%u");
	BUFFER_GET_TYPE(short,	int16_t, "%i");
	BUFFER_GET_TYPE(ushort,	uint16_t, "%u");
	BUFFER_GET_TYPE(int32,	int32_t, "%i");
	BUFFER_GET_TYPE(uint32,	uint32_t, "%u");
	BUFFER_GET_TYPE(int64,	int64_t, "%ll");
	BUFFER_GET_TYPE(uint64,	uint64_t, "%llu");
	BUFFER_GET_TYPE(inthex,	uint32_t, "%x");
	BUFFER_GET_TYPE(float,	float, "%f");
	BUFFER_GET_TYPE(double, double, "%lf");

#undef BUFFER_GET_TYPE

	template<uint32_t _Size>
	void			get_string(char pString[_Size])
	{
		get_string(pString, _Size);
	}

	void			get_string(char* str, uint32_t size)
	{
		if (!is_valid())
		{
			*str = 0;
			return;
		}
		else if (!size)
			return;

		// Remember, this *includes* the null character
		// It will be 0, however, if the buffer is empty.
		uint32_t nLen = peek_string_length();

		if (is_text())
			eat_white_space();

		if (nLen <= 0)
		{
			*str = 0;
			m_Error |= ErrorFlags_Get;
			return;
		}

		const uint32_t to_read = std::min(nLen, size) - 1;

		get(str, to_read);
		str[to_read] = 0;

		if ((uint32_t)nLen > (to_read + 1))
			seek_get(SeekType::Current, nLen - (to_read + 1));
	}

	void			get(void* pMem, int size)
	{
		if (size > 0 && check_get(size))
		{
			int Index = m_Get - m_nOffset;

			memcpy(pMem, &m_Memory[Index], size);
			m_Get += size;
		}
	}

	void			get_line(char* pLine, int max_chars = INT_MAX)
	{
		if (!is_valid())
		{
			*pLine = 0;
			return;
		}

		// Remember, this *includes* the null character
		// It will be 0, however, if the buffer is empty.
		int nLen = peek_line_length();
		if (nLen == 0)
		{
			*pLine = 0;
			m_Error |= ErrorFlags_Get;
			return;
		}

		// Strip off the terminating NULL
		if (nLen <= max_chars)
		{
			get(pLine, nLen - 1);
			pLine[nLen - 1] = 0;
		}
		else
		{
			max_chars -= 1;
			get(pLine, max_chars);
			pLine[max_chars] = 0;
			seek_get(SeekType::Current, nLen - max_chars);
		}
	}

	// This will get at least 1 byte and up to nSize bytes. 
	// It will return the number of bytes actually read.
	uint32_t			get_upto(void* pMem, uint32_t size)
	{
		if (check_arbitrary_peek_get(0, size))
		{
			int Index = m_Get - m_nOffset;

			memcpy(pMem, &m_Memory[Index], size);
			m_Get += size;
			return size;
		}
		return 0;
	}

	// This version of GetString converts \" to \\ and " to \, etc.
	// It also reads a " at the beginning and end of the string
	void			get_delimited_string(UtlCharConversion* pConv, char* pString, uint32_t max_chars = 0)
	{
		if (!is_text() || !pConv)
		{
			get_string(pString, max_chars);
			return;
		}

		if (!is_valid())
		{
			*pString = 0;
			return;
		}

		if (!max_chars)
			max_chars = std::numeric_limits<uint32_t>::max();

		eat_white_space();
		if (!peek_string_match(0, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
			return;

		// Pull off the starting delimiter
		seek_get(SeekType::Current, pConv->GetDelimiterLength());

		uint32_t read = 0;
		while (is_valid())
		{
			if (peek_string_match(0, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
			{
				seek_get(SeekType::Current, pConv->GetDelimiterLength());
				break;
			}

			char c = get_delimited_char_internal(pConv);

			if (read < max_chars)
			{
				pString[read] = c;
				++read;
			}
		}

		if (read >= max_chars)
			read = max_chars - 1;

		pString[read] = '\0';
	}

	char			get_delimited_char(UtlCharConversion* pConv)
	{
		if (!is_text() || !pConv)
			return get_char();
		return get_delimited_char_internal(pConv);
	}

	// This will return the # of characters of the string about to be read out
	// NOTE: The count will *include* the terminating 0!!
	// In binary mode, it's the number of characters until the next 0
	// In text mode, it's the number of characters until the next space.
	uint32_t			peek_string_length()
	{
		if (!is_valid())
			return 0;

		// Eat preceeding whitespace
		uint32_t nOffset = is_text() ? peek_white_space(0) : 0;

		int nStartingOffset = nOffset;

		do
		{
			uint32_t peek_size = 128;

			// NOTE: Add 1 for the terminating zero!
			if (!check_arbitrary_peek_get(nOffset, peek_size))
			{
				if (nOffset == nStartingOffset)
					return 0;
				return nOffset - nStartingOffset + 1;
			}

			const char* pTest = std::bit_cast<const char*>(peek_get(nOffset));

			if (!is_text())
			{
				for (uint32_t i = 0; i < peek_size; ++i)
				{
					// The +1 here is so we eat the terminating 0
					if (pTest[i] == 0)
						return (i + nOffset - nStartingOffset + 1);
				}
			}
			else
			{
				for (uint32_t i = 0; i < peek_size; ++i)
				{
					// The +1 here is so we eat the terminating 0
					if (!pTest[i] || isspace(pTest[i]))
						return (i + nOffset - nStartingOffset + 1);
				}
			}

			nOffset += peek_size;

		} while (true);

		return 0;
	}

	// This version of PeekStringLength converts \" to \\ and " to \, etc.
	// It also reads a " at the beginning and end of the string
	// NOTE: The count will *include* the terminating 0!!
	// In binary mode, it's the number of characters until the next 0
	// In text mode, it's the number of characters between "s (checking for \")
	// Specifying false for bActualSize will return the pre-translated number of characters
	// including the delimiters and the escape characters. So, \n counts as 2 characters when bActualSize == false
	// and only 1 character when bActualSize == true
	uint32_t			peek_delimited_string_length(UtlCharConversion* pConv, bool bActualSize = true)
	{
		if (!is_text() || !pConv)
			return peek_string_length();

		// Eat preceeding whitespace
		uint32_t offset = peek_white_space(0);

		if (!peek_string_match(offset, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
			return 0;

		// Try to read ending ", but don't accept \"
		const uint32_t start_offset = offset;
		offset += pConv->GetDelimiterLength();
		uint32_t terminate_count = 1;	// Starts at 1 for the '\0' termination

		do
		{
			if (peek_string_match(offset, pConv->GetDelimiter(), pConv->GetDelimiterLength()))
				break;

			if (!check_peek_get(offset, 1))
				break;

			char c = *std::bit_cast<const char*>(peek_get(offset));
			++terminate_count;
			++offset;
			if (c == pConv->GetEscapeChar())
			{
				uint32_t length = pConv->MaxConversionLength();
				if (!check_arbitrary_peek_get(offset, length))
					break;

				pConv->FindConversion(std::bit_cast<const char*>(peek_get(offset)), &length);
				offset += length;
			}
		} while (true);

		return bActualSize ? terminate_count : offset - start_offset + pConv->GetDelimiterLength() + 1;
	}

	// Just like scanf, but doesn't work in binary mode
	uint32_t			scanf(const char* pFmt, ...)
	{
		va_list args;

		__crt_va_start(args, pFmt);
		uint32_t count = vascanf(pFmt, args);
		__crt_va_end(args);

		return count;
	}

	uint32_t			vascanf(const char* pFmt, va_list list);

	// Just like scanf, but doesn't work in binary mode
	uint32_t			printf(const char* pFmt, ...)
	{
		va_list args;

		__crt_va_start(args, pFmt);
		uint32_t count = vaprintf(pFmt, args);
		__crt_va_end(args);

		return count;
	}

	uint32_t			vaprintf(const char* pFmt, va_list list)
	{
		char temp[2048];
		int count = vsnprintf(temp, sizeof(temp), pFmt, list);
		put_string(temp);
		return count == -1 ? 0 : static_cast<uint32_t>(count);
	}

	// Eats white space, advances Get index
	void			eat_white_space()
	{
		if (is_text() && is_valid())
		{
			while (check_get(sizeof(char)))
			{
				if (!isspace(*std::bit_cast<const char*>(peek_get())))
					break;
				m_Get += sizeof(char);
			}
		}
	}

	// Eats C++ style comments
	bool			eat_cpp_comments()
	{
		if (is_text() && is_valid())
		{
			// If we don't have a a c++ style comment next, we're done
			const char* pPeek = std::bit_cast<const char*>(peek_get(2 * sizeof(char), 0));
			if (!pPeek || (pPeek[0] != '/') || (pPeek[1] != '/'))
				return false;

			// Deal with c++ style comments
			m_Get += 2;

			// read complete line
			for (char c = get_char(); is_valid(); c = get_char())
			{
				if (c == '\n')
					break;
			}
			return true;
		}
		return false;
	}

	// (For text buffers only)
	// Parse a token from the buffer:
	// Grab all text that lies between a starting delimiter + ending delimiter
	// (skipping whitespace that leads + trails both delimiters).
	// If successful, the get index is advanced and the function returns true,
	// otherwise the index is not advanced and the function returns false.
	bool			parse_token(const char* pStartingDelim, const char* pEndingDelim, char* pString, size_t nMaxLen);

	// Advance the get index until after the particular string is found
	// Do not eat whitespace before starting. Return false if it failed
	// String test is case-insensitive.
	bool			get_token(const char* pToken);

	// Write stuff in
	// Binary mode: it'll just write the bits directly in, and strings will be
	//		written with a null terminating character
	// Text mode: it'll convert the numbers to text versions
	//		PutString will not write a terminating character
#define BUFFER_PUT_TYPE(name, type, fmt)	\
	void put_##name(type val)				\
	{										\
		put_type(val, fmt);					\
	}

	void			put_char(int8_t c)
	{
		if (was_last_char_cr())
			put_tabs();

		put_type_bin(c);
	}
	BUFFER_PUT_TYPE(uchar,	uint8_t,	"%u");
	BUFFER_PUT_TYPE(short,	uint16_t,	"%i");
	BUFFER_PUT_TYPE(ushort, uint16_t,	"%u");
	BUFFER_PUT_TYPE(int32,	int32_t,	"%i");
	BUFFER_PUT_TYPE(uint32, uint32_t,	"%u");
	BUFFER_PUT_TYPE(int64,	int64_t,	"%ll");
	BUFFER_PUT_TYPE(uint64, uint64_t,	"%llu");
	BUFFER_PUT_TYPE(float,	float,		"%f");
	BUFFER_PUT_TYPE(double, double,		"%lf");

#undef BUFFER_PUT_TYPE

	void			put_string(const char* pString);
	void			put(const void* pMem, uint32_t size)
	{
		if (size && check_put(size))
		{
			int idx = m_Put - m_nOffset;
			if (idx >= 0)
			{
				memcpy(&m_Memory[idx], pMem, size);
				m_Put += size;

				add_null_termination();
			}
		}
	}

	// This version of PutString converts \ to \\ and " to \", etc.
	// It also places " at the beginning and end of the string
	void			put_delimited_string(UtlCharConversion* pConv, const char* pString);
	void			put_delimited_char(UtlCharConversion* pConv, char c);

	// What am I writing (put)/reading (get)?
	void*			peek_put(uint32_t offset = 0)
	{
		return &m_Memory[m_Put + offset - m_nOffset];
	}

	const void*		peek_get(uint32_t offset = 0) const
	{
		return &m_Memory[m_Get + offset - m_nOffset];
	}

	const void*		peek_get(uint32_t size, uint32_t offset)
	{
		if (!check_peek_get(offset, size))
			return NULL;

		return peek_get(offset);
	}

	// Where am I writing (put)/reading (get)?
	uint32_t			tell_put() const
	{
		return m_Put;
	}
	uint32_t			tell_get() const
	{
		return m_Get;
	}

	// What's the most I've ever written?
	uint32_t			tell_put_max() const
	{
		return m_nMaxPut;
	}

	// How many bytes remain to be read?
	// NOTE: This is not accurate for streaming text files; it overshoots
	uint32_t			get_remaining_bytes() const
	{
		return m_nMaxPut - tell_get();
	}

	// Change where I'm writing (put)/reading (get)
	void			seek_put(SeekType type, uint32_t offset)
	{
		uint32_t next_put;
		switch (type)
		{
		case SeekType::Head:
			next_put = offset;
			break;

		case SeekType::Current:
			next_put = tell_put() + offset;
			break;

		case SeekType::Tail:
			next_put = tell_put_max() - offset;
			break;
		}

		// Force a write of the data
		// FIXME: We could make this more optimal potentially by writing out
		// the entire buffer if you seek outside the current range

		// NOTE: This call will write and will also seek the file to nNextPut.
		OnPutOverflow(-static_cast<int>(next_put + 1));
		m_Put = next_put;

		add_null_termination();
	}

	void			seek_get(SeekType type, int offset)
	{
		switch (type)
		{
		case SeekType::Head:
			m_Get = offset;
			break;

		case SeekType::Current:
			m_Get = tell_get() + offset;
			break;

		case SeekType::Tail:
			m_Get = tell_put_max() - offset;
			break;
		}

		if (m_Get > tell_put_max())
		{
			m_Error |= ErrorFlags_Get;
		}
		else
		{
			m_Error &= ~ErrorFlags_Get;
			if (m_Get < m_nOffset || m_Get >= m_nOffset + size())
				OnGetOverflow(static_cast<uint32_t>(-1));
		}
	}

	// Buffer base
	const void*		data() const
	{
		m_Memory.data();
	}

	void*			data()
	{
		m_Memory.data();
	}

	// Returns the base as a const char*, only valid in text mode.
	const char*		string() const
	{
		return std::bit_cast<const char*>(data());
	}

	// memory allocation size, does *not* reflect size written or read,
	//	use TellPut or TellGet for that
	int				size() const
	{
		return m_Memory.capacity();
	}

	// Am I a text buffer?
	bool			is_text() const
	{
		return (m_Flags & BufferFlags_Text) != 0;
	}

	// Can I grow if I'm externally allocated?
	bool			is_growable() const
	{
		return (m_Flags & BufferFlags_External_Growable) != 0;
	}

	// Am I valid? (overflow or underflow error), Once invalid it stays invalid
	bool			is_valid() const
	{
		return !m_Error;
	}

	// Do I contain carriage return/linefeeds? 
	bool			has_crlf() const
	{
		return is_text() && ((m_Flags & BufferFlags_Has_CRLF) != 0);
	}

	// Am I read-only
	bool			is_read_only() const
	{
		return (m_Flags & BufferFlags_ReadOnly) != 0;
	}

	// Converts a buffer from a CRLF buffer to a CR buffer (and back)
	// Returns false if no conversion was necessary (and outBuf is left untouched)
	// If the conversion occurs, outBuf will be cleared.
	bool			convert_to_cr(UtlBuffer& outBuf);

	// Push/pop pretty-printing tabs
	void			push_tab()
	{
		++m_nTab;
	}
	void			pop_tab()
	{
		if (--m_nTab < 0)
			m_nTab = 0;
	}

	// Temporarily disables pretty print
	void			set_tabs(bool bEnable)
	{
		if (bEnable)
			m_Flags &= ~BufferFlags_NoAutoTabs;
		else
			m_Flags |= BufferFlags_NoAutoTabs;
	}

protected:
	// error flags
	static constexpr uint32_t ErrorFlags_Put = 0x1;
	static constexpr uint32_t ErrorFlags_Get = 0x1;
	static constexpr uint32_t ErrorFlags_Max = ErrorFlags_Get;

	void set_overflow_func(OverflowCallback getFunc, OverflowCallback putFunc)
	{
		m_GetOverflowFunc = getFunc;
		m_PutOverflowFunc = putFunc;
	}

	bool OnPutOverflow(int nSize)
	{
		return false;
	}

	bool OnGetOverflow(int nSize)
	{
		return false;
	}

protected:
	// Checks if a get/put is ok
	bool check_put(uint32_t size)
	{
		if ((m_Error & ErrorFlags_Put) || is_read_only())
			return false;

		if ((m_Put < m_nOffset) || (m_Memory.capacity() < m_Put - m_nOffset + size))
		{
			if (!OnPutOverflow(size))
			{
				m_Error |= ErrorFlags_Put;
				return false;
			}
		}
		return true;
	}

	bool check_get(uint32_t size)
	{
		if (m_Error & ErrorFlags_Get)
			return false;

		if (tell_put_max() < m_Get + size)
		{
			m_Error |= ErrorFlags_Get;
			return false;
		}

		if ((m_Get < m_nOffset) || (m_Memory.capacity() < m_Get - m_nOffset + size))
		{
			if (!OnGetOverflow(size))
			{
				m_Error |= ErrorFlags_Get;
				return false;
			}
		}

		return true;
	}

	void add_null_termination()
	{
		if (m_Put > m_nMaxPut)
		{
			if (!is_read_only() && !(m_Error & ErrorFlags_Put))
			{
				// Add null termination value
				if (check_put(1))
				{
					int idx = m_Put - m_nOffset;
					if (idx >= 0)
						m_Memory[idx] = 0;
				}
				else
				{
					// Restore the overflow state, it was valid before...
					m_Error &= ~ErrorFlags_Put;
				}
			}
			m_nMaxPut = m_Put;
		}
	}

	// Methods to help with pretty-printing
	bool was_last_char_cr()
	{
		if (!is_text() || !tell_put())
			return false;
		return *std::bit_cast<const char*>(peek_put(-1)) == '\n';
	}

	void put_tabs()
	{
		for (int i = (m_Flags & BufferFlags_NoAutoTabs) ? 0 : m_nTab; --i >= 0; )
			put_type_bin('\t');
	}

	// Help with delimited stuff
	char get_delimited_char_internal(UtlCharConversion* pConv)
	{
		char c = get_char();
		if (c == pConv->GetEscapeChar())
		{
			uint32_t length = pConv->MaxConversionLength();
			if (!check_arbitrary_peek_get(0, length))
				return '\0';

			c = pConv->FindConversion(std::bit_cast<const char*>(peek_get()), &length);
			seek_get(SeekType::Current, length);
		}

		return c;
	}

	void put_delimited_char_internal(UtlCharConversion* pConv, char c)
	{
		if (!pConv->GetConversionLength(c))
		{
			put_char(c);
		}
		else
		{
			put_char(pConv->GetEscapeChar());
			put(pConv->GetConversionString(c), pConv->GetConversionLength(c));
		}
	}

	// Does the next bytes of the buffer match a pattern?
	bool peek_string_match(uint32_t offset, const char* pString, uint32_t size)
	{
		if (!check_peek_get(offset, size))
			return false;
		return !strncmp(std::bit_cast<const char*>(peek_get(offset)), pString, size);
	}

	// Peek size of line to come, check memory bound
	uint32_t	peek_line_length()
	{
		if (!is_valid())
			return 0;

		uint32_t offset = 0;
		uint32_t start_offset = offset;

		do
		{
			uint32_t peek_size = 128;

			// NOTE: Add 1 for the terminating zero!
			if (!check_arbitrary_peek_get(offset, peek_size))
			{
				return offset == start_offset ? 0 : offset - start_offset + 1;
			}

			const char* pTest = std::bit_cast<const char*>(peek_get(offset));

			for (uint32_t i = 0; i < peek_size; ++i)
			{
				// The +2 here is so we eat the terminating '\n' and 0
				if (pTest[i] == '\n' || pTest[i] == '\r')
					return (i + offset - start_offset + 2);
				// The +1 here is so we eat the terminating 0
				if (pTest[i] == 0)
					return (i + offset - start_offset + 1);
			}

			offset += peek_size;

		} while (true);

		return 0;
	}

	// How much whitespace should I skip?
	uint32_t peek_white_space(uint32_t offset)
	{
		if (!is_text() || !is_valid())
			return 0;

		while (check_peek_get(offset, sizeof(char)))
		{
			if (!isspace(*std::bit_cast<const char*>(peek_get(offset))))
				break;
			offset += sizeof(char);
		}

		return offset;
	}

	// Checks if a peek get is ok
	bool check_peek_get(uint32_t offset, uint32_t size)
	{
		if (m_Error & ErrorFlags_Get)
			return false;

		// Checking for peek can't set the overflow flag
		bool bOk = check_get(offset + size);
		m_Error &= ~ErrorFlags_Get;
		return bOk;
	}

	// Call this to peek arbitrarily long into memory. It doesn't fail unless
	// it can't read *anything* new
	bool check_arbitrary_peek_get(uint32_t offset, uint32_t& incr)
	{
		if (tell_get() + offset >= tell_put_max())
		{
			incr = 0;
			return false;
		}

		if (tell_get() + offset + incr > tell_put_max())
		{
			incr = tell_put_max() - tell_get() - offset;
		}

		// NOTE: CheckPeekGet could modify TellMaxPut for streaming files
		// We have to call TellMaxPut again here
		check_peek_get(offset, incr);
		uint32_t nMaxGet = tell_put_max() - tell_get();
		if (nMaxGet < incr)
			incr = nMaxGet;
		
		return incr != 0;
	}


	template<typename _Ty>
	void get_type(_Ty& dest, const char* pszFmt)
	{
		if (!is_text())
		{
			get_type_bin(dest);
		}
		else
		{
			dest = 0;
			this->scanf(pszFmt, &dest);
		}
	}

	template<typename _Ty>
	void get_type_bin(_Ty& dest)
	{
		if (check_get(sizeof(float)))
		{
			if constexpr (std::is_same_v<_Ty, float>)
			{
				dest = *std::bit_cast<float*>(peek_get());

				if (m_Byteswap.is_swapping_bytes())
					m_Byteswap.swap_buffer_to_target_endian(&dest, &dest);

				m_Get += sizeof(float);
			}
			else
			{
				if (!m_Byteswap.is_swapping_bytes() || (sizeof(_Ty) == 1))
					dest = *std::bit_cast<_Ty*>(peek_get());
				else
					m_Byteswap.swap_buffer_to_target_endian(&dest, std::bit_cast<_Ty*>(peek_get()));

				m_Get += sizeof(_Ty);
			}
		}
		else
		{
			dest = 0;
		}
	}

	template<typename _Ty>
	void put_type(const _Ty& src, const char* pszFmt)
	{
		if (!is_text())
			put_type_bin(src);
		else
			this->printf(pszFmt, src);
	}

	template<typename _Ty>
	void put_type_bin(const _Ty& src)
	{
		if (check_put(sizeof(_Ty)))
		{
			if (!m_Byteswap.is_swapping_bytes() || (sizeof(_Ty) == 1))
				*std::bit_cast<_Ty*>(peek_put()) = src;
			else
				m_Byteswap.swap_buffer_to_target_endian(std::bit_cast<_Ty*>(peek_put()), &src);
			
			m_Put += sizeof(_Ty);
			add_null_termination();
		}
	}


private:
	UtlMemory<unsigned char> m_Memory;
	uint32_t m_Get;
	uint32_t m_Put;

	uint8_t m_Error;
	uint8_t m_Flags;
	uint8_t m_Reserved;

	uint32_t m_nTab;
	uint32_t m_nMaxPut;
	uint32_t m_nOffset;

	OverflowCallback m_GetOverflowFunc;
	OverflowCallback m_PutOverflowFunc;

	ByteSwap	m_Byteswap;
};


bool UtlBuffer::parse_token(const char* pStartingDelim, const char* pEndingDelim, char* pString, size_t nMaxLen)
{
	uint32_t chars_to_copy = 0;
	uint32_t cur_get = 0;

	// Starting delimiter is optional
	char emptyBuf = '\0';
	if (!pStartingDelim)
		pStartingDelim = &emptyBuf;

	// Ending delimiter is not
	uint32_t end_delim_len = strlen(pEndingDelim);

	int start_get = tell_get();
	char cur_char;
	int token_start = -1;
	eat_white_space();
	while (*pStartingDelim)
	{
		cur_char = *pStartingDelim++;
		if (!isspace(cur_char))
		{
			if (tolower(get_char()) != tolower(cur_char))
				goto parseFailed;
		}
		else
			eat_white_space();
	}

	eat_white_space();
	token_start = tell_get();
	if (!get_token(pEndingDelim))
		goto parseFailed;

	cur_get = tell_get();
	chars_to_copy = (cur_get - end_delim_len) - token_start;
	if (chars_to_copy >= nMaxLen)
	{
		chars_to_copy = nMaxLen - 1;
	}

	if (chars_to_copy > 0)
	{
		seek_get(SeekType::Head, token_start);
		get(pString, chars_to_copy);
		if (!is_valid())
			goto parseFailed;

		// Eat trailing whitespace
		for (; chars_to_copy > 0; --chars_to_copy)
		{
			if (!isspace(pString[chars_to_copy - 1]))
				break;
		}
	}
	pString[chars_to_copy] = '\0';

	// Advance the Get index
	seek_get(SeekType::Head, cur_get);
	return true;

parseFailed:
	// Revert the get index
	seek_get(SeekType::Head, start_get);
	pString[0] = '\0';
	return false;
}

bool UtlBuffer::get_token(const char* pToken)
{
	// Look for the token
	uint32_t token_size = strlen(pToken);
	uint32_t check_size = size() - tell_get() - m_nOffset;
	
	uint32_t nGet = tell_get();

	auto strnistr = [] (auto pStr, char const* pSearch, uint32_t n) 
		-> decltype(pStr)
	{
		if (!pStr || !pSearch)
			return nullptr;

		auto pLetter = pStr;

		// Check the entire string
		while (*pLetter != 0)
		{
			if (n <= 0)
				return nullptr;

			// Skip over non-matches
			if (tolower(*pLetter) == tolower(*pSearch))
			{
				int n1 = n - 1;

				// Check for match
				char const* pMatch = pLetter + 1;
				char const* pTest = pSearch + 1;
				while (*pTest != 0)
				{
					if (n1 <= 0)
						return 0;

					// We've run off the end; don't bother.
					if (*pMatch == 0)
						return 0;

					if (tolower(*pMatch) != tolower(*pTest))
						break;

					++pMatch;
					++pTest;
					--n1;
				}

				// Found a match!
				if (*pTest == 0)
					return pLetter;
			}

			++pLetter;
			--n;
		}

		return nullptr;
	};

	do
	{
		uint32_t max_size = tell_put_max() - tell_get();
		if (max_size < check_size)
		{
			check_size = max_size;
		}
		if (token_size > check_size)
			break;

		if (!check_peek_get(0, check_size))
			break;

		const char* pBufStart = (const char*)peek_get();
		const char* pFoundEnd = strnistr(pBufStart, pToken, check_size);
		if (pFoundEnd)
		{
			uint32_t nOffset = std::bit_cast<uint32_t>(pFoundEnd) - std::bit_cast<uint32_t>(pBufStart);
			seek_get(SeekType::Current, nOffset + token_size);
			return true;
		}

		seek_get(SeekType::Current, check_size - token_size - 1);
		check_size = size() - (token_size - 1);

	} while (true);

	seek_get(SeekType::Head, nGet);
	return false;
}


uint32_t UtlBuffer::vascanf(const char* pFmt, va_list list)
{
	if (m_Error || !is_text())
		return 0;

	uint32_t count = 0;
	char c;
	char* pEnd;

	while ((c = *pFmt++))
	{
		// Stop if we hit the end of the buffer
		if (m_Get >= tell_put_max())
		{
			m_Error |= ErrorFlags_Get;
			break;
		}

		switch (c)
		{
		case ' ':
		{
			// eat all whitespace
			eat_white_space();
			break;
		}

		case '%':
		{
			// Conversion character... try to convert baby!
			char type = *pFmt++;
			if (type == 0)
				return count;

			switch (type)
			{
			case 'c':
			{
				char* ch = __crt_va_arg(list, char*);
				if (check_peek_get(0, sizeof(char)))
				{
					*ch = *std::bit_cast<const char*>(peek_get());
					++m_Get;
				}
				else
				{
					*ch = 0;
					return count;
				}
				break;
			}

			case 'i':
			case 'd':
			{
				int* i = __crt_va_arg(list, int*);

				// NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
				uint32_t size = 128;
				if (!check_arbitrary_peek_get(0, size))
				{
					*i = 0;
					return count;
				}

				*i = strtol(std::bit_cast<const char*>(peek_get()), &pEnd, 10);
				uint32_t bytes_read = std::bit_cast<uint32_t>(pEnd - std::bit_cast<const char*>(peek_get()));
				if (!bytes_read)
					return count;
				m_Get += bytes_read;
				break;
			}

			case 'x':
			{
				int* i = __crt_va_arg(list, int*);

				// NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
				uint32_t size = 128;
				if (!check_arbitrary_peek_get(0, size))
				{
					*i = 0;
					return count;
				}

				*i = strtol(std::bit_cast<const char*>(peek_get()), &pEnd, 16);
				int bytes_read = (int)(pEnd - std::bit_cast<const char*>(peek_get()));
				if (!bytes_read)
					return count;
				m_Get += bytes_read;
				break;
			}

			case 'u':
			{
				unsigned int* u = __crt_va_arg(list, unsigned int*);

				// NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
				uint32_t size = 128;
				if (!check_arbitrary_peek_get(0, size))
				{
					*u = 0;
					return count;
				}

				*u = strtoul(std::bit_cast<const char*>(peek_get()), &pEnd, 10);
				int bytes_read = (int)(pEnd - std::bit_cast<const char*>(peek_get()));
				if (!bytes_read)
					return count;
				m_Get += bytes_read;
				break;
			}

			case 'f':
			{
				float* u = __crt_va_arg(list, float*);

				// NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
				uint32_t size = 128;
				if (!check_arbitrary_peek_get(0, size))
				{
					*u = 0;
					return count;
				}

				*u = static_cast<float>(strtod(std::bit_cast<const char*>(peek_get()), &pEnd));
				int bytes_read = (int)(pEnd - std::bit_cast<const char*>(peek_get()));
				if (!bytes_read)
					return count;
				m_Get += bytes_read;
				break;
			}

			case 'l':
			{
				if (*pFmt != 'f')
					continue;

				pFmt++;
				double* u = __crt_va_arg(list, double*);

				// NOTE: This is not bullet-proof; it assumes numbers are < 128 characters
				uint32_t size = 128;
				if (!check_arbitrary_peek_get(0, size))
				{
					*u = 0;
					return count;
				}

				*u = strtod(std::bit_cast<const char*>(peek_get()), &pEnd);
				int bytes_read = (int)(pEnd - std::bit_cast<const char*>(peek_get()));
				if (!bytes_read)
					return count;
				m_Get += bytes_read;
				break;
			}

			case 's':
			{
				char* s = __crt_va_arg(list, char*);
				get_string(s, 256);
				break;
			}

			default:
			{
				// unimplemented scanf type
				return count;
			}
			}

			++count;
			break;
		}

		default:
		{
			// Here we have to match the format string character
			// against what's in the buffer or we're done.
			if (!check_peek_get(0, sizeof(char)))
				return count;

			if (c != *std::bit_cast<const char*>(peek_get()))
				return count;

			++m_Get;
			break;
		}
		}
	}
	return count;
}

void UtlBuffer::put_string(const char* pString)
{
	if (!is_text())
	{
		if (pString)
		{
			// Not text? append a null at the end.
			put(pString, (strlen(pString) + 1) * sizeof(char));
			return;
		}
		else
		{
			put_type_bin('\0');
		}
	}
	else if (pString)
	{
		int tab_count = (m_Flags & BufferFlags_NoAutoTabs) ? 0 : m_nTab;
		if (tab_count > 0)
		{
			if (was_last_char_cr())
				put_tabs();

			const char* pEndl = strchr(pString, '\n');
			while (pEndl)
			{
				uint32_t str_diff = static_cast<uint32_t>(pEndl - pString) + sizeof(char);
				put(pString, str_diff);
				pString = pEndl + 1;
				if (*pString)
				{
					put_tabs();
					pEndl = strchr(pString, '\n');
				}
				else
				{
					pEndl = nullptr;
				}
			}
		}
		uint32_t str_size = strlen(pString);
		if (str_size)
			put(pString, str_size * sizeof(char));
	}
}


bool UtlBuffer::convert_to_cr(UtlBuffer& outBuf)
{
	auto strnistr = [] (auto pStr, char const* pSearch, uint32_t n)
		-> decltype(pStr)
	{
		if (!pStr || !pSearch)
			return nullptr;

		auto pLetter = pStr;

		// Check the entire string
		while (*pLetter != 0)
		{
			if (n <= 0)
				return nullptr;

			// Skip over non-matches
			if (tolower(*pLetter) == tolower(*pSearch))
			{
				int n1 = n - 1;

				// Check for match
				char const* pMatch = pLetter + 1;
				char const* pTest = pSearch + 1;
				while (*pTest != 0)
				{
					if (n1 <= 0)
						return 0;

					// We've run off the end; don't bother.
					if (*pMatch == 0)
						return 0;

					if (tolower(*pMatch) != tolower(*pTest))
						break;

					++pMatch;
					++pTest;
					--n1;
				}

				// Found a match!
				if (*pTest == 0)
					return pLetter;
			}

			++pLetter;
			--n;
		}

		return nullptr;
	};

	if (!is_text() || !outBuf.is_text())
		return false;

	if (has_crlf() == outBuf.has_crlf())
		return false;

	uint32_t max_put = tell_put_max();

	outBuf.destroy();
	outBuf.reserve(max_put);

	bool has_crlf = this->has_crlf();

	// Start reading from the beginning
	uint32_t cur_get = tell_get();
	uint32_t cur_put = tell_put();
	int cur_get_delta = 0;
	int cur_put_delta = 0;

	const char* pBase = string();
	size_t out_get = 0;
	while (out_get < max_put)
	{
		const char* pCurr = &pBase[out_get];
		if (has_crlf)
		{
			const char* pNext = strnistr(pCurr, "\r\n", max_put - out_get);
			if (!pNext)
			{
				outBuf.put(pCurr, max_put - out_get);
				break;
			}

			size_t delta_bytes = static_cast<size_t>(pNext - pCurr);
			outBuf.put(pCurr, delta_bytes);
			outBuf.put_char('\n');
			out_get += delta_bytes + 2;
			if (cur_get >= out_get - 1)
			{
				--cur_get_delta;
			}
			if (cur_put >= out_get - 1)
			{
				--cur_put_delta;
			}
		}
		else
		{
			const std::string_view str(pCurr, pCurr + max_put - out_get);
			uint32_t new_line = str.find('\n');
			if (new_line == std::string::npos)
			{
				outBuf.put(pCurr, max_put - out_get);
				break;
			}

			outBuf.put(pCurr, new_line - 1);
			outBuf.put_char('\r');
			outBuf.put_char('\n');
			out_get += new_line;
			if (cur_get >= out_get)
			{
				++cur_get_delta;
			}
			if (cur_put >= out_get)
			{
				++cur_put_delta;
			}
		}
	}

	outBuf.seek_get(SeekType::Head, cur_get + cur_get_delta);
	outBuf.seek_put(SeekType::Head, cur_put + cur_put_delta);

	return true;
}


TF2_NAMESPACE_END();

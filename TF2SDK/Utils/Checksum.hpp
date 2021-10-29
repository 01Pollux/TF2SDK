#pragma once

#include "TF2Config.hpp"
#include <type_traits>

TF2_NAMESPACE_BEGIN(::Utils);

using CRC32_t = uint32_t;

SG_SDK_TF2 void CRC32_Init(CRC32_t*);
SG_SDK_TF2 void CRC32_ProcessBuffer(CRC32_t*, const void* p, int len);
SG_SDK_TF2 void CRC32_Final(CRC32_t*);

inline CRC32_t CRC32_ProcessSingleBuffer(const void* p, int len)
{
	CRC32_t crc;

	CRC32_Init(&crc);
	CRC32_ProcessBuffer(&crc, p, len);
	CRC32_Final(&crc);

	return crc;
}

#define MD5_DIGEST_LENGTH 16  
#define MD5_BIT_LENGTH MD5_DIGEST_LENGTH * sizeof(uint8_t)

struct MD5Value
{
	uint8_t bits[MD5_BIT_LENGTH]{ };

	bool is_zero() const noexcept
	{
		for (const uint8_t bit : bits)
			if (bit)
				return false;
		return true;
	}

	bool operator==(const MD5Value& src) const = default;
	bool operator!=(const MD5Value& src) const = default;
};


struct MD5Context
{
	uint32_t	Buf[4]{ };
	uint32_t	Bits[2]{ };
	uint8_t		In[64]{ };
};

SG_SDK_TF2 void MD5Init(MD5Context* context);
SG_SDK_TF2 void MD5Update(MD5Context* context, const uint8_t* buf, uint32_t len);
SG_SDK_TF2 void MD5Final(uint8_t digest[MD5_DIGEST_LENGTH], MD5Context* context);

inline void MD5_ProcessSingleBuffer(const void* p, int len, MD5Value& md5Result)
{
	MD5Context ctx;
	MD5Init(&ctx);
	MD5Update(&ctx, (unsigned char const*)p, len);
	MD5Final(md5Result.bits, &ctx);
}

inline uint32_t MD5_PseudoRandom(uint32_t nSeed)
{
	MD5Context ctx;
	MD5Value digset;

	MD5Init(&ctx);
	MD5Update(&ctx, (uint8_t*)&nSeed, sizeof(nSeed));
	MD5Final(digset.bits, &ctx);

	return *reinterpret_cast<uint32_t*>(digset.bits + 6);
}

TF2_NAMESPACE_END();

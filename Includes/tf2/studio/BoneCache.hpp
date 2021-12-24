#pragma once

#include <vector>
#include <tf2/math/Vector.hpp>
#include "Studio.hpp"

TF2_NAMESPACE_BEGIN();

class IBoneCache
{
public:
	/// <summary>
	/// Update bone cache
	/// </summary>
	SG_SDK_TF2 void
		UpdateBones(const Matrix3x4_F* pBoneToWorld, int numbones, float curtime);

	/// <summary>
	/// Get pointer to bone matrix
	/// </summary>
	/// <returns>a valid pointer to the cache, nullptr if it doesn't exists</returns>
	SG_SDK_TF2 const Matrix3x4_F*
		GetCachedBone(int studioIndex) const;

	/// <summary>
	/// Copy bone matrix to 'bones'
	/// </summary>
	SG_SDK_TF2 void
		ReadCachedBones(std::vector<Matrix3x4_F>& bones) const;
	
	/// <summary>
	/// Copy pointer to bone matrix to 'pbones'
	/// </summary>
	SG_SDK_TF2 void
		ReadCachedBonePointers(std::vector<const Matrix3x4_F*>& pbones) const;

	/// <summary>
	/// Query if the current cache time is valid
	/// </summary>
	bool IsValid(float curtime, float dt = 0.1f) const noexcept
	{
		return curtime - TimeValid <= dt;
	}

	float			TimeValid;
	int				BoneMask;

	unsigned int	Size;
	unsigned short	CachedBoneCount;
	unsigned short	MatrixOffset;
	unsigned short	CachedToStudioOffset;
	unsigned short	BoneOutOffset;

private:
	const Matrix3x4_F* BoneArray() const
	{
		return std::bit_cast<const Matrix3x4_F*>(std::bit_cast<uint8_t*>(this + 1) + MatrixOffset);
	}
	const short* StudioToCached() const
	{
		return std::bit_cast<int16_t*>(this + 1);
	}
	const short* CachedToStudio() const
	{
		return std::bit_cast<int16_t*>(this + 1) + CachedToStudioOffset;
	}
};

struct BoneResult
{
	Vector3D_F Position, Min, Max;
};

class IBoneInfo
{
public:
	IBoneInfo() = default;

	IBoneInfo(const IBoneCache* bonecache, const mstudiohitboxset_t* hitbox_set, const studiohdr_t* studio_hdr) :
		bonecache(bonecache), hitbox_set(hitbox_set), studio_hdr(studio_hdr)
	{ }

	operator bool() const noexcept
	{
		return bonecache != nullptr;
	}

	BoneResult GetBonePosition(int bone) const;

	void GetBonePosition(const int* bone, size_t size, std::vector<BoneResult>* positions) const;

	void GetBonePosition(std::vector<BoneResult>* positions) const;

	const IBoneCache* GetBoneCache() const noexcept
	{
		return bonecache;
	}

	const mstudiohitboxset_t* GetStudioHitbox() const noexcept
	{
		return hitbox_set;
	}

	const studiohdr_t* GetStudioHDR() const noexcept
	{
		return studio_hdr;
	}

private:
	const IBoneCache* bonecache{ };

	const mstudiohitboxset_t* hitbox_set;
	const studiohdr_t* studio_hdr;
};



TF2_NAMESPACE_END();

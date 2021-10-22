
#include "Studio/BoneCache.hpp"
#include "Engine/ModelInfo.hpp"

TF2_NAMESPACE_BEGIN();


const Matrix3x4_F* IBoneCache::GetCachedBone(int studioIndex) const
{
	int cachedIndex = StudioToCached()[studioIndex];
	return cachedIndex >= 0 ? static_cast<const Matrix3x4_F*>(BoneArray() + cachedIndex) : nullptr;
}

void IBoneCache::ReadCachedBones(std::vector<Matrix3x4_F>& bones) const
{
	const Matrix3x4_F* pBones = BoneArray();
	const short* cachedstudio = CachedToStudio();

	bones.resize(CachedBoneCount);
	for (size_t i = 0; i < CachedBoneCount; i++)
		bones[cachedstudio[i]] = pBones[i];
}

void IBoneCache::ReadCachedBonePointers(std::vector<const Matrix3x4_F*>& bones) const
{
	const Matrix3x4_F* pBones = BoneArray();
	const short* pCachedToStudio = CachedToStudio();

	for (int i = 0; i < CachedBoneCount; i++)
		bones[pCachedToStudio[i]] = pBones + i;
}

BoneResult IBoneInfo::GetBonePosition(const int hitbox) const
{
	const mstudiobbox_t* sbb = hitbox_set->GetHitbox(hitbox);
	const Matrix3x4_F* bone = bonecache->GetCachedBone(sbb->Bone);

	return { bone->get_column(3), sbb->Min, sbb->Max };
}

void IBoneInfo::GetBonePosition(const int* bone, size_t size, std::vector<BoneResult>* positions) const
{
	positions->reserve(size);

	for (size_t i = 0; i < size; i++)
		positions->emplace_back(GetBonePosition(bone[i]));
}

void IBoneInfo::GetBonePosition(std::vector<BoneResult>* positions) const
{
	const size_t num_of_bones = hitbox_set->NumHitboxes;
	positions->reserve(num_of_bones);

	for (size_t i = 0; i < num_of_bones; i++)
		positions->emplace_back(GetBonePosition(i));
}

TF2_NAMESPACE_END();

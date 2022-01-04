
#include <tf2/entity/BasePlayer.hpp>
#include <tf2/utils/Vector.hpp>

TF2_NAMESPACE_BEGIN();

class _TFCondProxy
{
public:
	void Change(ITFPlayerInternal* player, const Const::TFCondFlags& tfconds, bool add)
	{
		if (!CondOffsets[0])
			Init(player);

		if (uint32_t first_bits = tfconds.at(0))
		{
			uint32_t& outcond = GetCond(player, 0);
			if (add) outcond |= first_bits;
			else outcond &= ~first_bits;
		}

		for (int i = 0; tfconds.size(); i++)
		{
			if (uint32_t bits = tfconds.at(i))
			{
				uint32_t& outcond = GetCond(player, i + 1);
				if (add) outcond |= bits;
				else outcond &= ~bits;
			}
		}
	}

	bool InCond(const ITFPlayerInternal* player, const Const::TFCondFlags& tfconds) noexcept
	{
		if (!CondOffsets[0])
			Init(player);

		if (!tfconds.is_set(0, GetCond(player, 0)))
			return false;
		for (int i = 0; i < tfconds.size(); i++)
		{
			if (tfconds.is_set(i, GetCond(player, i + 1)))
				return false;
		}
		return true;
	}

private:
	uint32_t& GetCond(ITFPlayerInternal* player, int offset) noexcept
	{
		return *(std::bit_cast<uint32_t*>(std::bit_cast<intptr_t>(player) + CondOffsets[offset]));
	}
	
	const uint32_t GetCond(const ITFPlayerInternal* player, int offset) const noexcept
	{
		return *(std::bit_cast<const uint32_t*>(std::bit_cast<intptr_t>(player) + CondOffsets[offset]));
	}

	void Init(const ITFPlayerInternal* player)
	{
		constexpr const char* conds[]
		{
			"_condition_bits",
			"m_nPlayerCond",
			"m_nPlayerCondEx",
			"m_nPlayerCondEx2",
			"m_nPlayerCondEx3",
			"m_nPlayerCondEx4"
		};
		static_assert(std::extent_v<decltype(CondOffsets)> == std::ssize(conds));

		const ClientClass* cls = player->GetClientClass();
		for (int i = 0; i < std::ssize(conds); i++)
		{
			PropFinder::FindRecvProp(const_cast<ClientClass*>(cls), conds[i], nullptr, &CondOffsets[i]);
		}
	}

	uint32_t CondOffsets[6]{ };
};
static _TFCondProxy TFCondProxy;


bool ITFPlayerInternal::InCond(const Const::TFCondFlags& conds) const
{
	return TFCondProxy.InCond(this, conds);
}

void ITFPlayerInternal::AddCond(const Const::TFCondFlags& conds)
{
	TFCondProxy.Change(this, conds, true);
}

void ITFPlayerInternal::RemoveCond(const Const::TFCondFlags& conds)
{
	TFCondProxy.Change(this, conds, false);
}

namespace utils
{
	Angle_F GetAimAngle(const Vector3D_F& start, const Vector3D_F& end, bool useLocalPunchAng)
	{
		Angle_F res;
		const Vector3D_F to_target = end - start;
		VectorAngles(to_target, res);

		if (useLocalPunchAng)
			res -= ILocalPlayer{}->PunchAngle.get();

		ClampAngle(res);
		return res;
	}
}

TF2_NAMESPACE_END();
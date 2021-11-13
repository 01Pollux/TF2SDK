
#include "Entity/BasePlayer.hpp"
#include "Utils/Vector.hpp"

TF2_NAMESPACE_BEGIN();

class _TFCondProxy
{
public:
	void Change(ITFPlayerInternal* player, Const::TFCond tfcond, bool add)
	{
		if (!CondOffsets[0])
			Init(player);

		int cond = static_cast<int>(tfcond);
		int bit{ };
		int type = cond / 32;

		switch (type)
		{
		case 0:
		{
			bit = 1 << cond;
			uint32_t& cond = GetCond(player, 0);
			uint32_t& cond2 = GetCond(player, 1);

			if (add)
			{
				cond |= bit;
				cond2 |= bit;
			}
			else
			{
				cond &= ~bit;
				cond2 &= ~bit;
			}

			return;
		}

		case 1:
		{
			bit = 1 << (cond - 32);
			break;
		}

		case 2:
		{
			bit = 1 << (cond - 64);
			break;
		}

		case 3:
		{
			bit = 1 << (cond - 96);
			break;
		}

		case 4:
		{
			bit = 1 << (cond - 128);
			break;
		}
		}

		uint32_t& outcond = GetCond(player, type + 1);

		if (add) outcond |= bit;
		else outcond &= ~bit;
	}

	bool InCond(const ITFPlayerInternal* player, Const::TFCond tfcond) noexcept
	{
		if (!CondOffsets[0])
			Init(player);

		int cond = static_cast<int>(tfcond);
		int bit{ };
		int type = cond / 32;

		switch (type)
		{
		case 0:
		{
			bit = 1 << cond;
			if (GetCond(player, 0) & bit || GetCond(player, 1) & bit)
				return true;
			else return false;
		}

		case 1:
		{
			bit = 1 << (cond - 32);
			break;
		}

		case 2:
		{
			bit = 1 << (cond - 64);
			break;
		}

		case 3:
		{
			bit = 1 << (cond - 96);
			break;
		}

		case 4:
		{
			bit = 1 << (cond - 128);
			break;
		}
		}

		return (GetCond(player, type + 1) & bit) != 0;
	}

private:
	uint32_t& GetCond(ITFPlayerInternal* player, int offset) noexcept
	{
		return *(std::bit_cast<uint32_t*>(std::bit_cast<const uint32_t>(player) + CondOffsets[offset]));
	}
	
	const uint32_t GetCond(const ITFPlayerInternal* player, int offset) const noexcept
	{
		return *(std::bit_cast<const uint32_t*>(std::bit_cast<const uint32_t>(player) + CondOffsets[offset]));
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


bool ITFPlayerInternal::InCond(Const::TFCond cond) const
{
	return TFCondProxy.InCond(this, cond);
}

void ITFPlayerInternal::AddCond(Const::TFCond cond, float duration)
{
	TFCondProxy.Change(this, cond, true);
}

void ITFPlayerInternal::RemoveCond(Const::TFCond cond)
{
	TFCondProxy.Change(this, cond, false);
}

namespace Utils
{
	Angle_F GetAimAngle(const Vector3D_F& start, const Vector3D_F& end, bool useLocalPunchAng)
	{
		Angle_F res;
		const Vector3D_F to_target = end - start;
		VectorAngles(to_target, res);

		ITFPlayer ps;
		if (useLocalPunchAng)
			res -= ILocalPlayer{}->PunchAngle.get();

		ClampAngle(res);
		return res;
	}
}

TF2_NAMESPACE_END();
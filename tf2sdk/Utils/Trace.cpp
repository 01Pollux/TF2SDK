
#include <tf2/engine/ModelInfo.hpp>
#include <tf2/client/GameRules.hpp>
#include <tf2/engine/ClientDll.hpp>

#include <tf2/utils/Vector.hpp>
#include <tf2/utils/Trace.hpp>

TF2_NAMESPACE_BEGIN(::utils);


bool FilterSimple::ShouldHitEntity(IClientUnknown* pUnk, uint32_t mask)
{
	assert(interfaces::ModelInfo);
	assert(interfaces::GameRules);

	IBaseEntity pEnt(pUnk->GetBaseEntity());
	if (!pEnt)
		return false;

	const ModelInfo* mdl = pEnt->GetModel();
	if ((interfaces::ModelInfo->GetModelType(mdl) != Const::ModelType::Brush) || (pEnt->SolidType != Const::EntSolidType::BSP && pEnt->SolidType != Const::EntSolidType::VPhysics))
	{
		if (!(mask & Const::ContentsFlags::Monster))
			return false;
	}

	if (!(mask & Const::ContentsFlags::Window) && pEnt->IsTransparent())
		return false;

	if (!(mask & Const::ContentsFlags::Moveable) && pEnt->MoveType == Const::EntMoveType::Push)
		return false;

	if (m_IgnoreEntity)
	{
		if (m_IgnoreEntity == pEnt ||
			pEnt->OwnerEntity == m_IgnoreEntity->GetRefEHandle() ||
			m_IgnoreEntity->OwnerEntity == pEnt->GetRefEHandle())
			return false;
	}

	if (!pEnt->ShouldCollide(this->m_CollisionGroup, mask))
		return false;

	if (!IGameRules::ShouldCollide(m_CollisionGroup, pEnt->CollisionGroup))
		return false;

	return m_Callback ? m_Callback(pEnt.get(), mask) : true;
}

float DistanceToGround(Vector3D_F position, IBaseEntityInternal* pIgnore)
{
	assert(interfaces::ClientTrace);

	Vector3D_F bottom;
	AngleVectors({ 90.f, 0.f, 0.f }, &bottom);

	bottom.normalize();
	bottom = bottom * Const::MaxTrace_Length + position;

	GameTrace results;
	FilterSimple filter_only_world{
		pIgnore,
		Const::EntCollisionGroup::None,
		[](IBaseEntityInternal* pEnt, uint32_t collision_group) -> bool
		{
			return pEnt->GetEntIndex() == 0;
		}
	};

	position[2] += 10.f;
	utils::TraceLine(position, bottom, Const::TraceMask::PlayerSolid, results, filter_only_world);
	if (results.DidHit())
	{
		position[2] -= 10.f;
		return static_cast<float>(results.EndPos.distance_to(position));
	}
	else return 0.f;
}

float DistanceToGround(Vector3D_F position, const Vector3D_F& mins, const Vector3D_F& maxs, IBaseEntityInternal* pIgnore)
{
	assert(interfaces::ClientTrace);

	Vector3D_F bottom;
	AngleVectors({ 90.f, 0.f, 0.f }, &bottom);

	bottom.normalize();
	bottom = bottom * Const::MaxTrace_Length + position;

	GameTrace results;
	FilterSimple filter_only_world{
		pIgnore,
		Const::EntCollisionGroup::None,
		[](IBaseEntityInternal* pEnt, uint32_t collision_group) -> bool
		{
			return pEnt->GetEntIndex() == 0;
		}
	};

	position[2] += 10.f;
	utils::TraceHull(position, bottom, mins, maxs, Const::TraceMask::PlayerSolid, results, filter_only_world);
	if (results.DidHit())
	{
		position[2] -= 10.f;
		return static_cast<float>(results.EndPos.distance_to(position));
	}
	else return 0.f;
}


TF2_NAMESPACE_END();

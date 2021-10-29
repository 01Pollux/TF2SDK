
#include "Utils/Trace.hpp"
#include "Engine/ModelInfo.hpp"
#include "Client/GameRules.hpp"

TF2_NAMESPACE_BEGIN(::Utils);


bool FilterSimple::ShouldHitEntity(IClientUnknown* pUnk, uint32_t mask)
{
	IBaseEntity pEnt(pUnk->GetBaseEntity());
	if (!pEnt)
		return false;

	Const::EntSolidType solidtype = pEnt->SolidType;
	const ModelInfo* mdl = pEnt->GetModel();

	if ((Interfaces::ModelInfo->GetModelType(mdl) != Const::ModelType::Brush) || (solidtype != Const::EntSolidType::BSP && solidtype != Const::EntSolidType::VPhysics))
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

	// CBaseEntity::ShouldCollide todo : Fool proof check it in IDA
	if (pEnt->CollisionGroup == Const::EntCollisionGroup::Debris)
	{
		if (!(mask & Const::ContentsFlags::Debris))
			return false;
	}

	// TODO: add TFGameRules::ShouldCollide
	if (!Interfaces::GameRules->ShouldCollide(m_CollisionGroup, pEnt->CollisionGroup))
		return false;
	
	return m_Callback ? m_Callback(pEnt.get(), mask) : true;
}


TF2_NAMESPACE_END();

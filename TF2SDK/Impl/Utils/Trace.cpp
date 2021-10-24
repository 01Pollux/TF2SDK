
#include "Utils/Trace.hpp"
#include "Engine/ModelInfo.hpp"

TF2_NAMESPACE_BEGIN(::Utils);


bool FilterSimple::ShouldHitEntity(IClientUnknown* pUnk, int mask)
{
	IBaseEntity pEnt(pUnk->GetBaseEntity());
	if (!pEnt)
		return false;

	ICollideable* pCol = pEnt->CollisionProp.data();

	Const::EntSolidType solidtype = pCol->GetSolid();
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

	if (m_Callback && !m_Callback(pEnt.get(), mask))
		return false;
	else return true;
}


TF2_NAMESPACE_END();

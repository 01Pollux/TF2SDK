#pragma once

#include <functional>
#include <tf2/config.hpp>
#include <tf2/engine/Trace.hpp>

TF2_NAMESPACE_BEGIN(::utils);

using TraceFilterCallback = std::function<bool(IBaseEntityInternal*, uint32_t)>;

class FilterGeneric : public ITraceFilter
{
public:
	Const::GameTraceType GetTraceType() const override
	{
		return Const::GameTraceType::Everything;
	}
};


class FilterEntitiesOnly : public ITraceFilter
{
public:
	Const::GameTraceType GetTraceType() const override
	{
		return Const::GameTraceType::EntitiesOnly;
	}
};


class FilterWorldOnly : public ITraceFilter
{
public:
	bool ShouldHitEntity(IClientUnknown*, uint32_t) final
	{
		return false;
	}

	Const::GameTraceType GetTraceType() const final
	{
		return Const::GameTraceType::WorldOnly;
	}
};


class FilterWorldAndPropsOnly : public ITraceFilter
{
public:
	bool ShouldHitEntity(IClientUnknown*, uint32_t) final
	{
		return false;
	}

	Const::GameTraceType GetTraceType() const final
	{
		return Const::GameTraceType::Everything;
	}
};


class FilterHitAll : public FilterGeneric
{
public:
	bool ShouldHitEntity(IClientUnknown*, uint32_t) final
	{
		return true;
	}

	Const::GameTraceType GetTraceType() const final
	{
		return Const::GameTraceType::Everything;
	}
};


class FilterSimple : public FilterGeneric
{
public:
	FilterSimple(
		const IBaseEntityInternal* ignore = nullptr,
		Const::EntCollisionGroup collision_group = Const::EntCollisionGroup::None, 
		const TraceFilterCallback& extra = nullptr
	) noexcept : m_IgnoreEntity(ignore), m_CollisionGroup(collision_group), m_Callback(extra) { };

	PX_SDK_TF2 bool ShouldHitEntity(IClientUnknown* pUnk, uint32_t mask) override;

protected:
	const IBaseEntityInternal* m_IgnoreEntity;
	const Const::EntCollisionGroup m_CollisionGroup;
	const TraceFilterCallback m_Callback;
};


class FilterIgnoreAllExcept : public FilterSimple
{
public:
	FilterIgnoreAllExcept(
		std::vector<const IBaseEntityInternal*> entries,
		const IBaseEntityInternal* ignore = nullptr,
		Const::EntCollisionGroup collision_group = Const::EntCollisionGroup::None,
		const TraceFilterCallback& extra = nullptr
		) noexcept : FilterSimple(ignore, collision_group, extra), m_Entities(entries) { };

	bool ShouldHitEntity(IClientUnknown* pUnk, uint32_t mask) override
	{
		for (auto entity : m_Entities)
			if (pUnk == entity)
				return FilterSimple::ShouldHitEntity(pUnk, mask);

		return false;
	}

	void reserve(size_t size)
	{
		m_Entities.reserve(size);
	}

	void add(const IBaseEntityInternal* ent) noexcept
	{
		m_Entities.push_back(ent);
	}

protected:
	std::vector<const IBaseEntityInternal*> m_Entities;
};


class FilterIgnoreAllExceptOne : public FilterSimple
{
public:
	FilterIgnoreAllExceptOne(
		const IBaseEntityInternal* entity,
		const IBaseEntityInternal* ignore = nullptr,
		Const::EntCollisionGroup collision_group = Const::EntCollisionGroup::None,
		const TraceFilterCallback& extra = nullptr
	) noexcept : FilterSimple(ignore, collision_group, extra), m_Entity(entity) { };

	bool ShouldHitEntity(IClientUnknown* pUnk, uint32_t mask) override
	{
		if (pUnk == m_Entity)
			return FilterSimple::ShouldHitEntity(pUnk, mask);

		return false;
	}

protected:
	const IBaseEntityInternal* m_Entity;
};


class FilterAcceptAllExcept : public FilterSimple
{
public:
	FilterAcceptAllExcept(
		std::vector<const IBaseEntityInternal*> entries,
		const IBaseEntityInternal* ignore = nullptr,
		Const::EntCollisionGroup collision_group = Const::EntCollisionGroup::None,
		const TraceFilterCallback& extra = nullptr
	) noexcept : FilterSimple(ignore, collision_group, extra), m_Entities(entries) { };

	bool ShouldHitEntity(IClientUnknown* pUnk, uint32_t mask) override
	{
		for (auto entity : m_Entities)
			if (pUnk == entity)
				return false;

		return FilterSimple::ShouldHitEntity(pUnk, mask);
	}

	void reserve(size_t size)
	{
		m_Entities.reserve(size);
	}

	void add(const IBaseEntityInternal* ent) noexcept
	{
		m_Entities.push_back(ent);
	}

protected:
	std::vector<const IBaseEntityInternal*> m_Entities;
};

class FilterAcceptAllExceptOne : public FilterSimple
{
public:
	FilterAcceptAllExceptOne(
		const IBaseEntityInternal* entity,
		const IBaseEntityInternal* ignore = nullptr,
		Const::EntCollisionGroup collision_group = Const::EntCollisionGroup::None,
		const TraceFilterCallback& extra = nullptr
	) noexcept : FilterSimple(ignore, collision_group, extra), m_Entity(entity) { };

	bool ShouldHitEntity(IClientUnknown* pUnk, uint32_t mask) override
	{
		if (m_Entity == pUnk)
			return false;

		return FilterSimple::ShouldHitEntity(pUnk, mask);
	}

protected:
	const IBaseEntityInternal* m_Entity;
};

inline void TraceLine(
	const Vector3D_F& vec1,
	const Vector3D_F& vec2,
	uint32_t mask,
	GameTrace& results,
	ITraceFilter& trace
)
{
	assert(interfaces::ClientTrace);

	GameRay ray{ vec1, vec2 };
	interfaces::ClientTrace->TraceRay(ray, mask, &trace, &results);
}

inline void TraceHull(
	const Vector3D_F& vec1,
	const Vector3D_F& vec2,
	const Vector3D_F& mins,
	const Vector3D_F& maxs,
	uint32_t mask,
	GameTrace& results,
	ITraceFilter& trace
)
{
	assert(interfaces::ClientTrace);

	GameRay ray{ vec1, vec2, mins, maxs };
	interfaces::ClientTrace->TraceRay(ray, mask, &trace, &results);
}

inline bool VecIsVisible(
	const Vector3D_F& start,
	const Vector3D_F& end,
	const IBaseEntityInternal* pEnt,
	const IBaseEntityInternal* pIgnore = nullptr,
	uint32_t mask = Const::TraceMask::Shot
)
{
	GameTrace res;
	FilterSimple filter(pIgnore);
	TraceLine(start, end, mask, res, filter);

	return res.Entity == pEnt;
}

PX_SDK_TF2 float DistanceToGround(Vector3D_F position, IBaseEntityInternal* pIgnore = nullptr);
PX_SDK_TF2 float DistanceToGround(Vector3D_F position, const Vector3D_F& mins, const Vector3D_F& maxs, IBaseEntityInternal* pIgnore = nullptr);

TF2_NAMESPACE_END();

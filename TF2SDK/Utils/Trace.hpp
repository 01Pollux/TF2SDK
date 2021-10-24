#pragma once

#include "TF2Config.hpp"
#include "Engine/Trace.hpp"
#include <functional>

TF2_NAMESPACE_BEGIN(::Utils);

using TraceFilterCallback = std::function<bool(IBaseEntityInternal*, int)>;

class GenericFilter : public ITraceFilter
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
	bool ShouldHitEntity(IClientUnknown*, int) final
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
	bool ShouldHitEntity(IClientUnknown*, int) final
	{
		return false;
	}

	Const::GameTraceType GetTraceType() const final
	{
		return Const::GameTraceType::Everything;
	}
};


class FilterHitAll : public GenericFilter
{
public:
	bool ShouldHitEntity(IClientUnknown*, int) final
	{
		return true;
	}

	Const::GameTraceType GetTraceType() const final
	{
		return Const::GameTraceType::Everything;
	}
};


class FilterSimple : public GenericFilter
{
public:
	FilterSimple(const IBaseEntityInternal* pIgnore = nullptr, const TraceFilterCallback& extra = nullptr) noexcept : m_IgnoreEntity(pIgnore), m_Callback(extra) { };

	SG_SDK_TF2 bool ShouldHitEntity(IClientUnknown* pUnk, int mask) override;

protected:
	const IBaseEntityInternal* m_IgnoreEntity{ };
	const TraceFilterCallback m_Callback;
};


class FilterIgnoreAllExcept : public GenericFilter
{
public:
	FilterIgnoreAllExcept(std::initializer_list<IBaseEntityInternal*> entries) noexcept : m_Entities(entries) { };

	bool ShouldHitEntity(IClientUnknown* pUnk, int) override
	{
		if (!pUnk)
			return false;

		for (auto entity : m_Entities)
			if (pUnk == entity)
				return true;

		return false;
	}

	void reserve(size_t size)
	{
		m_Entities.reserve(size);
	}

	void add(IBaseEntityInternal* ent) noexcept
	{
		m_Entities.push_back(ent);
	}

protected:
	std::vector<IBaseEntityInternal*> m_Entities;
};

class FilterIgnoreAllExceptOne : public GenericFilter
{
public:
	FilterIgnoreAllExceptOne(const IBaseEntityInternal* ent) noexcept : Entity(ent) { };

	bool ShouldHitEntity(IClientUnknown* pUnk, int) override
	{
		if (pUnk == Entity)
			return true;

		return false;
	}

protected:
	const IBaseEntityInternal* Entity;
};

class FilterAcceptAllExcept : public GenericFilter
{
public:
	FilterAcceptAllExcept(std::initializer_list<const IBaseEntityInternal*> entries) noexcept : m_Entities(entries) { };

	bool ShouldHitEntity(IClientUnknown* incoming, int) override
	{
		if (!incoming)
			return false;

		for (auto entity : m_Entities)
			if (incoming == entity)
				return false;

		return true;
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

class FilterAcceptAllExceptOne : public GenericFilter
{
public:
	FilterAcceptAllExceptOne(const IBaseEntityInternal* ent) noexcept : m_Entity(ent) { };

	bool ShouldHitEntity(IClientUnknown* pUnk, int) override
	{
		if (m_Entity == pUnk)
			return false;

		return pUnk != nullptr;
	}

protected:
	const IBaseEntityInternal* m_Entity;
};

inline void TraceLine(
	const Vector3D_F& vec1,
	const Vector3D_F& vec2,
	uint32_t mask,
	GameTrace* results,
	ITraceFilter* trace
)
{
	GameRay ray{ vec1, vec2 };
	Interfaces::ClientTrace->TraceRay(ray, mask, trace, results);
}

inline void TraceHull(
	const Vector3D_F& vec1,
	const Vector3D_F& vec2,
	const Vector3D_F& mins,
	const Vector3D_F& maxs,
	uint32_t mask,
	GameTrace* results,
	ITraceFilter* trace
)
{
	GameRay ray{ vec1, vec2, mins, maxs };
	Interfaces::ClientTrace->TraceRay(ray, mask, trace, results);
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
	TraceLine(start, end, mask, &res, &filter);

	return res.Entity == pEnt;
}

TF2_NAMESPACE_END();

#pragma once

#include <list>
#include <memory>
#include "TF2Config.hpp"

TF2_NAMESPACE_BEGIN(::Utils);


template<typename _Ty>
class IAutoList
{
public:
	using container_type = std::list<_Ty*>;
	
	IAutoList()
	{
		if (!Entries)
			Init();
		Entries->push_back(static_cast<_Ty*>(this));
	}

	virtual ~IAutoList()
	{
		if (Entries)
		{
			Entries->erase(std::find(Entries->cbegin(), Entries->cend(), static_cast<_Ty*>(this)));
			if (Entries->empty())
				Entries = nullptr;
		}
	}

	static const auto& GetEntries() noexcept
	{
		return *Entries;
	}

	static void RemoveAll() noexcept
	{
		Entries = nullptr;
	}

	IAutoList(IAutoList&) = delete;
	IAutoList& operator=(const IAutoList&) = delete;
	IAutoList(IAutoList&&) = delete;
	IAutoList& operator=(IAutoList&&) = delete;

private:
	static void Init()
	{
		Entries = std::make_unique<container_type>();
	}

	static inline std::unique_ptr<container_type> Entries;
};

class MyCls : IAutoList<MyCls>
{
public:

};


TF2_NAMESPACE_END();
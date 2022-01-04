#include <px/interfaces/GameData.hpp>

#include <tf2/entity/EntityIterator.hpp>

#include <tf2/utils/UtlLinkedList.hpp>

TF2_NAMESPACE_BEGIN(::utils);

namespace Impl::EntityIterator
{
	struct Iterator
	{
		static auto Get()
		{
			static auto EntityList = IntPtr(
				interfaces::SDKManager::Get()->ReadSignature("pCEntityListPtr") +
				interfaces::SDKManager::Get()->ReadOffset({ "CEntityList", "offsets" }, "BaseEntities").value_or(0)
			).get<utils::UtlLinkedList<IBaseEntityInternal*, unsigned short>>();
			assert(EntityList);
			return EntityList;
		}
	};
}


_NODISCARD Impl::EntityIterator::const_iterator::reference Impl::EntityIterator::const_iterator::operator*() const
{
	return Impl::EntityIterator::Iterator::Get()->at(m_CurrentIndex);
}

_NODISCARD Impl::EntityIterator::iterator::reference Impl::EntityIterator::iterator::operator*() const
{
	return Impl::EntityIterator::Iterator::Get()->at(m_CurrentIndex);
}


IBaseEntityIterator::const_iterator& IBaseEntityIterator::const_iterator::operator++()
{
	m_CurrentIndex = Impl::EntityIterator::Iterator::Get()->next(m_CurrentIndex);
	return *this;
}
IBaseEntityIterator::const_iterator& IBaseEntityIterator::const_iterator::operator--()
{
	m_CurrentIndex = Impl::EntityIterator::Iterator::Get()->previous(m_CurrentIndex);
	return *this;
}

IBaseEntityIterator::iterator& IBaseEntityIterator::iterator::operator++()
{
	m_CurrentIndex = Impl::EntityIterator::Iterator::Get()->next(m_CurrentIndex);
	return *this;
}
IBaseEntityIterator::iterator& IBaseEntityIterator::iterator::operator--()
{
	m_CurrentIndex = Impl::EntityIterator::Iterator::Get()->previous(m_CurrentIndex);
	return *this;
}


IBaseEntityIterator::const_iterator IBaseEntityIterator::begin() const
{
	return Impl::EntityIterator::Iterator::Get()->head();
}

IBaseEntityIterator::iterator IBaseEntityIterator::begin()
{
	return Impl::EntityIterator::Iterator::Get()->head();
}


IBaseEntityIterator::const_reverse_iterator IBaseEntityIterator::rbegin() const
{
	return const_reverse_iterator(Impl::EntityIterator::Iterator::Get()->tail());
}

IBaseEntityIterator::reverse_iterator IBaseEntityIterator::rbegin()
{
	return reverse_iterator(Impl::EntityIterator::Iterator::Get()->tail());
}


TF2_NAMESPACE_END();


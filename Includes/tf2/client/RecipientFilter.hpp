#pragma once

#include <tf2/config.hpp>

TF2_NAMESPACE_BEGIN();

class IRecipientFilter
{
public:
	virtual			~IRecipientFilter() = default;

	virtual bool	IsReliable() const abstract;
	virtual bool	IsInitMessage() const abstract;

	virtual int		GetRecipientCount() const abstract;
	virtual int		GetRecipientIndex(int slot) const abstract;
};


class CRecipientFilter : public IRecipientFilter
{
public:

	CRecipientFilter(bool reliable = false, bool is_init_msg = false) : 
		m_IsReliable(reliable), m_IsInitMsg(is_init_msg)
	{ }

	bool IsReliable() const
	{
		return m_IsReliable;
	}

	bool IsInitMessage() const
	{
		return m_IsInitMsg;
	}

	int	GetRecipientCount() const
	{
		return static_cast<int>(m_Recipients.size());
	}

	int	GetRecipientIndex(int slot) const
	{
		return m_Recipients[slot];
	}

	bool SetReliable(bool state = true)
	{
		m_IsReliable = state;
	}

	bool SetInitMsg(bool state = true)
	{
		m_IsInitMsg = state;
	}

	void Reserve(size_t size)
	{
		m_Recipients.reserve(size);
	}

	void Push(int index)
	{
		m_Recipients.push_back(index);
	}

private:
	bool m_IsReliable;
	bool m_IsInitMsg;
	std::vector<int> m_Recipients;
};


TF2_NAMESPACE_END();

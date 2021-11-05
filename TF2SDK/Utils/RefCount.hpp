#pragma once

#include "TF2Config.hpp"

TF2_NAMESPACE_BEGIN(::Utils);


//-----------------------------------------------------------------------------
// Purpose:	Implement a standard reference counted interface. Use of this
//			is optional insofar as all the concrete tools only require
//			at compile time that the function signatures match.
//-----------------------------------------------------------------------------

class IRefCounted
{
public:
	virtual int AddRef() abstract;
	virtual int Release() abstract;
};

template <class T = IRefCounted>
class CAutoRef
{
public:
	CAutoRef(T* pRef)
		: m_Ref(pRef)
	{
		if (m_Ref)
			m_Ref->AddRef();
	}

	~CAutoRef()
	{
		if (m_Ref)
			m_Ref->Release();
	}

private:
	T* m_Ref;
};

TF2_NAMESPACE_END();

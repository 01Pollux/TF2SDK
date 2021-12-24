#pragma once

#include <tf2/consts.hpp>

TF2_NAMESPACE_BEGIN(::Utils::VAlloc);


template <class T>
inline T* Construct(T* pMemory)
{
	return reinterpret_cast<T*>(::new(pMemory) T);
}

template <class T, typename ARG1>
inline T* Construct(T* pMemory, ARG1 a1)
{
	return reinterpret_cast<T*>(::new(pMemory) T(a1));
}

template <class T, typename ARG1, typename ARG2>
inline T* Construct(T* pMemory, ARG1 a1, ARG2 a2)
{
	return reinterpret_cast<T*>(::new(pMemory) T(a1, a2));
}

template <class T, typename ARG1, typename ARG2, typename ARG3>
inline T* Construct(T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3)
{
	return reinterpret_cast<T*>(::new(pMemory) T(a1, a2, a3));
}

template <class T, typename ARG1, typename ARG2, typename ARG3, typename ARG4>
inline T* Construct(T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4)
{
	return reinterpret_cast<T*>(::new(pMemory) T(a1, a2, a3, a4));
}

template <class T, typename ARG1, typename ARG2, typename ARG3, typename ARG4, typename ARG5>
inline T* Construct(T* pMemory, ARG1 a1, ARG2 a2, ARG3 a3, ARG4 a4, ARG5 a5)
{
	return reinterpret_cast<T*>(::new(pMemory) T(a1, a2, a3, a4, a5));
}

template <class T, class P>
inline void ConstructOneArg(T* pMemory, P const& arg)
{
	::new(pMemory) T(arg);
}

template <class T, class P1, class P2 >
inline void ConstructTwoArg(T* pMemory, P1 const& arg1, P2 const& arg2)
{
	::new(pMemory) T(arg1, arg2);
}

template <class T, class P1, class P2, class P3 >
inline void ConstructThreeArg(T* pMemory, P1 const& arg1, P2 const& arg2, P3 const& arg3)
{
	::new(pMemory) T(arg1, arg2, arg3);
}

template <class T>
inline T* CopyConstruct(T* pMemory, T const& src)
{
	return reinterpret_cast<T*>(::new(pMemory) T(src));
}

template <class T>
inline void Destruct(T* pMemory)
{
	pMemory->~T();
}

// The above will error when binding to a type of: foo(*)[] -- there is no provision in c++ for knowing how many objects
// to destruct without preserving the count and calling the necessary destructors.
template <class T, size_t N>
inline void Destruct(T(*pMemory)[N])
{
	for (size_t i = 0; i < N; i++)
	{
		(pMemory[i])->~T();
	}
}

TF2_NAMESPACE_END();
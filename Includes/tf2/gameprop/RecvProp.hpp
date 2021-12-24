#pragma once

#include "Consts.hpp"

TF2_NAMESPACE_BEGIN();

class IRecvProxyData;
class IClientNetworkable;
struct RecvTable;
struct RecvProp;


/// <summary>
/// ArrayLengthRecvProxies are optionally used to get the length of the 
/// incoming array when it changes.
/// </summary>
using ArrayLengthRecvProxyFn = void(*)(void* pThisPtr, int objectID, int arraySize);

/// <summary>
/// Convert the network-standard-type value in m_Value into your own format in pStruct/pOut.
/// pStruct = the base structure of the datatable this variable is in (like C_BaseEntity)
/// pOut    = the variable that this this proxy represents (like C_BaseEntity::m_SomeValue).
/// </summary>
using RecvVarProxyFn = void(*)(const IRecvProxyData* pData, void* pThis, void* pOut);

/// <summary>
/// NOTE: DataTable receive proxies work differently than the other proxies.
/// pData points at the object + the recv table's offset.
/// pOut should be set to the location of the object to unpack the data table into.
/// If the parent object just contains the child object, the default proxy just does *pOut = pData.
/// If the parent object points at the child object, you need to dereference the pointer here.
/// NOTE: don't ever return null from a DataTable receive proxy function. Bad things will happen.
/// </summary>
using DataTableRecvVarProxyFn = void(*)(const RecvProp* pProp, void** pOut, void* pData, int objectID);



struct RecvProp
{
	const char*		Name;
	Const::PropType	Type;
	int				Flags;
	int				StringBufferSize;

	/// <summary>
	/// Set to true by the engine if this property sits inside an array
	/// </summary>
	const bool IsInsideArray;

	/// <summary>
	/// Extra data that certain special property types bind to the property here
	/// </summary>
	const void* ExtraData;

	/// <summary>
	/// If this is an array (PropType::Array)
	/// </summary>
	RecvProp* ArrayProp;

	/// <summary>
	/// Array's set size callback
	/// </summary>
	ArrayLengthRecvProxyFn	ArrayLengthProxy;

	/// <summary>
	/// Var's set callback
	/// </summary>
	RecvVarProxyFn			ProxyFn;

	/// <summary>
	/// for PropType::DataTable
	/// </summary>
	DataTableRecvVarProxyFn	DataTableProxyFn;

	/// <summary>
	/// for PropType::DataTable
	/// </summary>
	RecvTable* DataTable;

	const int Offset;

	/// <summary>
	/// Pointer distance between array elements
	/// </summary>
	int	ElementStride;

	/// <summary>
	/// Number of elements in the array (or 1 if it's not an array)
	/// </summary>
	int	NumElements;

	/// <summary>
	/// If it's one of the numbered "000", "001", etc properties in an array, then
	/// these can be used to get its array property name for debugging
	/// </summary>
	const char* const ParentArrayPropName;
};


struct RecvTable
{
	/// <summary>
	/// Props in the table
	/// </summary>
	RecvProp* Props;

	/// <summary>
	/// size of props in the table
	/// </summary>
	int	NumProps;

	const void* const Decoder;

	/// <summary>
	/// Name of the table
	/// </summary>
	const char* const Name;

	const bool IsInitialized;

	const bool IsInMainList;
};


class ClientClass
{
private:
	IClientNetworkable* (*_CreateCallFunc)(int, int);
	IClientNetworkable* (*_CreateEventFunc)();

public:
	const char* const NetworkName;
	RecvTable* RecvTable;

	ClientClass* const NextClass;
	const Const::EntClassID ClassID;
};


namespace PropFinder
{
	/// <summary>
	/// Find RecvProp from ClientClass
	/// </summary>
	/// <param name="pProp">[out] pointer to RecvProp*</param>
	/// <param name="pOffset">[out] pointer to offset</param>
	/// <returns>true if the prop exists, false otherwise</returns>
	SG_SDK_TF2 bool 
		FindRecvProp(ClientClass* pClass, const char* prop_name, RecvProp** pProp, uint32_t* pOffset);

	/// <summary>
	/// Find RecvProp from class name
	/// </summary>
	/// <param name="pProp">[out] pointer to RecvProp*</param>
	/// <param name="pOffset">[out] pointer to offset</param>
	/// <returns>true if the prop exists, false otherwise</returns>
	SG_SDK_TF2 bool 
		FindRecvProp(const char* class_name, const char* prop_name, RecvProp** pProp, uint32_t* pOffset);

	/// <summary>
	/// Find RecvProp from class name
	/// </summary>
	/// <param name="pProp">[out] pointer to RecvProp*</param>
	/// <param name="pOffset">[out] pointer to offset</param>
	/// <returns>true if the prop exists, false otherwise</returns>
	SG_SDK_TF2 bool 
		FindRecvProp(Const::EntClassID class_id, const char* prop_name, RecvProp** pProp, uint32_t* pOffset);
}


TF2_NAMESPACE_END();

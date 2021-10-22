#pragma once

#include "Consts.hpp"

TF2_NAMESPACE_BEGIN();

struct SendProp;
struct RecvProp;
struct SendTable;
class CSendProxyRecipients;


/// <summary>
/// ArrayLengthSendProxies are used when you want to specify an array's length dynamically
/// </summary>
using ArrayLengthSendProxyFn = int(*)(const void* pThisPtr, int objectID);

/// <summary>
/// 
/// <summary>
/// Send proxies can be used to convert a variable into a networkable type 
///  (a good example is converting an edict pointer into an integer index).
/// These allow you to translate data. For example, if you had a user-entered 
/// string number like "10" (call the variable pUserStr) and wanted to encode 
/// it as an integer, you would use a SendPropInt32 and write a proxy that said:
/// pOut->m_Int = atoi(pUserStr);
///
/// pProp       : the SendProp that has the proxy
/// pStructBase : the base structure (like CBaseEntity*).
/// pData       : the address of the variable to proxy.
/// pOut        : where to output the proxied value.
/// iElement    : the element index if this data is part of an array (or 0 if not).
/// objectID    : entity index for debugging purposes.
/// 
/// Return false if you don't want the engine to register and send a delta to
/// the clients for this property (regardless of whether it actually changed or not).
/// </summary>
using SendVarProxyFn = void(*)(const SendProp* pProp, const void* pStructBase, const void* pData, Const::PropVariant* pOut, int iElement, int objectID);

/// <summary>
/// Return the pointer to the data for the datatable.
/// If the proxy returns null, it's the same as if pRecipients->ClearAllRecipients() was called.
/// </summary>
using SendTableProxyFn = void(*)(const SendProp* pProp, const void* pStructBase, const void* pData, CSendProxyRecipients* pOut, int objectID);



struct SendProp
{
	/// <summary>
	/// This is temporary and only used while precalculating data for the decoders
	/// </summary>
	RecvProp*		ClientProp;

	Const::PropType	Type;
	int				Bits;
	float			LowValue;
	float			HighValue;

	/// <summary>
	/// If this is an array, this is the property that defines each array element (PropType::Array)
	/// </summary>
	SendProp*	ArrayProp;

	/// <summary>
	/// This callback returns the array length
	/// </summary>
	ArrayLengthSendProxyFn ArrayLengthProxy;


	/// <summary>
	/// Number of elements in the array (or 1 if it's not an array)
	/// </summary>
	int	NumElements;

	/// <summary>
	/// Pointer distance between array elements
	/// </summary>
	int	ElementStride;

	/// <summary>
	/// If this is an exclude prop, then this is the name of the datatable to exclude a prop from
	/// </summary>
	const char* const ExcludeName;

	const char* const ParrentArrayName;

	const char* const Name;

	float HighLowMul;

	/// <summary>
	/// PropFlags::*
	/// </summary>
	int Flags;

	/// <summary>
	/// NULL for PropType::DataTable
	/// </summary>
	SendVarProxyFn VarProxyFn;

	/// <summary>
	/// Valid for PropType::DataTable
	/// </summary>
	SendTableProxyFn TableProxy;


	SendTable* DataTable;

	/// <summary>
	/// SENDPROP_VECTORELEM makes this negative to start with so we can detect that and
	/// set the SPROP_IS_VECTOR_ELEM flag.
	/// </summary>
	const int Offset;

	/// <summary>
	/// Extra data bound to this property
	/// </summary>
	const void* ExtraData;
};


struct SendTable
{
	/// <summary>
	/// Props in the table
	/// </summary>
	SendProp* Props;

	/// <summary>
	/// size of props in the table
	/// </summary>
	int	NumProps;

	/// <summary>
	/// Name of the table
	/// </summary>
	const char* const Name;

	/// <summary>
	/// The engine hooks the SendTable here
	/// </summary>
	const void* PreCalc;
	const bool IsInitialized;

	const bool IsInMainList;

private:
	bool Initialized : 1;
	bool HasBeenWritten : 1;
	// m_flSimulationTime and m_flAnimTime, e.g.
	bool HasPropsEncodedAgainstCurrentTickCount : 1;
};


struct ServerClass
{
	const char* const NetworkName;
	SendTable* SendTable;

	ServerClass* const NextClass;
	const Const::EntClassID ClassID;

	int BaseLineIndex;
};

namespace PropFinder
{
	/// <summary>
	/// Find RecvProp from ClientClass
	/// </summary>
	/// <param name="pProp">[out] pointer to RecvProp*</param>
	/// <param name="pOffset">[out] pointer to offset</param>
	/// <returns>true if the prop exists, false otherwise</returns>
	SG_SDK_TF2
	bool FindSendProp(ServerClass* pClass, const char* prop_name, SendProp** pProp, uint32_t* pOffset);

	/// <summary>
	/// Find RecvProp from class name
	/// </summary>
	/// <param name="pProp">[out] pointer to RecvProp*</param>
	/// <param name="pOffset">[out] pointer to offset</param>
	/// <returns>true if the prop exists, false otherwise</returns>
	SG_SDK_TF2
	bool FindSendProp(const char* class_name, const char* prop_name, SendProp** pProp, uint32_t* pOffset);

	/// <summary>
	/// Find RecvProp from class name
	/// </summary>
	/// <param name="pProp">[out] pointer to RecvProp*</param>
	/// <param name="pOffset">[out] pointer to offset</param>
	/// <returns>true if the prop exists, false otherwise</returns>
	SG_SDK_TF2
	bool FindSendProp(Const::EntClassID class_id, const char* prop_name, SendProp** pProp, uint32_t* pOffset);
}



TF2_NAMESPACE_END();

#pragma once

#include <tf2/config.hpp>
#include "ClassID.hpp"

TF2_NAMESPACE_BEGIN(::Const);

enum class PropType
{
	Int,
	Float,
	Vector,
	VectorXY,
	String,
	Array,
	DataTable,

	MaxCount
};


// SendProp::m_Flags.
namespace PropFlags
{
	// Unsigned integer data.
	static constexpr uint32_t Unsigned	= 1 << 0;
	// If this is set, the float/vector is treated like a world coordinate.
	static constexpr uint32_t Coord		= 1 << 1;

	// Note that the bit count is ignored in this case.
	// For floating point, don't scale into range, just take value as is.
	static constexpr uint32_t NoScale	= 1 << 2;
	// For floating point, limit high value to range minus one bit unit.
	static constexpr uint32_t RoundDown = 1 << 3;
	// For floating point, limit low value to range minus one bit unit.
	static constexpr uint32_t RoundUp	= 1 << 4;
	// If this is set, the vector is treated like a normal (only valid for vectors).
	static constexpr uint32_t Normal	= 1 << 5;
	// This is an exclude prop (not excludED, but it points at another prop to be excluded).
	static constexpr uint32_t Exclude	= 1 << 6;
	// Use XYZ/Exponent encoding for vectors.
	static constexpr uint32_t XYZ		= 1 << 7;

	// This tells us that the property is inside an array, so it shouldn't be put into the
	// flattened property list. Its array will point at it when it needs to.
	static constexpr uint32_t InsideArray = 1 << 8;

	// Set for datatable props using one of the default datatable proxies like
	// SendProxy_DataTableToDataTable that always send the data to all clients.
	static constexpr uint32_t ProxyAlways = 1 << 9;

	// this is an often changed field, moved to head of sendtable so it gets a small index.
	static constexpr uint32_t ChagesOften = 1 << 10;
	// Set automatically if SPROP_VECTORELEM is used.
	static constexpr uint32_t IsVecElement = 1 << 11;

	// Set automatically if it's a datatable with an offset of 0 that doesn't change the pointer
	// (ie: for all automatically-chained base classes).
	// In this case, it can get rid of this SendPropDataTable altogether and spare the
	// trouble of walking the hierarchy more than necessary.
	static constexpr uint32_t Collapsible	= 1 << 12;

	// Like PropFlags::Coord, but special handling for multiplayer games
	static constexpr uint32_t CoordMP		= 1 << 13;
	// Like PropFlags::Coord, but special handling for multiplayer games where the fractional component only gets a 3 bits instead of 5
	static constexpr uint32_t CoordMP_LP	= 1 << 14;
	// Like PropFlags::CoordMP, but coordinates are rounded to integral boundaries
	static constexpr uint32_t CoordMP_INT	= 1 << 15;

	// reuse existing flag so we don't break demo. note you want to include SPROP_UNSIGNED if needed, its more efficient
	static constexpr uint32_t VarInt = Normal;
};

class PropVariant
{
public:
	union
	{
		float			FloatVar;
		int				IntVar;
		const char*		StringVar;
		void*			PtrVar;
		float			VectorVar[3];
	};
	PropType		Type;
};


TF2_NAMESPACE_END();
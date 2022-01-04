#pragma once

#include <tf2/consts.hpp>

TF2_NAMESPACE_BEGIN();

class IBaseEntityInternal;
struct DataMapInputData;

namespace Const
{
	enum class DataMapFieldType
	{
		// No type or value
		Void,
		// Any floating point value
		Float,
		// A string ID (return from ALLOC_STRING)
		String,
		// Any vector, QAngle, or AngularImpulse
		Vector,
		// A quaternion
		Quaternion,
		// Any integer or enum
		Integer,
		// boolean, implemented as an int, I may use this as a hint for compression
		Boolean,
		// 2 byte integer
		Short,
		// a byte
		Character,
		// 8-bit per channel r,g,b,a (32bit color)
		Color32,
		// an embedded object with a datadesc, recursively traverse and embedded class/structure based on an additional typedescription
		Embedded,
		// special type that contains function pointers to it's read/write/parse functions
		Custom,

		// CBaseEntity *
		ClassPointer,
		// Entity handle
		EHandle,
		// edict_t *
		Edict,

		// A world coordinate (these are fixed up across level transitions automagically)
		WorldVector,
		// a floating point time (these are fixed up automatically too!)
		Time,
		// an integer tick count( fixed up similarly to time)
		Tick,
		// Engine string that is a model name (needs precache)
		ModelName,
		// Engine string that is a sound name (needs precache)
		SoundName,

		// a list of inputed data fields (all derived from CMultiInputVar)
		Input,
		// A class function pointer (Think, Use, etc)
		Function,

		// a vmatrix (output coords are NOT worldspace)
		VMatrix,

		// NOTE: Use float arrays for local transformations that don't need to be fixed up.
		// A VMatrix that maps some local space to world space (translation is fixed up on level transitions)
		VMatrix_WorldSpace,
		// matrix3x4_t that maps some local space to world space (translation is fixed up on level transitions)
		Matrix3x4_WorldSpace,

		// a start and range floating point interval ( e.g., 3.2->3.6 == 3.2 and 0.4 )
		Interval,
		// a model index
		ModelIndex,
		// a material index (using the material precache string table)
		MaterialIndex,

		// 2 floats
		Vector2D,

		Count
	};

	namespace DataMapTypeDesc
	{
		// This field is masked for global entity save/restore
		static constexpr uint32_t Global		= 1 << 0;
		// This field is saved to disk
		static constexpr uint32_t Save			= 1 << 1;
		// This field can be requested and written to by string name at load time
		static constexpr uint32_t Key			= 1 << 2;
		// This field can be written to by string name at run time, and a function called
		static constexpr uint32_t Input			= 1 << 3;
		// This field propogates it's value to all targets whenever it changes
		static constexpr uint32_t Output		= 1 << 4;
		// This is a table entry for a member function pointer
		static constexpr uint32_t FuncTable		= 1 << 5;
		// This field is a pointer, not an embedded object
		static constexpr uint32_t Pointer		= 1 << 6;
		// The field is an override for one in a base class (only used by prediction system for now)
		static constexpr uint32_t Override		= 1 << 7;

		// Flags used by other systems (e.g., prediction system)
		// This field is present in a network SendTable
		static constexpr uint32_t InSendTable	= 1 << 8;
		// The field is local to the client or server only (not referenced by prediction code and not replicated by networking)
		static constexpr uint32_t Private		= 1 << 9;
		// The field is part of the prediction typedescription, but doesn't get compared when checking for errors
		static constexpr uint32_t NoErrorCheck	= 1 << 10;

		// The field is a model index (used for debugging output)
		static constexpr uint32_t ModelIndex	= 1 << 11;
		// The field is an index into file data, used for byteswapping. 
		static constexpr uint32_t Index			= 1 << 12;

		// These flags apply to C_BasePlayer derived objects only
		// By default you can only view fields on the local player (yourself), but if this is set, then we allow you to see fields on other players
		static constexpr uint32_t ViewOtherPlayer = 1 << 13;
		// Only show this data if the player is on the same team as the local player
		static constexpr uint32_t ViewOwnTeam	= 1 << 14;
		// Never show this field to anyone, even the local player (unusual)
		static constexpr uint32_t ViewNever		= 1 << 15;

		enum class PackType
		{
			Normal,
			Packed,

			Count
		};
	}
}

struct EntityDataMap;
class ISaveRestoreOps;

// Function prototype for all input handlers.
using DataMapInputFunction = void(IBaseEntityInternal::*)(DataMapInputData&);



struct DM_TypeDescription
{
	Const::DataMapFieldType	Type;
	const char*			FieldName;
	int					Offset[2]; // 0 == normal, 1 == packed offset
	unsigned short		Size;
	short				Flags;
	// the name of the variable in the map/fgd data, or the name of the action
	const char*			ExternalName;
	// pointer to the function set for save/restoring of custom data types
	ISaveRestoreOps*	SaveRestoreOps;
	// for associating function with string names
	DataMapInputFunction InputFunction;
	// For embedding additional datatables inside this one
	EntityDataMap*		DataDesc;

	// Stores the actual member variable size in bytes
	int					SizeInBytes;

	// FTYPEDESC_OVERRIDE point to first baseclass instance if chains_validated has occurred
	DM_TypeDescription* OverrideField;

	// Used to track exclusion of baseclass fields
	int					OverrideCcount;

	// Tolerance for field errors for float fields
	float				Tolerance;
};


//-----------------------------------------------------------------------------
// stores the list of objects in the hierarchy
// used to iterate through an object's data descriptions
//-----------------------------------------------------------------------------
struct EntityDataMap
{
	DM_TypeDescription* DataDesc;
	int					DataNumFields;
	char const*			ClassName;
	EntityDataMap*		BaseMap;

	bool				ChainsValidated;
	// Have the "packed" offsets been computed
	bool				PackedOffsetsComputed;
	int					PackedSize;
};



namespace PropFinder
{
	/// <summary>
	/// Find Datamap on an entity
	/// </summary>
	/// <param name="pProp">[out] pointer to RecvProp*</param>
	/// <param name="pOffset">[out] pointer to offset</param>
	/// <returns>true if the prop exists, false otherwise</returns>
	PX_SDK_TF2
	bool FindDataMap(EntityDataMap* datamap, const char* prop_name, DM_TypeDescription** pDesc, uint32_t* pOffset);
}


TF2_NAMESPACE_END();

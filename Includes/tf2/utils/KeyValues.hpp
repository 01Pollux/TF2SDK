#pragma once

#include <memory>
#include <tf2/math/Vector.hpp>

TF2_NAMESPACE_BEGIN();

class IBaseFileSystem;
class IKeyValuesDumpContext;

enum class KeyValuesType : char
{
	None = 0,
	String,
	Int,
	Float,
	Pointer,
	WString,
	Color,
	UInt64,

	Count,
};

class KeyValues
{
public:
	PX_SDK_TF2 KeyValues(const char* setName);

	// Quick setup constructors
	PX_SDK_TF2 KeyValues(const char* setName, const char* firstKey, const char* firstValue);
	PX_SDK_TF2 KeyValues(const char* setName, const char* firstKey, const wchar_t* firstValue);
	PX_SDK_TF2 KeyValues(const char* setName, const char* firstKey, int firstValue);
	PX_SDK_TF2 KeyValues(const char* setName, const char* firstKey, const char* firstValue, const char* secondKey, const char* secondValue);
	PX_SDK_TF2 KeyValues(const char* setName, const char* firstKey, int firstValue, const char* secondKey, int secondValue);

	// Section name
	const char* GetName() const noexcept
	{
		return GetStringForSymbolFn(KeyName);
	}

	void SetName(const char* setName) noexcept
	{
		KeyName = GetSymbolForStringFn(setName, true);
	}

	PX_SDK_TF2 bool LoadFromFile(const char* fileName, const char* pathID = nullptr, bool refreshCache = false);

	// Find a keyValue, create it if it is not found.
	// Set bCreate to true to create the key if it doesn't already exist (which ensures a valid pointer will be returned)
	[[nodiscard]] PX_SDK_TF2 KeyValues* FindKey(const char* keyName, bool bCreate = false);
	[[nodiscard]] PX_SDK_TF2 const KeyValues* FindKey(const char* keyName) const;
	[[nodiscard]] PX_SDK_TF2 KeyValues* FindKey(int keySymbol) noexcept;
	[[nodiscard]] PX_SDK_TF2 const KeyValues* FindKey(int keySymbol) const noexcept;
	// Adds a subkey. Make sure the subkey isn't a child of some other keyvalues
	PX_SDK_TF2 void AddSubKey(KeyValues* pSubkey) noexcept;	
	// removes a subkey from the list, DOES NOT DELETE IT
	PX_SDK_TF2 void RemoveSubKey(KeyValues* subKey) noexcept;

	// Key iteration.
	//
	// NOTE: GetFirstSubKey/GetNextKey will iterate keys AND values. Use the functions 
	// below if you want to iterate over just the keys or just the values.
	//
	// returns the first subkey in the list
	[[nodiscard]] KeyValues* GetFirstSubKey()			noexcept { return SubKV; }
	[[nodiscard]] const KeyValues* GetFirstSubKey()	const noexcept { return SubKV; }
	// returns the next subkey
	[[nodiscard]] KeyValues* GetNextKey()				noexcept { return PeerKV; }
	[[nodiscard]] const KeyValues* GetNextKey()		const noexcept { return PeerKV; }

	void SetNextKey(KeyValues* pDat)	noexcept { PeerKV = pDat; }

	//
	// These functions can be used to treat it like a true key/values tree instead of 
	// confusing values with keys.
	//
	// So if you wanted to iterate all subkeys, then all values, it would look like this:
	//     for ( KeyValues *pKey = pRoot->GetFirstTrueSubKey(); pKey; pKey = pKey->GetNextTrueSubKey() )
	//     {
	//		   Msg( "Key name: %s\n", pKey->GetName() );
	//     }
	//     for ( KeyValues *pValue = pRoot->GetFirstValue(); pKey; pKey = pKey->GetNextValue() )
	//     {
	//         Msg( "Int value: %d\n", pValue->GetInt() );  // Assuming pValue->GetDataType() == TYPE_INT...
	//     }
	[[nodiscard]] PX_SDK_TF2 const KeyValues* GetFirstTrueSubKey() const noexcept;
	[[nodiscard]] PX_SDK_TF2 KeyValues* GetFirstTrueSubKey() noexcept;
	[[nodiscard]] PX_SDK_TF2 const KeyValues* GetNextTrueSubKey() const noexcept;
	[[nodiscard]] PX_SDK_TF2 KeyValues* GetNextTrueSubKey() noexcept;

	// When you get a value back, you can use GetX and pass in NULL to get the value.
	[[nodiscard]] PX_SDK_TF2 const KeyValues* GetFirstValue() const noexcept;
	[[nodiscard]] PX_SDK_TF2 KeyValues* GetFirstValue() noexcept;
	[[nodiscard]] PX_SDK_TF2 const KeyValues* GetNextValue() const noexcept;
	[[nodiscard]] PX_SDK_TF2 KeyValues* GetNextValue() noexcept;

	// Data access
	[[nodiscard]] int			GetInt(const char* keyName = nullptr, int defaultValue = 0) const noexcept;
	[[nodiscard]] uint64_t		GetUint64(const char* keyName = nullptr, uint64_t defaultValue = 0) const noexcept;
	[[nodiscard]] float			GetFloat(const char* keyName = nullptr, float defaultValue = 0.0f) const noexcept;
	[[nodiscard]] const char*	GetString(const char* keyName = nullptr, const char* defaultValue = "") const noexcept;
	[[nodiscard]] const wchar_t* GetWString(const char* keyName = nullptr, const wchar_t* defaultValue = L"") const noexcept;
	[[nodiscard]] void*			GetPtr(const char* keyName = nullptr, void* defaultValue = nullptr) const noexcept;
	[[nodiscard]] bool			GetBool(const char* keyName = nullptr, bool defaultValue = false, bool* optGotDefault = nullptr) const noexcept;
	[[nodiscard]] Color4_8		GetColor(const char* keyName = nullptr) const noexcept;
	[[nodiscard]] bool			IsEmpty(const char* keyName = nullptr) const noexcept;

	// Data access
	[[nodiscard]] int			GetInt(int keySymbol, int defaultValue = 0) const noexcept;
	[[nodiscard]] float			GetFloat(int keySymbol, float defaultValue = 0.0f) const noexcept;
	[[nodiscard]] const char*	GetString(int keySymbol, const char* defaultValue = "") const noexcept;
	[[nodiscard]] const wchar_t*GetWString(int keySymbol, const wchar_t* defaultValue = L"") const noexcept;
	[[nodiscard]] void*			GetPtr(int keySymbol, void* defaultValue = nullptr) const noexcept;
	[[nodiscard]] Color4_8		GetColor(int keySymbol) const noexcept;
	[[nodiscard]] bool			IsEmpty(int keySymbol) const noexcept;

	// Key writing
	PX_SDK_TF2 void SetWString(const char* keyName, const wchar_t* value);
	PX_SDK_TF2 void SetString(const char* keyName, const char* value);
	PX_SDK_TF2 void SetInt(const char* keyName, int value);
	PX_SDK_TF2 void SetUint64(const char* keyName, uint64_t value);
	PX_SDK_TF2 void SetFloat(const char* keyName, float value);
	PX_SDK_TF2 void SetPtr(const char* keyName, void* value);
	PX_SDK_TF2 void SetColor(const char* keyName, const Color4_8& value);
	void SetBool(const char* keyName, bool value) { SetInt(keyName, value ? 1 : 0); }

	// Allocate & create a new copy of the keys
	[[nodiscard]] PX_SDK_TF2 KeyValues* MakeCopy() const;
	PX_SDK_TF2 void CopySubkeys(KeyValues* pParent) const;

	// Allocate & create a new copy of the keys, including the next keys. This is useful for top level files
	// that don't use the usual convention of a root key with lots of children (like soundscape files).
	[[nodiscard]] PX_SDK_TF2 KeyValues* MakeCopy(bool copySiblings) const;
	// Make a new copy of all subkeys, add them all to the passed-in keyvalues
	// Clear out all subkeys, and the current value
	PX_SDK_TF2 void Clear();

	[[nodiscard]] PX_SDK_TF2 KeyValuesType GetDataType(const char* keyName = nullptr) const noexcept;

	// Virtual deletion function - ensures that KeyValues object is deleted from correct heap
	void DeleteThis() { delete this; }

	PX_SDK_TF2 void SetStringValue(char const* strValue);

	[[nodiscard]] PX_SDK_TF2 bool Dump(IKeyValuesDumpContext* pDump, int nIndentLevel = 0, bool bSorted = false) const;

	PX_SDK_TF2 void Init();

	int		KeyName;	// keyname is a symbol defined in KeyValuesSystem

	// These are needed out of the union because the API returns string pointers
	char* StringValue;
	wchar_t* WStringValue;

	// we don't delete these
	union
	{
		int		IntValue;
		float	FloatValue;
		void* PtrValue;
		uint8_t ColorValue[4];
	};

	KeyValuesType	DataType;
	char			HasEscapeSequences;
	char			EvaluateConditionals;

private: char	   _unused[1]{ }; public:

	KeyValues* PeerKV;	// pointer to next key in list
	KeyValues* SubKV;	// pointer to Start of a new sub key list
	KeyValues* ChainKV;	// Search here if it's not in our list

	static inline int (*GetSymbolForStringFn)(const char* name, bool bCreate) = nullptr;
	static inline const char* (*GetStringForSymbolFn)(int symbol) = nullptr;

	KeyValues(KeyValues&&) = default; KeyValues& operator=(KeyValues&&) = default;
	KeyValues(const KeyValues&) = delete; KeyValues& operator=(const KeyValues&) = delete;

protected:
	~KeyValues();
};

using UniqueKeyValues = std::unique_ptr<KeyValues, decltype([] (KeyValues* kv) { kv->DeleteThis(); })>;


class IKeyValuesDumpContext
{
public:
	virtual bool OnBeginKVSection(const KeyValues* pKey, int indentlvl) abstract;
	virtual bool OnKVSection(const KeyValues* pValue, int indentlvl) abstract;
	virtual bool OnEndKVSection(const KeyValues* pKey, int indentlvl) abstract;
};



inline int KeyValues::GetInt(int keySymbol, int defaultValue) const noexcept
{
	const KeyValues* dat = FindKey(keySymbol);
	return dat ? dat->GetInt(nullptr, defaultValue) : defaultValue;
}


inline float KeyValues::GetFloat(int keySymbol, float defaultValue) const noexcept
{
	const KeyValues* dat = FindKey(keySymbol);
	return dat ? dat->GetFloat(nullptr, defaultValue) : defaultValue;
}


inline const char* KeyValues::GetString(int keySymbol, const char* defaultValue) const noexcept
{
	const KeyValues* dat = FindKey(keySymbol);
	return dat ? dat->GetString(nullptr, defaultValue) : defaultValue;
}


inline const wchar_t* KeyValues::GetWString(int keySymbol, const wchar_t* defaultValue) const noexcept
{
	const KeyValues* dat = FindKey(keySymbol);
	return dat ? dat->GetWString(nullptr, defaultValue) : defaultValue;
}


inline void* KeyValues::GetPtr(int keySymbol, void* defaultValue) const noexcept
{
	const KeyValues* dat = FindKey(keySymbol);
	return dat ? dat->GetPtr(nullptr, defaultValue) : defaultValue;
}


inline Color4_8 KeyValues::GetColor(int keySymbol) const noexcept
{
	const KeyValues* dat = FindKey(keySymbol);
	return dat ? dat->GetColor() : Color4_8{};
}


inline bool  KeyValues::IsEmpty(int keySymbol) const noexcept
{
	const KeyValues* dat = FindKey(keySymbol);
	return dat ? dat->IsEmpty() : true;
}

TF2_NAMESPACE_END();
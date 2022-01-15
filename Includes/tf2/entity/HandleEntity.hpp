#pragma once

#include <tf2/consts.hpp>

TF2_NAMESPACE_BEGIN();

class IHandleEntity;

class IBaseHandle
{
public:
	IBaseHandle() = default;

	IBaseHandle(const IBaseHandle&) = default;
	IBaseHandle& operator=(const IBaseHandle&) = default;
	IBaseHandle(IBaseHandle&&) = default;
	IBaseHandle& operator=(IBaseHandle&&) = default;

	IBaseHandle(unsigned long value) noexcept : Index(value) { }
	IBaseHandle(int entry, int serial) noexcept : Index(entry | serial << Const::EHandle_EntryBits) { }
	
	[[nodiscard]] int entry_index() const noexcept { return Index & Const::EHandle_EntryMask; }
	[[nodiscard]] int serial_number() const noexcept { return Index >> Const::EHandle_EntryBits; }
	[[nodiscard]] bool valid_entity() const noexcept;

	[[nodiscard]] auto operator<=>(const IBaseHandle&) const = default;
	IBaseHandle& operator=(nullptr_t) { Index = Const::EHandle_EntryInvalid; return *this; }

	[[nodiscard]] operator bool() const noexcept
	{
		return Index != Const::EHandle_EntryInvalid;
	}

private:
	// The low NUM_SERIAL_BITS hold the index. If this value is less than MAX_EDICTS, then the entity is networkable.
	// The high NUM_SERIAL_NUM_BITS bits are the serial number.
	int Index{ Const::EHandle_EntryInvalid };
};

TF2_NAMESPACE_END();

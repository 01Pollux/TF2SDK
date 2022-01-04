#pragma once

#include "args.hpp"

PX_NAMESPACE_BEGIN();

class IPlugin;
class IConsoleManager;

class ConCommand
{
	friend class ConsoleManager;

public:
	template<typename _Ty>
	struct typeinfo;
	
	template<typename _Ty>
	struct typeclamp
	{
		bool	_HasMin{ }, _HasMax{ };
		_Ty		_Min{ },	_Max{ };
	};

	using ExecuteCallback_t = const char* (*)(ConCommand* pCmd, const CommandArgs& args);

	ConCommand(
		const char* cmd_name,
		const ExecuteCallback_t callback,
		const char* help_string = ""
	) noexcept : 
		m_CmdName(cmd_name),
		m_HelpString(help_string),

		m_ExecCallback(callback)
	{
		m_NextCommand = ConCommand::m_FirstCommand;
		ConCommand::m_FirstCommand = this;
	}

	ConCommand(ConCommand&&) = delete;		 ConCommand& operator=(ConCommand&&) = delete;
	ConCommand(const ConCommand&) = delete;  ConCommand& operator=(const ConCommand&) = delete;

	_NODISCARD IPlugin* plugin() const noexcept
	{
		return m_Plugin;
	}

	_NODISCARD const std::string_view& name() const noexcept
	{
		return m_CmdName;
	}

	_NODISCARD bool is_command() const noexcept
	{
		return m_IsCommand;
	}

	_NODISCARD constexpr bool has_description() const noexcept 
	{
		return description().size() > 0;
	}

	_NODISCARD constexpr const std::string_view& description() const noexcept
	{
		return m_HelpString;
	}

	_NODISCARD ExecuteCallback_t exec_callback() const noexcept
	{
		return m_ExecCallback;
	}

	/// <summary>
	/// Initialize current commands for current plugin
	/// </summary>
	static bool Init(IPlugin* plugin, IConsoleManager* console_manager);
	
private:
	IPlugin* m_Plugin{ };
	static inline ConCommand* m_FirstCommand{ };
	ConCommand* m_NextCommand;

	std::string_view m_CmdName;
	std::string_view m_HelpString;

	ExecuteCallback_t m_ExecCallback;

protected:
	static inline IConsoleManager* ConsoleManager{ };
	bool m_IsCommand : 1 { true };

	static void Print(const std::array<uint8_t, 4>& color, const std::string& str);
};


template<
	typename _Ty
>
class ConVar : public ConCommand
{
public:
	struct typeinfo;

	using value_type = _Ty;
	using reference = value_type&;
	using const_reference = const value_type&;
	using pointer = value_type*;
	using const_pointer = const value_type*;

	constexpr explicit ConVar(
		const char* convar_name,
		const value_type& defval = value_type{ },
		const char* description = "",

		ConCommand::ExecuteCallback_t callback = &ConVar::DefaultCommandExecute
	) :
		ConCommand(
			convar_name,
			callback,
			description
		),
		m_Value(defval)
	{ ConCommand::m_IsCommand = false; }

	_NODISCARD constexpr const char* type() const noexcept	{ return ConCommand::typeinfo<value_type>::type_name; }

	_NODISCARD constexpr const_reference get() const noexcept	{ return m_Value; }
	_NODISCARD reference get() noexcept							{ return m_Value; }

	_NODISCARD constexpr operator const_reference() const noexcept { return get(); }
	_NODISCARD operator reference() noexcept { return get(); }

	template<typename _OTy = value_type> const_reference operator=(const ConVar<_OTy>& o)	{ return (m_Value = static_cast<const_reference>(o.m_Value)); }
	template<typename _OTy = value_type> const_reference operator=(const _OTy& o)			{ return (m_Value = static_cast<const_reference>(o)); }

	template<typename _OTy = value_type> _NODISCARD auto operator<=>(const _OTy& o) const noexcept	{ return m_Value <=> static_cast<const_reference>(o); }
	template<typename _OTy = value_type> _NODISCARD bool operator!=(const _OTy& o) const noexcept	{ return m_Value != static_cast<const_reference>(o); }

	template<typename _OTy = value_type> _NODISCARD auto operator<=>(const ConVar<_OTy>& prop) const noexcept	{ return m_Value <=> static_cast<const_reference>(prop.m_Value); }
	template<typename _OTy = value_type> _NODISCARD bool operator!=(const ConVar<_OTy>& prop) const noexcept	{ return m_Value != static_cast<const_reference>(prop.m_Value); }

	ConVar& operator++() noexcept					{ ++m_Value; return *this; }
	ConVar& operator--() noexcept					{ --m_Value; return *this; }
	constexpr value_type operator++(int) noexcept	{ return m_Value++; }
	constexpr value_type operator--(int) noexcept	{ return m_Value--; }

	_NODISCARD constexpr value_type operator-() const noexcept { return -m_Value; }
	_NODISCARD constexpr value_type operator~() const noexcept { return ~m_Value; }

public:

#define PX_CONFIG_OPEARTOR(SYMBOL)																\
	constexpr ConVar& operator##SYMBOL##=(const ConVar& prop) noexcept							\
	{																							\
		m_Value SYMBOL##= prop.m_Value;															\
		return *this;																			\
	}																							\
	template<typename _OTy>																		\
	constexpr ConVar& operator##SYMBOL##=(const _OTy& o)	noexcept							\
	{																							\
		m_Value SYMBOL##= static_cast<const_reference>(o);										\
		return *this;																			\
	}																							\
	_NODISCARD constexpr auto operator##SYMBOL(const ConVar& prop) const noexcept				\
	{																							\
		return m_Value SYMBOL prop.m_Value;														\
	}																							\
	template<typename _OTy>																		\
	_NODISCARD constexpr auto operator##SYMBOL(const _OTy& o) const noexcept					\
	{																							\
		return m_Value SYMBOL static_cast<const_reference>(o);									\
	}

	PX_CONFIG_OPEARTOR(+);
	PX_CONFIG_OPEARTOR(-);
	PX_CONFIG_OPEARTOR(/);
	PX_CONFIG_OPEARTOR(*);
	PX_CONFIG_OPEARTOR(%);
	PX_CONFIG_OPEARTOR(&);
	PX_CONFIG_OPEARTOR(|);
	PX_CONFIG_OPEARTOR(^);
	PX_CONFIG_OPEARTOR(<<);
	PX_CONFIG_OPEARTOR(>>);

#undef PX_CONFIG_OPEARTOR

public:
	_NODISCARD auto data() const noexcept
	{
		if constexpr (requires(const_reference v) { v.data(); })
			return m_Value.data();
		else
			return &m_Value;
	}

	_NODISCARD auto data() noexcept
	{
		if constexpr (requires(reference v) { v.data(); })
			return m_Value.data();
		else
			return &m_Value;
	}

	_NODISCARD auto begin() const noexcept
	{
		if constexpr (requires(const_reference v) { std::begin(v); })
			return std::begin(m_Value);
		else
			return &m_Value;
	}

	_NODISCARD auto begin() noexcept
	{
		if constexpr (requires(reference v) { std::begin(v); })
			return std::begin(m_Value);
		else
			return &m_Value;
	}

	_NODISCARD auto end() const noexcept
	{
		if constexpr (requires(const_reference v) { std::end(v); })
			return std::end(m_Value);
		else
			return begin() + sizeof(value_type);
	}

	_NODISCARD std::string str() const
	{
		return CommandParser<value_type>::to_string(get());
	}

public:
	pointer operator->() noexcept { return &m_Value; }
	constexpr const_pointer operator->() const noexcept { return &m_Value; }

	template<typename _IdxType = ptrdiff_t>
	_NODISCARD const auto& operator[](const _IdxType& _idx) const noexcept
	{
		return m_Value[_idx];
	}

	template<typename _IdxType = ptrdiff_t>
	_NODISCARD auto& operator[](const _IdxType& _idx) noexcept
	{
		return m_Value[_idx];
	}

	static const char* DefaultCommandExecute(ConCommand* pCmd, const CommandArgs& args)
	{
		ConVar* pCvar = static_cast<ConVar*>(pCmd);
		if (args.has_val())
		{
			pCvar->m_Value = args.get_val<value_type>();
		}
		else
		{
			ConCommand::Print(
				{ 12, 255, 87, 255 },
				pCvar->str()
			);
		}
		return nullptr;
	}

private:
	value_type	m_Value;
};

#define PX_COMMAND(Name, ...)															\
static const char* Name##_callback(px::ConCommand* pCmd, const px::CommandArgs& args);	\
static px::ConCommand Name##_cmd{														\
	#Name,																				\
	Name##_callback,																	\
	__VA_ARGS__																			\
};																						\
const char* Name##_callback(px::ConCommand* pCmd, const px::CommandArgs& args)

PX_NAMESPACE_END();
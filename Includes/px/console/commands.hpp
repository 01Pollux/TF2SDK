#pragma once

#include "args.hpp"

class ConsoleManager;

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

	[[nodiscard]] IPlugin* plugin() const noexcept
	{
		return m_Plugin;
	}

	[[nodiscard]] const std::string_view& name() const noexcept
	{
		return m_CmdName;
	}

	[[nodiscard]] bool is_command() const noexcept
	{
		return m_IsCommand;
	}

	[[nodiscard]] constexpr bool has_description() const noexcept 
	{
		return description().size() > 0;
	}

	[[nodiscard]] constexpr const std::string_view& description() const noexcept
	{
		return m_HelpString;
	}

	[[nodiscard]] ExecuteCallback_t exec_callback() const noexcept
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
	) noexcept(std::is_nothrow_constructible_v<value_type>) :
		ConCommand(
			convar_name,
			callback,
			description
		),
		m_Value(defval)
	{ ConCommand::m_IsCommand = false; }

	[[nodiscard]] constexpr const char* type() const noexcept	{ return ConCommand::typeinfo<value_type>::type_name; }

	[[nodiscard]] constexpr const_reference get() const noexcept	{ return m_Value; }
	[[nodiscard]] reference get() noexcept							{ return m_Value; }

	[[nodiscard]] constexpr operator const_reference() const noexcept { return get(); }
	[[nodiscard]] operator reference() noexcept { return get(); }

	template<typename _OTy = value_type> const_reference operator=(const ConVar<_OTy>& o)	{ return (m_Value = static_cast<const_reference>(o.m_Value)); }
	template<typename _OTy = value_type> const_reference operator=(const _OTy& o)			{ return (m_Value = static_cast<const_reference>(o)); }

	template<typename _OTy = value_type> [[nodiscard]] auto operator<=>(const _OTy& o) const noexcept	{ return m_Value <=> static_cast<const_reference>(o); }
	template<typename _OTy = value_type> [[nodiscard]] bool operator!=(const _OTy& o) const noexcept	{ return m_Value != static_cast<const_reference>(o); }

	template<typename _OTy = value_type> [[nodiscard]] auto operator<=>(const ConVar<_OTy>& prop) const noexcept	{ return m_Value <=> static_cast<const_reference>(prop.m_Value); }
	template<typename _OTy = value_type> [[nodiscard]] bool operator!=(const ConVar<_OTy>& prop) const noexcept	{ return m_Value != static_cast<const_reference>(prop.m_Value); }

	ConVar& operator++() noexcept					{ ++m_Value; return *this; }
	ConVar& operator--() noexcept					{ --m_Value; return *this; }
	constexpr value_type operator++(int) noexcept	{ return m_Value++; }
	constexpr value_type operator--(int) noexcept	{ return m_Value--; }

	[[nodiscard]] constexpr value_type operator-() const noexcept { return -m_Value; }
	[[nodiscard]] constexpr value_type operator~() const noexcept { return ~m_Value; }

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
	[[nodiscard]] constexpr auto operator##SYMBOL(const ConVar& prop) const noexcept			\
	{																							\
		return m_Value SYMBOL prop.m_Value;														\
	}																							\
	template<typename _OTy>																		\
	[[nodiscard]] constexpr auto operator##SYMBOL(const _OTy& o) const noexcept					\
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
	[[nodiscard]] auto data() const noexcept
	{
		if constexpr (requires(const_reference v) { v.data(); })
			return m_Value.data();
		else
			return &m_Value;
	}

	[[nodiscard]] auto data() noexcept
	{
		if constexpr (requires(reference v) { v.data(); })
			return m_Value.data();
		else
			return &m_Value;
	}

	[[nodiscard]] auto begin() const noexcept
	{
		if constexpr (requires(const_reference v) { std::begin(v); })
			return std::begin(m_Value);
		else
			return &m_Value;
	}

	[[nodiscard]] auto begin() noexcept
	{
		if constexpr (requires(reference v) { std::begin(v); })
			return std::begin(m_Value);
		else
			return &m_Value;
	}

	[[nodiscard]] auto end() const noexcept
	{
		if constexpr (requires(const_reference v) { std::end(v); })
			return std::end(m_Value);
		else
			return begin() + sizeof(value_type);
	}

	[[nodiscard]] std::string str() const
	{
		return CommandParser<value_type>::to_string(get());
	}

public:
	[[nodiscard]] pointer operator->() noexcept { return &m_Value; }
	[[nodiscard]] constexpr const_pointer operator->() const noexcept { return &m_Value; }

	template<typename _IdxType = ptrdiff_t>
	[[nodiscard]] const auto& operator[](const _IdxType& _idx) const noexcept
	{
		return m_Value[_idx];
	}

	template<typename _IdxType = ptrdiff_t>
	[[nodiscard]] auto& operator[](const _IdxType& _idx) noexcept
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
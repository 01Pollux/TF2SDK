#pragma once

#include <array>
#include <px/interfaces/InterfacesSys.hpp>
#include <px/console/commands.hpp>

PX_NAMESPACE_BEGIN();

static constexpr const char* Interface_ConsoleManager = "IConsoleManager";

class IConsoleManager : public IInterface
{
	friend class ConCommand;

protected:
	/// <summary>
	/// Add console commands to plugin's commands
	/// </summary>
	virtual bool AddCommands(ConCommand* command) abstract;

public:
	/// <summary>
	/// remove console command from plugin's commands
	/// </summary>
	virtual bool RemoveCommand(ConCommand* command) abstract;

	/// <summary>
	/// remove plugin's commands
	/// </summary>
	virtual bool RemoveCommands(IPlugin* plugin) abstract;

	/// <summary>
	/// find console command by name
	/// </summary>
	virtual ConCommand* FindCommand(const std::string_view& name) abstract;

	/// <summary>
	/// find console commands by plugin
	/// </summary>
	virtual std::vector<ConCommand*> FindCommands(IPlugin* plugin) abstract;

	/// <summary>
	/// find console commands by plugin
	/// </summary>
	virtual std::vector<ConCommand*> FindCommands(const std::string_view& name) abstract;

	/// <summary>
	/// execute console command
	/// </summary>
	virtual void Execute(const std::string_view& cmds) abstract;

	/// <summary>
	/// clear entries in console
	/// </summary>
	virtual void Clear(size_t size = 0, bool is_history = false) abstract;

	/// <summary>
	/// output message to console with colors
	/// </summary>
	virtual void Print(uint32_t color, const std::string& msg) abstract;

	/// <summary>
	/// output message to console
	/// </summary>
	void Print(const std::array<uint8_t, 4>& color, const std::string& msg)
	{
		Print(
			color[0] | color[1] << 0x8 | color[2] << 0x10 | color[3] << 0x18,
			msg
		);
	}

	/// <summary>
	/// output message to console
	/// </summary>
	void Print(const std::string& msg)
	{
		Print(
			255 | 255 << 0x8 | 255 << 0x10 | 255 << 0x18,
			msg
		);
	}
};


inline bool ConCommand::Init(IPlugin* plugin, IConsoleManager* console_manager)
{
	bool init = false;
	ConCommand::ConsoleManager = console_manager;
	if (m_FirstCommand)
	{
		m_FirstCommand->m_Plugin = plugin;
		init = console_manager->AddCommands(m_FirstCommand);
		m_FirstCommand = nullptr;
	}
	return init;
}

inline void ConCommand::Print(const std::array<uint8_t, 4>& color, const std::string& str)
{
	ConCommand::ConsoleManager->Print(
		color,
		str
	);
}

PX_NAMESPACE_END();
#pragma once

#include <array>
#include <px/interfaces/InterfacesSys.hpp>
#include <px/cmds.hpp>

PX_NAMESPACE_BEGIN();

static constexpr const char* Interface_ConsoleManager = "IConsoleManager";

class IConsoleManager : public IInterface
{
public:
	/// <summary>
	/// Adds console commands registered by the plugin
	/// </summary>
	void AddCommands(IPlugin* plugin)
	{
		AddCommands(plugin, cmd_manager::begin(), cmd_manager::end());
		cmd_manager::clear();
	}

	/// <summary>
	/// remove console command from plugin's commands
	/// </summary>
	virtual bool RemoveCommand(con_command* command) abstract;

	/// <summary>
	/// remove plugin's commands
	/// </summary>
	virtual bool RemoveCommands(IPlugin* plugin) abstract;

	/// <summary>
	/// find console commands by plugin
	/// </summary>
	[[nodiscard]] virtual std::vector<con_command*> FindCommands(IPlugin* plugin) abstract;

	/// <summary>
	/// find console command by name
	/// </summary>
	[[nodiscard]] virtual con_command* FindCommand(std::string_view name) abstract;

	/// <summary>
	/// execute console command
	/// </summary>
	virtual void Execute(std::string_view cmds) abstract;

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

protected:
	/// <summary>
	/// Add console commands to plugin's commands
	/// </summary>
	virtual void AddCommands(
		IPlugin* plugin, 
		con_command::entries_type::iterator begin,
		con_command::entries_type::iterator end
	) abstract;
};

PX_NAMESPACE_END();
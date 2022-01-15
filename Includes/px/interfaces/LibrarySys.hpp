#pragma once

#include <px/intptr.hpp>
#include <px/string.hpp>

#include <asmjit/core/api-config.h>
#include "InterfacesSys.hpp"

_STD_BEGIN;

template<typename>
class function;

_STD_END;

#ifndef ASMJIT_NO_JIT
ASMJIT_BEGIN_NAMESPACE
class JitRuntime;
ASMJIT_END_NAMESPACE
#endif

PX_NAMESPACE_BEGIN();

namespace profiler
{
	class manager;
}

class IGameData;

static constexpr const char* Interface_ILibrary = "ILibraryManager";

class ILibrary : public IInterface
{
public:
	/// <summary>
	/// read a signature from pattern
	/// </summary>
	/// <returns>pointer to the target address, null if it doesn't exists</returns>
	[[nodiscard]] virtual IntPtr FindByName(std::string_view name) abstract;

	/// <summary>
	/// read a signature from pattern
	/// </summary>
	/// <returns>pointer to the target address, null if it doesn't exists</returns>
	[[nodiscard]] virtual IntPtr FindBySignature(std::string_view signature) abstract;

	using SignaturePredicate = std::function<bool(IntPtr cur_address, IntPtr end_address)>;
	/// <summary>
	/// read a signature from predicate function
	/// </summary>
	/// <returns>pointer to the target address, null if it doesn't exists</returns>
	[[nodiscard]] virtual IntPtr FindBySignature(const SignaturePredicate& signature) abstract;

	/// <summary>
	/// read a signature from a string
	/// </summary>
	/// <returns>pointer to the target address, null if it doesn't exists</returns>
	[[nodiscard]] virtual IntPtr FindByString(std::string_view str) abstract;
};

enum class PlDirType
{
	Root,	// main executable directory
	Main,	// ./Pleiades/ directory
	Plugins,// ./Pleiades/Plugins
	Logs,	// ./Pleiades/Logs
	Profiler,//./Pleiades/Logs/Profiler
	Sounds	// ./Pleiades/Sounds
};

class ILibraryManager : public IInterface
{
public:
	static constexpr const char* MainName	= "pleiades";
	static constexpr const char* MainDir	= "pleiades";
	static constexpr const char* MainCfg	= "pleiades/Pleiades.json";
	static constexpr const char* CommonTag	= "pleiades/Pleiades";
	static constexpr const char* PluginsDir = "pleiades/plugins";
	static constexpr const char* SoundsDir	= "pleiades/pounds";
	static constexpr const char* ConfigDir	= "pleiades/ponfigs";
	static constexpr const char* LogsDir	= "pleiades/logs";
	static constexpr const char* MainLog	= "Pleiades";
	static constexpr const char* ProfilerDir= "pleiades/logs/Profiler";

	/// <summary>
	/// read a pre-loaded library
	/// </summary>
	/// <returns>pointer to the module, null if it doesn't exists</returns>
	[[nodiscard]] virtual ILibrary* ReadLibrary(const std::string_view module_name) abstract;

	/// <summary>
	/// load a module to the process
	/// </summary>
	/// <param name="load_in_memory">Load dll without LoadLibraryA</param>
	/// <returns>pointer to the module, null if it doesn't exists</returns>
	[[nodiscard]] virtual ILibrary* OpenLibrary(const std::wstring_view module_name, bool manual_map) abstract;

	/// <summary>
	/// load a module to the process
	/// </summary>
	/// <param name="load_in_memory">Load dll without LoadLibraryA</param>
	/// <returns>pointer to the module, null if it doesn't exists</returns>
	[[nodiscard]] ILibrary* OpenLibrary(const std::string_view module_name, bool manual_map)
	{
		return OpenLibrary(px::string_cast<std::wstring>(module_name), manual_map);
	}

	/// <summary>
	/// Get desired directory from type
	/// </summary>
	/// <param name="type">directory type</param>
	/// <param name="path">output buffer</param>
	/// <param name="max_len">buffer length</param>
	/// <param name="extra_path">extra directory to travel to</param>
	/// <returns>true if directory exists, false otherwise, exception output will be displayed in output buffer 'path'</returns>
	[[nodiscard]] virtual std::string GoToDirectory(PlDirType type, std::string_view extra_path = "") abstract;

	/// <summary>
	/// Open gamedata for the plugin '<PLUGIN_NAME>.<GAME_NAME>.gamedata', or null to open 'Pleiades.<GAME_NAME>.gamedata'
	/// </summary>
	/// <returns>pointer to the gamedata, and must be delete afterwards</returns>
	[[nodiscard]] virtual IGameData* OpenGameData(IPlugin* pPlugin) abstract;

	/// <summary>
	/// Get current host name
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] virtual std::string_view GetHostName() abstract;

	/// <summary>
	/// Get AsmJit main runtime
	/// </summary>
	/// <returns></returns>
	[[nodiscard]] virtual asmjit::JitRuntime* GetRuntime() abstract;

	/// <summary>
	/// Get last error for current os
	/// </summary>
	[[nodiscard]] virtual std::string GetLastError() abstract;

	/// <summary>
	/// Get main profiler instance
	/// </summary>
	[[nodiscard]] virtual profiler::manager* GetProfiler() abstract;
};

PX_NAMESPACE_END();
#pragma once

#include <px/defines.hpp>
#include <px/version.hpp>

PX_NAMESPACE_BEGIN();

class IInterface
{
public:
	virtual ~IInterface() = default;

	IInterface() = default;
	IInterface(const IInterface&) = delete; IInterface& operator=(const IInterface&) = delete;
	IInterface(IInterface&&) = delete; IInterface& operator=(IInterface&&) = delete;
};

enum class PlCfgLoadType
{
	Save,			// modify variables without actually saving
	Load,			// request update to the current variables
};

class IPlugin;

class IPluginManager : public IInterface
{
public:
	/// <summary>
	/// Expose interface by name.
	/// </summary>
	/// <returns>true if the interface does exists, false otherwise</returns>
	[[nodiscard]] virtual bool ExposeInterface(const std::string& name, IInterface* iface, IPlugin* owner) abstract;

	/// <summary>
	/// Request an interface by name.
	/// </summary>
	/// <returns>true if the interface does exists, false otherwise</returns>
	[[nodiscard]] virtual bool RequestInterface(const std::string& iface_name, IInterface** iface) abstract;

	/// <summary>
	/// find plugin by either it's file name
	/// </summary>
	[[nodiscard]] virtual IPlugin* FindPlugin(const std::string& name) abstract;

	/// <summary>
	/// find and add plugin to caller's dependencies
	/// </summary>
	/// <param name="is_required">true to immediatly load the plugin if it's not loaded</param>
	/// <returns>true if the plugin was bounded, false otherwise</returns>
	virtual bool BindPlugin(IPlugin* caller, const char* name, bool is_required) abstract;

	/// <summary>
	/// Update plugin config.
	/// </summary>
	/// <param name="plugin">pointer to the plugin, nullptr if you want to update every plugin</param>
	[[nodiscard]] virtual void UpdatePluginConfig(IPlugin* plugin, PlCfgLoadType loadtype) abstract;

	/// <summary>
	/// force unload the plugin.
	/// </summary>
	[[nodiscard]] virtual void RequestShutdown(IPlugin* iface) abstract;

	/// <summary>
	/// Unload main plugin and all of its subplugins.
	/// </summary>
	virtual void Shutdown() abstract;

	/// <summary>
	/// Get plugin host version.
	/// </summary>
	[[nodiscard]] virtual version GetHostVersion() abstract;

	/// <summary>
	/// Load and init a plugin outside of its host.
	/// </summary>
	/// <returns>true if the plugin was successfully loaded, false otherwise</returns>
	virtual IPlugin* LoadPlugin(const std::string& name) abstract;

	/// <summary>
	/// unload plugin outside of its host.
	/// </summary>
	/// <returns>true if the plugin was successfully unloaded, false otherwise</returns>
	virtual bool UnloadPlugin(const std::string& name) abstract;
};

PX_NAMESPACE_END();